#include "ip_substraction.h"

template <typename TYPE>
IPSubstraction<TYPE>::IPSubstraction(mc::image3d<TYPE>*ref, mc::image3d<TYPE>*flt) {
	m_ref = ref;
	m_flt = flt;
}

template <typename TYPE>
IPSubstraction<TYPE>::~IPSubstraction() {}

template <typename TYPE>
void IPSubstraction<TYPE>::substract() {
	assert(m_ref != nullptr);
	assert(m_flt != nullptr);

	m_substracted_img = SAFE_ALLOC_VOLUME(TYPE, m_ref->depth(), m_ref->height() * m_ref->width());

	for (int i = 0; i < m_ref->depth(); i++) {
		for (int j = 0; j < m_ref->height(); j++) {
			for (int k = 0; k < m_ref->width(); k++) {
				short cur_x = k;
				short cur_y = j;
				short cur_z = i;

				TYPE ref_val = m_ref->get(cur_x, cur_y, cur_z);
				TYPE flt_val = m_flt->get(cur_x, cur_y, cur_z);
				m_substracted_img[cur_z][cur_y * m_ref->width() + cur_x] = ref_val - flt_val;
			}
		}
	}
}

template <typename TYPE>
void IPSubstraction<TYPE>::save() {

	// Temporally write arr to file to test
	std::ofstream write_test_file1("substract.raw", std::ios::binary | std::ios::out);

	for (int i = 0; i < m_ref->depth(); i++) {
		for (int j = 0; j < m_ref->height(); j++) {
			for (int k = 0; k < m_ref->width(); k++) {
				if (write_test_file1.is_open()) {
					TYPE res = m_substracted_img[i][j * m_ref->width() + k];
					write_test_file1.write((char*)&res, sizeof(TYPE));
				}
			}
		}
	}
	write_test_file1.close();
}