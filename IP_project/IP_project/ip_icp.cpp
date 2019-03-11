#include"ip_icp.h"

template <typename TYPE>
IPICP<TYPE>::IPICP() {
	m_ref = nullptr;
	m_flt = nullptr;

	// Initialize the transformation matrix to Identity
	m_trans.setIdentity();
}

template <typename TYPE>
IPICP<TYPE>::~IPICP() {
}

template <typename TYPE>
void IPICP<TYPE>::calculateInitT() {
	assert(m_ref != nullptr);
	assert(m_flt != nullptr);

	calculateCOI(m_ref, &m_ref_coi);
	calculateCOI(m_flt, &m_flt_coi);

	point sub = m_flt_coi - m_ref_coi;
	sub[3] = 1; // to make homogeneous form

	m_trans.rightCols<1>() = sub;

	std::cout << m_trans << std::endl;
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

	*coi << sum_x, sum_y, sum_z, 1;
}