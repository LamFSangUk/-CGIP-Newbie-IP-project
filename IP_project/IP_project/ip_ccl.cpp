#include "ip_ccl.h"

#include<iostream>

template <typename TYPE>
IPCCL<TYPE>::IPCCL(mc::image3d<TYPE>* img) : neighbor {// 4-connectivity
	// {x, y, z}
	{(short)0,	(short)0,	(short)-1},
	{(short)0,	(short)-1,	(short)0},
	{(short)-1,	(short)0,	(short)0}
} {
	m_img = img;

	m_labels = SAFE_ALLOC_VOLUME(int, m_img->depth(), m_img->height()*m_img->width());
}

template <typename TYPE>
IPCCL<TYPE>::~IPCCL() {

}

template <typename TYPE>
void IPCCL<TYPE>::analyze() {

	const int bg_intensity = 0;

	// CCL for 3d

	// Initialize for an image
	int current_label_count = 0;

	// Pass 1
	for (int i = 0; i < m_img->depth(); i++) {
		for (int j = 0; j < m_img->height(); j++) {
			for (int k = 0; k < m_img->width(); k++) {

				short cur_x = k;
				short cur_y = j;
				short cur_z = i;

				//std::cout << cur_x << " " << cur_y << " " << cur_z << std::endl;

				//If background, skip the process
				if (m_img->get(cur_x, cur_y, cur_z) == bg_intensity) continue;

				int min_label = INT_MAX;
				int neighbor_count = neighbor.size(); // The number of neighbor pixels except bg pixels.
				int single_label = 0;
				int single_label_count = 0;

				for (auto relation : neighbor) {

					short neighbor_x = cur_x + std::get<0>(relation);
					short neighbor_y = cur_y + std::get<1>(relation);
					short neighbor_z = cur_z + std::get<2>(relation);

					// need to check the range of width and height
					if (neighbor_x < 0
						|| neighbor_y < 0
						|| neighbor_z < 0) {
						neighbor_count--;
						continue;
					}

					if (m_img->get(neighbor_x, neighbor_y, neighbor_z) == bg_intensity) {
						// ignore bg pixels
						neighbor_count--;
						continue;
					}

					int neighbor_label = m_labels[neighbor_z][neighbor_y * m_img->width() + neighbor_x];
					if (neighbor_label < min_label) { // get minimun label
						min_label = neighbor_label;
					}

					// check the neighbors have single values
					if (single_label_count == 0) {
						single_label = neighbor_label;
						single_label_count++;
					}
					else if (neighbor_label == single_label) {
						single_label_count++;
					}
				}

				// if no neighbor pixel assigned
				if (single_label_count == 0) {
					// make a new set
					make_new_component(cur_x,cur_y,cur_z, ++current_label_count);
				}
				else if (neighbor_count == single_label_count) {// In case neighbors have single label
					int& cur_label = m_labels[cur_z][cur_y * m_img->width() + cur_x];
					cur_label = single_label;
					add_new_element(cur_label);
				}
				else {// neighbors have different labels
					int& cur_label = m_labels[cur_z][cur_y * m_img->width() + cur_x];
					cur_label = min_label;
					add_new_element(cur_label);

					for (auto relation : neighbor) {

						short neighbor_x = cur_x + std::get<0>(relation);
						short neighbor_y = cur_y + std::get<1>(relation);
						short neighbor_z = cur_z + std::get<2>(relation);


						// need to check the range of width and height
						if (neighbor_x < 0
							|| neighbor_y < 0
							|| neighbor_z < 0) {
							continue;
						}

						if (m_img->get(neighbor_x, neighbor_y, neighbor_z) == bg_intensity) {
							// ignore bg pixels
							continue;
						}

						int neighbor_label = m_labels[neighbor_z][neighbor_y * m_img->width() + neighbor_x];
						merge(neighbor_label, cur_label);
					}
				}

			}
		}
	}
	std::cout << "Pass 1 complete, comp size :" << m_components.size() << std::endl;

	// Pass 2
	for(int i = 0; i < m_img->depth(); i++){
		for (int j = 0; j < m_img->height(); j++) {
			for (int k = 0; k < m_img->width(); k++) {

				short cur_x = k;
				short cur_y = j;
				short cur_z = i;

				if (m_img->get(cur_x, cur_y, cur_z) == bg_intensity) continue; // ignore bg pixels

				int& cur_label = m_labels[cur_z][cur_y * m_img->width() + cur_x];

				Component root = find(m_components[cur_label - 1]);
				cur_label = root.label;
			}
		}
	}
}

template <typename TYPE>
void IPCCL<TYPE>::bg_pruning() {
	// Sorted by decending order for size
	std::sort(m_components.begin(), m_components.end(),std::greater<Component>());

	const int bg_intensity = 0;
	const int interest_label = m_components[20].label;

	TYPE** img_arr = m_img->data();

	for (int i = 0; i < m_img->depth(); i++) {
		for (int j = 0; j < m_img->height(); j++) {
			for (int k = 0; k < m_img->width(); k++) {

				short cur_x = k;
				short cur_y = j;
				short cur_z = i;

				int& cur_label = m_labels[cur_z][cur_y * m_img->width() + cur_x];

				//If background, skip the process
				if (m_img->get(cur_x, cur_y, cur_z) == bg_intensity) continue;
				else if (cur_label == interest_label) continue;
				else {
					img_arr[cur_z][cur_y * m_img->width() + cur_x] = (TYPE)bg_intensity;
				}
			}
		}
	}

}

template <typename TYPE>
void IPCCL<TYPE>::result() {
	for (int i = 0; i < m_components.size(); i++) {
		std::cout << "comp_label : " << m_components[i].label << " size : " << m_components[i].size << std::endl;
	}
}

template <typename TYPE>
void IPCCL<TYPE>::add_new_element(int label) {
	//Component& root = find(m_components[label - 1]);
	Component& comp = m_components[label - 1];
	comp.size++;
}

template <typename TYPE>
void IPCCL<TYPE>::merge(int label_x,int label_y) {
	Component& root_x = find(m_components[label_x-1]);
	Component& root_y = find(m_components[label_y-1]);

	if (root_x.label == root_y.label) return;

	if (root_x.size < root_y.size) {
		root_x.parent = root_y.label;
		root_y.size += root_x.size;
	}
	else {
		root_y.parent = root_x.label;
		root_x.size += root_y.size;
	}

}

template <typename TYPE>
Component& IPCCL<TYPE>::find(Component& c) {
	if (c.label == c.parent) {
		return c;
	}
	else {
		m_components[c.parent - 1] = find(m_components[c.parent - 1]);
		return m_components[c.parent - 1];
	}
}


template <typename TYPE>
void IPCCL<TYPE>::make_new_component(short x, short y, short z, int label){
	m_labels[z][y * m_img->width() + x] = label;

	Component comp;
	comp.label = comp.parent = label;
	comp.size = 1;
	m_components.push_back(comp);
}