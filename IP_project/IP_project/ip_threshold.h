#pragma once
#include "../Common/image3d.h"

#include <cassert>

template <typename TYPE>
class IPThreshold {
public:
	IPThreshold();
	~IPThreshold();

	__forceinline void setMaxThresholdValue(int val) noexcept { m_max_threshold_val = val; }
	__forceinline void setMinThresholdValue(int val) noexcept { m_min_threshold_val = val; }
	__forceinline void setTarget(mc::image3d<TYPE>* ref, mc::image3d<TYPE>* dest) noexcept {
		m_ref = ref;
		m_res = dest;
	}

	void thresholding();
private:
	int m_max_threshold_val;
	int m_min_threshold_val;
	mc::image3d<TYPE>* m_ref;
	mc::image3d<TYPE>* m_res;
};

#include "ip_threshold.tpp"