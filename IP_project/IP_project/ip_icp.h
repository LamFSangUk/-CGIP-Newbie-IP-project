#pragma once

#include<vector>
#include<cassert>
#include "../Common/image3d.h"
#include "global.h"

template <typename TYPE>
class IPICP {
public:
	IPICP();
	~IPICP();

	__forceinline void setRefImg(mc::image3d<TYPE>* ref) { m_ref = ref; }
	__forceinline void setFltImg(mc::image3d<TYPE>* flt) { m_flt = flt; }

	void calculateInitT();
private:
	void calculateCOI(mc::image3d<TYPE>* img, point* coi);

	mc::image3d<TYPE>* m_ref;
	mc::image3d<TYPE>* m_flt;
	point m_ref_coi;
	point m_flt_coi;

	Eigen::Matrix4d m_trans;
};