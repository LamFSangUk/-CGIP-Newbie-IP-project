#include"ip_registration.h"

template <typename TYPE>
IPRegistration<TYPE>::IPRegistration() {
	m_ref = nullptr;
	m_flt = nullptr;
	m_ref_obj = nullptr;
	m_flt_obj = nullptr;

	// Initialize the transformation matrix to Identity
	m_trans.setIdentity();
}

template <typename TYPE>
IPRegistration<TYPE>::~IPRegistration() {
}

template <typename TYPE>
void IPRegistration<TYPE>::calculateInit() {
	assert(m_ref_obj != nullptr);
	assert(m_flt_obj != nullptr);

	calculateCOI(m_ref_obj, &m_ref_coi);
	calculateCOI(m_flt_obj, &m_flt_coi);

	//Point sub = m_ref_coi - m_flt_coi;
	//sub[3] = 1; // to make homogeneous form

	//m_trans.rightCols<1>() = sub;
	//std::cout << m_trans * m_flt_coi << std::endl;
	//std::cout << m_ref_coi << std::endl;

	//std::cout << m_trans << std::endl;
}

template <typename TYPE>
void IPRegistration<TYPE>::calculateCOI(mc::image3d<TYPE>* img, Point* coi) {
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
void IPRegistration<TYPE>::makeFltObjectPointList() {
	for (int i = 0; i < m_flt_obj->depth(); i++) {
		for (int j = 0; j < m_flt_obj->height(); j++) {
			for (int k = 0; k < m_flt_obj->width(); k++) {
				if (m_flt_obj->get(k, j, i) != 0) { // Object
					Point p; p << k, j, i, 1;
					p = p - m_flt_coi; // set the COI has coordinate (0,0,0)
					p[3] = 1;
					m_flt_obj_points.push_back(p);
				}
			}
		}
	}
}

template <typename TYPE>
double IPRegistration<TYPE>::calculateSimilarity(std::vector<Point>& moved) {
	assert(m_ref_distance_map != nullptr);

	unsigned int count_valid = 0;
	unsigned int sum_distance = 0;

	for (Point p : moved) {
		// p(x,y,z,1)
		p = p + m_flt_coi;

		short cur_x = p[0];
		short cur_y = p[1];
		short cur_z = p[2];

		if (cur_x >= 0 && cur_x < m_ref->width()
			&& cur_y >= 0 && cur_y < m_ref->height()
			&& cur_z >= 0 && cur_z < m_ref->depth()) {

			count_valid++;

			short distance = m_ref_distance_map[cur_z][cur_y * m_ref_obj->width() + cur_x];
			sum_distance += distance;
		}
	}

	assert(count_valid != 0);

	double avg_distance = (double)sum_distance / count_valid;
	return avg_distance;
}

template <typename TYPE>
Eigen::Matrix4d IPRegistration<TYPE>::makeTransformMatrix(int t_param,double degree) {
	// t_param is a transformation parameter, 0 to 5 value indicates Tx,Ty,Tz,Rx,Ry,Rz.
	Eigen::Matrix4d mat;
	mat.setIdentity();

	if (t_param / 3 == 0) { // translation
		Eigen::Vector4d v; v << 0, 0, 0, 1;
		v[t_param % 3] = degree;

		mat.rightCols<1>() = v;
	}
	else { // rotation
		double radian = degree * M_PI / 180;
		
		if (t_param == 3) {
			mat(1,1) = mat(2,2) = cos(radian);
			mat(2,1) = sin(radian);
			mat(1,2) = -sin(radian);
		}
		else if (t_param == 4) {
			mat(0,0) = mat(2,2) = cos(radian);
			mat(0,2) = sin(radian);
			mat(2,0) = -sin(radian);
		}
		else {
			mat(0,0) = mat(1,1) = cos(radian);
			mat(1,0) = sin(radian);
			mat(0,1) = -sin(radian);
		}
	}

	return mat;
}

template <typename TYPE>
void IPRegistration<TYPE>::iterate() {
	double similarity = calculateSimilarity(m_flt_obj_points);
	double prev_similarity = similarity;
	double threshold = 1e-6;
	int count_loop = 1;
	int count_same_loop = 0;
	int count_reduce_degree = 0;
	int degree_scale = 1;
	
#ifdef _DEBUG
	std::cout << std::fixed;
	std::cout.precision(6);
	std::cout << " - Default similarity metric : " << similarity << std::endl;
	std::cout << " - Default degree : " << 1.0f / degree_scale << std::endl;
#endif

	while (true) {
#ifdef _DEBUG
		std::cout << " --- Loop #" << count_loop++ << std::endl;
#endif

		for (int i = 0; i < 6; i++) {
			Eigen::Matrix4d min_transform_mat;
			min_transform_mat.setIdentity();

			for (int degree = -2; degree <= 2; degree++) {
				if (degree == 0) continue;// skip the 0 degree
				// finding local minimum

				std::vector<Point> target(m_flt_obj_points);
				Eigen::Matrix4d mat = makeTransformMatrix(i, (double)degree/degree_scale);

				transform(&target,mat);
				double transform_similarity = calculateSimilarity(target);
				if (transform_similarity < similarity) {
					similarity = transform_similarity;
					min_transform_mat = mat;
				}

			}

#ifdef _DEBUG
			std::cout << " - Similarity metric : " << similarity << std::endl;
			std::cout << " - Transform Matrix : " << std::endl;
			std::cout << min_transform_mat << std::endl << std::endl;
#endif
			m_trans = min_transform_mat * m_trans;
			transform(&m_flt_obj_points, min_transform_mat);
		}

#ifdef _DEBUG
		std::cout << " prev-cur : " << prev_similarity-similarity << std::endl;
		std::cout << " threshold : " <<threshold<< std::endl;
#endif

		if (prev_similarity - similarity < threshold) {	// In case very similar
			
			count_same_loop++;

			if (count_same_loop > 1) break;				// break the loop when m_trans has no change twice time continuously.
			else if (count_reduce_degree > 1) break;	// break the loop when degree is smaller than 0.01
			else {
				count_reduce_degree++;
				degree_scale *= 10;						// to perform transforming with low degree
				count_same_loop = 0;

#ifdef _DEBUG
				std::cout << " - Decrease degree" << std::endl;
				std::cout << " - To : " << 1.0f / degree_scale << std::endl << std::endl;
#endif
			}
		}
		else { // In case there was some change
			count_same_loop = 0;
		}
		prev_similarity = similarity;

	}

	std::cout << m_trans << std::endl;
}

template <typename TYPE>
void IPRegistration<TYPE>::transform(std::vector<Point>* target,Eigen::Matrix4d mat) {// transform flt_obj
	for (Point& p : *target) {
		p = mat * p;
		p[0] = p[0] / p[3];
		p[1] = p[1] / p[3];
		p[2] = p[2] / p[3];
		p[3] = 1;
	}
}

template <typename TYPE>
void IPRegistration<TYPE>::transform(mc::image3d<TYPE>* img) {
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
					Point cur; cur << cur_x, cur_y, cur_z, 1;
					Point trans = m_trans * cur;
					
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
void IPRegistration<TYPE>::transformFltImg() {
	transform(m_flt);
}