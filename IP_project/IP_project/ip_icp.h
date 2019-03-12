#pragma once

#include<vector>
#include<cassert>
#include "../Common/image3d.h"
#include "../Common/memory.h"
#include "global.h"

template <typename TYPE>
class IPICP {
public:
	IPICP();
	~IPICP();

	__forceinline void setRefImg(mc::image3d<TYPE>* ref) { m_ref = ref; }
	__forceinline void setFltImg(mc::image3d<TYPE>* flt) { m_flt = flt; }
	__forceinline void setRefObject(mc::image3d<TYPE>* ref_obj) { m_ref_obj = ref_obj; }
	__forceinline void setFltObject(mc::image3d<TYPE>* flt_obj) { m_flt_obj = flt_obj; }
	__forceinline void setRefDistanceMap(short** dt_map) { m_ref_distance_map = dt_map; }

	void calculateInitT();
	void iterate();
	void transformFltImg();
	void calculateSimilarity();


private:
	void calculateCOI(mc::image3d<TYPE>* img, point* coi);
	void transform(mc::image3d<TYPE>* img);

	mc::image3d<TYPE>* m_ref;
	mc::image3d<TYPE>* m_flt;
	mc::image3d<TYPE>* m_ref_obj;
	mc::image3d<TYPE>* m_flt_obj;
	short** m_ref_distance_map;

	point m_ref_coi;
	point m_flt_coi;

	Eigen::Matrix4d m_trans;

	unsigned int m_avg_dist;
};