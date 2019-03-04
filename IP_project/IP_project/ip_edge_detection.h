#pragma once
#include<vector>
#include "../Common/image3d.h"
#include "../Common/memory.h"

template <typename TYPE>
class IPEdge {
public:

	IPEdge(mc::image3d<TYPE>* img);
	~IPEdge();

	void detect();
private:

	TYPE** m_img_origin_arr;
	mc::image3d<TYPE> * m_img;
	const std::vector<std::pair<short, short> > filter;
};