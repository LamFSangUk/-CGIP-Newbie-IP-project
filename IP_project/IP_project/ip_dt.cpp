#include "ip_dt.h"

template <typename TYPE>
IPDT<TYPE>::IPDT(mc::image3d<TYPE>* ref, mc::image3d<TYPE>* flt) : first_kernel{
	// {x, y, z}
	{(short)-1,	(short)-1,	(short)-1},
	{(short)0,	(short)-1,	(short)-1},
	{(short)1,	(short)-1,	(short)-1},
	{(short)-1,	(short)0,	(short)-1},
	{(short)0,	(short)0,	(short)-1},
	{(short)1,	(short)0,	(short)-1},
	{(short)-1,	(short)1,	(short)-1},
	{(short)0,	(short)1,	(short)-1},
	{(short)1,	(short)1,	(short)-1},
	{(short)-1,	(short)-1,	(short)0},
	{(short)0,	(short)-1,	(short)0},
	{(short)1,	(short)-1,	(short)0},
	{(short)-1,	(short)0,	(short)0}
}, second_kernel{
	// {x, y, z}
	{(short)1,	(short)1,	(short)1},
	{(short)0,	(short)1,	(short)1},
	{(short)-1,	(short)1,	(short)1},
	{(short)1,	(short)0,	(short)1},
	{(short)0,	(short)0,	(short)1},
	{(short)-1,	(short)0,	(short)1},
	{(short)1,	(short)-1,	(short)1},
	{(short)0,	(short)-1,	(short)1},
	{(short)-1,	(short)-1,	(short)1},
	{(short)1,	(short)1,	(short)0},
	{(short)0,	(short)1,	(short)0},
	{(short)-1,	(short)1,	(short)0},
	{(short)1,	(short)0,	(short)0}
} {
	m_ref = ref;
	m_flt = flt;

	m_distance_map = SAFE_ALLOC_VOLUME(int, m_img->depth(), m_img->height()*m_img->width());

	// Initialize distance map
	for (int i = 0; i < m_ref->depth(); i++) {
		for (int j = 0; j < m_ref->height(); j++) {
			for (int k = 0; k < m_ref->width(); k++) {
				m_distance_map[i][j * m_ref->width() + k] = INT_MAX;
			}
		}
	}
}

template <typename TYPE>
IPDT<TYPE>::~IPDT() {

}

template <typename TYPE>
void IPDT<TYPE>::construct_distance_map(){
	const int obj_intensity = 1;

	// first pass
	for (int i = 0; i < m_ref->depth(); i++) {
		for (int j = 0; j < m_ref->height(); j++) {
			for (int k = 0; k < m_ref->width(); k++) {
				int cur_x = k;
				int cur_y = j;
				int cur_z = i;

				if (m_ref->get(cur_x, cur_y, cur_z) == obj_intensity) {
					m_distance_map[cur_z][cur_y * m_ref->width() + cur_x] = 0;
					continue;
				}

				int min_distance = m_distance_map[cur_z][cur_y * m_ref->width() + cur_x];

				for (auto relation : first_kernel) {
					int neighbor_x = cur_x + std::get<0>(relation);
					int neighbor_y = cur_y + std::get<1>(relation);
					int neighbor_z = cur_z + std::get<2>(relation);

					if (neighbor_x < 0
						|| neighbor_x >= m_ref->width()
						|| neighbor_y < 0
						|| neighbor_y >= m_ref->height()
						|| neighbor_z < 0
						|| neighbor_z >= m_ref->depth()) {
						continue;
					}

					int neighbor_distance = m_distance_map[neighbor_z][neighbor_y * m_width() + neighbor_x] + 1;
					if (min_distance < neighbor_distance) {
						min_distance = neighbor_distance;
					}
				}

				// update distance map
				m_distance_map[cur_z][cur_y * m_ref->width() + cur_x] = min_distance;
			}
		}
	}

	// second pass
	for (int i = m_ref->depth() - 1; i >= 0; i--) {
		for (int j = m_ref->height() - 1; j >= 0 ; j--) {
			for (int k = m_ref->width() - 1; k >= 0; k--) {
				int cur_x = k;
				int cur_y = j;
				int cur_z = i;

				if (m_ref->get(cur_x, cur_y, cur_z) == obj_intensity) {
					m_distance_map[cur_z][cur_y * m_ref->width() + cur_x] = 0;
					continue;
				}

				int min_distance = m_distance_map[cur_z][cur_y * m_ref->width() + cur_x];

				for (auto relation : second_kernel) {
					int neighbor_x = cur_x + std::get<0>(relation);
					int neighbor_y = cur_y + std::get<1>(relation);
					int neighbor_z = cur_z + std::get<2>(relation);

					if (neighbor_x < 0
						|| neighbor_x >= m_ref->width()
						|| neighbor_y < 0
						|| neighbor_y >= m_ref->height()
						|| neighbor_z < 0
						|| neighbor_z >= m_ref->depth()) {
						continue;
					}

					int neighbor_distance = m_distance_map[neighbor_z][neighbor_y * m_width() + neighbor_x] + 1;
					if (min_distance < neighbor_distance) {
						min_distance = neighbor_distance;
					}
				}

				// update distance map
				m_distance_map[cur_z][cur_y * m_ref->width() + cur_x] = min_distance;
			}
		}
	}
}