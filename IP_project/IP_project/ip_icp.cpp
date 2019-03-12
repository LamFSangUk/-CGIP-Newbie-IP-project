#include"ip_icp.h"

template <typename TYPE>
IPICP<TYPE>::IPICP() {
	m_ref = nullptr;
	m_flt = nullptr;
	m_ref_obj = nullptr;
	m_flt_obj = nullptr;

	// Initialize the transformation matrix to Identity
	m_trans.setIdentity();
}

template <typename TYPE>
IPICP<TYPE>::~IPICP() {
}

template <typename TYPE>
void IPICP<TYPE>::calculateInitT() {
	assert(m_ref_obj != nullptr);
	assert(m_flt_obj != nullptr);

	calculateCOI(m_ref_obj, &m_ref_coi);
	calculateCOI(m_flt_obj, &m_flt_coi);

	point sub = m_ref_coi - m_flt_coi;
	sub[3] = 1; // to make homogeneous form

	m_trans.rightCols<1>() = sub;
	//std::cout << m_trans * m_flt_coi << std::endl;
	//std::cout << m_ref_coi << std::endl;

	//std::cout << m_trans << std::endl;
}

template <typename TYPE>
void IPICP<TYPE>::calculateCOI(mc::image3d<TYPE>* img, point* coi) {
	// calculate center of mass
	unsigned int sum_x = 0;
	unsigned int sum_y = 0;
	unsigned int sum_z = 0;
	unsigned int obj_size = 0;

	for (int i = 0; i < img->depth(); i++) {
		for (int j = 0; j < img->height(); j++) {
			for (int k = 0; k < img->width(); k++) {
				if (img->get(k, j, i) != 0) {// Object
					obj_size++;
					sum_x += k;
					sum_y += j;
					sum_z += i;
				}
			}
		}
	}

	sum_x /= obj_size;
	sum_y /= obj_size;
	sum_z /= obj_size;

	*coi << sum_x, sum_y, sum_z, 1; // (x,y,z,1) , homogeneous form
}

template <typename TYPE>
void IPICP<TYPE>::iterate() {
	transform(m_flt_obj);
}

template <typename TYPE>
void IPICP<TYPE>::transform(mc::image3d<TYPE>* img) {
	assert(img != nullptr);
	TYPE** trans_data;
	trans_data = SAFE_ALLOC_VOLUME(TYPE, img->depth(), img->height()*img->width());

	for (int i = 0; i < img->depth(); i++) {
		for (int j = 0; j < img->height(); j++) {
			for (int k = 0; k < img->width(); k++) {
				short cur_x = k;
				short cur_y = j;
				short cur_z = i;
				TYPE cur_val = img->get(cur_x, cur_y, cur_z);

				if (cur_val != 0) {// incase object
					point cur; cur << cur_x, cur_y, cur_z, 1;
					point trans = m_trans * cur;
					
					short trans_x = (short)(trans[0] / trans[3]);
					short trans_y = (short)(trans[1] / trans[3]);
					short trans_z = (short)(trans[2] / trans[3]);

					if (trans_x < 0 || trans_x >= img->width()
						|| trans_y < 0 || trans_y >= img->height()
						|| trans_z < 0 || trans_z >= img->depth()) {
						continue;
					}

					trans_data[trans_z][trans_y * img->width() + trans_x] = cur_val;
				}
			}
		}
	}

	MEMCPY_VOLUME(TYPE, img->data(), trans_data, img->depth(), img->height()*img->width());
}

template <typename TYPE>
void IPICP<TYPE>::transformFltImg() {
	transform(m_flt);
}

template <typename TYPE>
void IPICP<TYPE>::calculateSimilarity() {
	assert(m_flt_obj != nullptr);
	assert(m_ref_distance_map != nullptr);

	unsigned long sum_distance = 0;
	unsigned int count = 0;

	for (int i = 0; i < m_flt_obj->depth(); i++) {
		for (int j = 0; j < m_flt_obj->height(); j++) {
			for (int k = 0; k < m_flt_obj->width(); k++) {
				short cur_x = k;
				short cur_y = j;
				short cur_z = i;

				if ((m_flt_obj->get(cur_x, cur_y, cur_z) != 0)
					&& cur_x<m_ref_obj->width()
					&& cur_y<m_ref_obj->height()
					&& cur_z<m_ref_obj->depth()) {
					short distance = m_ref_distance_map[cur_z][cur_y * m_ref_obj->width() + cur_x];
					sum_distance += distance;
					count++;
				}
			}
		}
	}

	std::cout << sum_distance << std::endl;

	m_avg_dist = sum_distance / count;
	std::cout << m_avg_dist << std::endl;
}