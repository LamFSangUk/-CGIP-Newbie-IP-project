#pragma once
#include "../Common/image3d.h"

template <typename TYPE>
class IPThreshold {
public:
	IPThreshold();
	~IPThreshold();

	__forceinline void setMaxThresholdValue(int val) const noexcept { m_max_threshold_val = val; }
	__forceinline void setMinThresholdValue(int val) const noexcept { m_min_threshold_val = val; }
	__forceinline void setTarget(mc::image3d<TYPE> ref, mc::image3d<TYPE> dest) const noexcept {
		m_ref = ref;
		m_res = dest;
	}
	void thresholding();
private:
	int m_max_threshold_val;
	int m_min_threshold_val;
	const mc::image3d<TYPE> m_ref;
	mc::image3d<TYPE> m_res;
};