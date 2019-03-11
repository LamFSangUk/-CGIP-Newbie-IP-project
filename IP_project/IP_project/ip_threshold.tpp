#include "ip_threshold.h"

template <typename TYPE>
IPThreshold<TYPE>::IPThreshold() {

	m_ref = nullptr;
	m_res = nullptr;

	// Initialize thresholding value
	m_max_threshold_val = INT_MAX;
	m_min_threshold_val = INT_MIN;
}

template <typename TYPE>
IPThreshold<TYPE>::~IPThreshold() {

}

template <typename TYPE>
void IPThreshold<TYPE>::thresholding() {
	assert(m_ref != nullptr);
	assert(m_res != nullptr);

	assert(m_ref->depth() == m_res->depth());
	assert(m_ref->height() == m_res->height());
	assert(m_ref->width() == m_res->width());

	for (int i = 0; i < m_ref->depth(); i++) {
		for (int j = 0; j < m_ref->height(); j++) {
			for (int k = 0; k < m_ref->width(); k++) {
				TYPE val = m_ref->get(k, j, i);
				
				if (m_min_threshold_val <= val
					&& val <= m_max_threshold_val) {
					m_res->data()[i][j*m_ref->width() + k] = 1;
				}
			}
		}
	}
}