#include "ip_ccl.h"

#include<iostream>

template <typename TYPE>
IPCCL<TYPE>::IPCCL(mc::image3d<TYPE>* img) : neighbor {// 4-connectivity
	{(short)0,	(short)-1},
	{(short)-1,	(short)0},
	{(short)1,	(short)0},
	{(short)0,	(short)1}
} {
	m_img = img;
	
	Point init_p;
	init_p.label = init_p.parent = 0;

	for (int i = 0; i < m_img->height(); i++) {
		for (int j = 0; j < m_img->width(); j++) {
			m_points.push_back(init_p);

			//std::cout << m_img->get(j, i, 26) << " ";
		}
		//std::cout << std::endl;
	}
}

template <typename TYPE>
IPCCL<TYPE>::~IPCCL() {

}

template <typename TYPE>
void IPCCL<TYPE>::analyze() {

	const int bg_intensity = 0;
	for (int i = 25; i < m_img->depth(); i++) { // for test

		int current_label_count = 0;


		// CCA per image
		for (int j = 0; j < m_img->height(); j++) {
			for (int k = 0; k < m_img->width(); k++) {

				short cur_x = k;
				short cur_y = j;

				//If background, skip the process
				if (m_img->get(k, j, i) == bg_intensity) continue;

				// for every neighbor pixels
				Point min_p, single_p;
				min_p.label = INT_MAX;

				// check neighbors' label and find min
				int label_value = 0;
				int neighbor_count = neighbor.size(); // The number of neighbor pixels except bg pixels.
				int single_label_count = 0;

				for (auto relation : neighbor) {
					short neighbor_x = cur_x + relation.first;
					short neighbor_y = cur_y + relation.second;

					// need to check the range of width and height
					if (neighbor_x < 0
						|| neighbor_x >= m_img->width()
						|| neighbor_y < 0
						|| neighbor_y >= m_img->height()) {
						neighbor_count--;
						continue;
					}

					if (m_img->get(neighbor_x, neighbor_y, i) == bg_intensity) neighbor_count--; // ignore bg pixels

					Point neighbor_p = m_points[neighbor_y * m_img->width() + neighbor_x];
					if (neighbor_p.label == 0) { // not visited neighbor
						neighbor_count--;
						continue;
					}
					else if (neighbor_p.label < min_p.label) { // get minimun label point
						min_p = neighbor_p;
					}

					// check the neighbors have single values
					if (single_label_count == 0) {
						single_p = neighbor_p;
						label_value = neighbor_p.label;
						single_label_count++;
					}
					else if(label_value == neighbor_p.label){
						single_label_count++;
					}
				}

				Coord cur_coord = std::make_pair(k, j);
				// if no neightbor pixel assigned
				if (single_label_count == 0) {
					// make a new set
					make_new_component(cur_coord, ++current_label_count);
				}
				else if (neighbor_count == single_label_count) {// In case neighbors have single label
					Point& cur_p = m_points[cur_coord.second * m_img->width() + cur_coord.first];
					cur_p.label = single_p.label;
					cur_p.parent = single_p.parent;

					Component& comp = m_components[cur_p.label-1];
					comp.size++;
				}
				else {// neighbors have different labels
					Point& cur_p = m_points[cur_coord.second * m_img->width() + cur_coord.first];
					cur_p.label = min_p.label;
					cur_p.parent = min_p.parent;

					Component& comp = m_components[cur_p.label-1];
					comp.size++;

					for (auto relation : neighbor) {
						short neighbor_x = cur_x + relation.first;
						short neighbor_y = cur_y + relation.second;

						// need to check the range of width and height
						if (neighbor_x < 0
							|| neighbor_x >= m_img->width()
							|| neighbor_y < 0
							|| neighbor_y >= m_img->height()) {
							neighbor_count--;
							continue;
						}

						Point& neighbor_p = m_points[neighbor_y * m_img->width() + neighbor_x];
						neighbor_p.parent = min_p.parent;
					}
				}
				
			}
		}
		break;//temporal break
	}
}

template <typename TYPE>
void IPCCL<TYPE>::result() {
	std::cout << m_components.size();
}


template <typename TYPE>
void IPCCL<TYPE>::make_new_component(Coord c, int label_count) {
	Point& p = m_points[c.second * m_img->width() + c.first];
	p.label = p.parent = label_count;

	Component comp;
	comp.label = label_count;
	comp.size = 1;
	m_components.push_back(comp);
}