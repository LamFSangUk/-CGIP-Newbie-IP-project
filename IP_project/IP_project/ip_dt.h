#pragma once

#include<vector>
#include "../Common/image3d.h"
#include "../Common/memory.h"

template <typename TYPE>
class IPDT {
public:
	IPDT(mc::image3d<TYPE>* ref,mc::image3d<TYPE>* flt);
	~IPDT();

	void construct_distance_map();
	void copy_dt_arr(TYPE** dst);
private:
	mc::image3d<TYPE>* m_ref;
	mc::image3d<TYPE>* m_flt;
	const std::vector<std::tuple<short, short, short> > first_kernel;
	const std::vector<std::tuple<short, short, short> > second_kernel;

	TYPE** m_distance_map; // chessboard distance
};

#include "ip_dt.tpp"