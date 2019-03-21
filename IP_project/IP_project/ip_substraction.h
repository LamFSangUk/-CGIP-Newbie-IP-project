#pragma once

#include <algorithm>
#include "../Common/image3d.h"
#include "../Common/memory.h"
#include "global.h"

template <typename TYPE>
class IPSubstraction {
public:
	IPSubstraction(mc::image3d<TYPE>*ref,mc::image3d<TYPE>*flt);
	~IPSubstraction();

	void substract();
	void save();	// save substract img in file
private:
	mc::image3d<TYPE>* m_ref;
	mc::image3d<TYPE>* m_flt;

	TYPE** m_substracted_img;
};