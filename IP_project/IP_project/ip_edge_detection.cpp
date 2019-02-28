#include"ip_edge_detection.h"

template <typename TYPE>
IPEdge<TYPE>::IPEdge(mc::image3d<TYPE>* img) : filter{
	{(short)0,	(short)-1},
	{(short)-1,	(short)0},
	{(short)0,	(short)1},
	{(short)1,	(short)0}
} {
	m_img = img;

	m_img_origin_arr = SAFE_ALLOC_VOLUME(TYPE, m_img->depth(), m_img->width() * m_img->height());
	MEMCPY_VOLUME(TYPE, m_img_origin_arr, m_img->data(), m_img->depth(), m_img->width()*m_img->height());
}

template <typename TYPE>
IPEdge<TYPE>::~IPEdge() {

}

template <typename TYPE>
void IPEdge<TYPE>::detect() {
	const int bg_intensity = 0;

	short** img_arr = m_img->data();

	for (int i = 0; i < m_img->depth(); i++) {
		for (int j = 0; j < m_img->height(); j++) {
			for (int k = 0; k < m_img->width(); k++) {
				int cur_x = k;
				int cur_y = j;

				int neighbor_count = filter.size();
				int intensity_sum = 0;

				if (m_img_origin_arr[i][cur_y * m_img->width() + cur_x] == bg_intensity) continue;

				for (auto relation : filter) {
					short neighbor_x = cur_x + relation.first;
					short neighbor_y = cur_y + relation.second;

					if (neighbor_x < 0
						|| neighbor_x >= m_img->width()
						|| neighbor_y < 0
						|| neighbor_y >= m_img->height()) {
						continue;
					}
					if(m_img_origin_arr[i][neighbor_y * m_img->width() + neighbor_x] == 1) intensity_sum++;
				}
				//printf("%d %d\n", intensity_sum, neighbor_count);
				if (intensity_sum == neighbor_count) img_arr[i][cur_y * m_img->width() + cur_x] = bg_intensity;
			}
		}
	}
}