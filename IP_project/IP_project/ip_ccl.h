#pragma once
#include<vector>
#include "../Common/image3d.h"

#define Coord std::pair<short,short>

typedef struct {
	int label;
	int parent;
}Point;

typedef struct {
	int label;
	int size;
}Component;

template <typename TYPE>
class IPCCL {
public:
	IPCCL(mc::image3d<TYPE>* img);
	~IPCCL();

	void analyze();
	void result();

private:
	mc::image3d<TYPE> * m_img;

	std::vector<Point> m_points;
	const std::vector<std::pair<short, short> > neighbor;
	std::vector<Component> m_components;
 
	void make_new_component(Coord c,int label_count);
};