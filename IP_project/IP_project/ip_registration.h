#pragma once

#include<vector>
#include<cassert>
#include<cmath>
#include "../Common/image3d.h"
#include "../Common/memory.h"
#include "global.h"

# define M_PI           3.14159265358979323846  /* pi */

template <typename TYPE>
class IPRegistration {
public:
	IPRegistration();
	~IPRegistration();

	__forceinline void setRefImg(mc::image3d<TYPE>* ref) { m_ref = ref; }
	__forceinline void setFltImg(mc::image3d<TYPE>* flt) { m_flt = flt; }
	__forceinline void setRefObject(mc::image3d<TYPE>* ref_obj) { m_ref_obj = ref_obj; }
	__forceinline void setFltObject(mc::image3d<TYPE>* flt_obj) { m_flt_obj = flt_obj; }
	__forceinline void setRefDistanceMap(short** dt_map) { m_ref_distance_map = dt_map; }

	void calculateInit();
	void iterate();
	void transformFltImg();
	void makeFltObjectPointList();
	void transform(mc::image3d<TYPE>* img);

private:
	void calculateCOI(mc::image3d<TYPE>* img, Point* coi);
	double calculateSimilarity(std::vector<Point>& moved);
	Eigen::Matrix4d makeTransformMatrix(int t_param,double degree);
	void transform(std::vector<Point>* target, Eigen::Matrix4d mat);

	mc::image3d<TYPE>* m_ref;
	mc::image3d<TYPE>* m_flt;
	mc::image3d<TYPE>* m_ref_obj;
	mc::image3d<TYPE>* m_flt_obj;

	std::vector<Point> m_flt_obj_points;	// center is coi, coi's coordinate is (0,0,0)

	short** m_ref_distance_map;

	Point m_ref_coi;
	Point m_flt_coi;

	Eigen::Matrix4d m_trans;

	unsigned int m_avg_dist;
};