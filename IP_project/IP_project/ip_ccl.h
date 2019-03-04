#pragma once
#include<vector>
#include<tuple>
#include<algorithm>
#include "../Common/image3d.h"
#include "../Common/memory.h"

struct Component {
	int label;
	int parent;
	int size;

	bool operator > (const Component& comp) const {
		return size > comp.size;
	}
};

template <typename TYPE>
class IPCCL {
public:
	IPCCL(mc::image3d<TYPE>* img);
	~IPCCL();

	void analyze();
	void result();
	void bg_pruning(int obj_label);

private:
	mc::image3d<TYPE> * m_img;

	int** m_labels;
	const std::vector<std::tuple<short, short, short> > neighbor;
	std::vector<Component> m_components;
 
	void add_new_element(int label);
	void merge(int label_x, int label_y);
	Component& find(Component& c);
	void make_new_component(short x, short y, short z, int label);
};