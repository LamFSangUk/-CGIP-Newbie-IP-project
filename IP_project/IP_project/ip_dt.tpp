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

	m_distance_map = SAFE_ALLOC_VOLUME(TYPE, m_ref->depth(), m_ref->height()*m_ref->width());

	// Initialize distance map
	for (int i = 0; i < m_ref->depth(); i++) {
		for (int j = 0; j < m_ref->height(); j++) {
			for (int k = 0; k < m_ref->width(); k++) {
				m_distance_map[i][j * m_ref->width() + k] = SHRT_MAX;
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

				short min_distance = m_distance_map[cur_z][cur_y * m_ref->width() + cur_x];

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

					short neighbor_distance = m_distance_map[neighbor_z][neighbor_y * m_ref->width() + neighbor_x];
					if (neighbor_distance == SHRT_MAX) continue; // skip for unvisited point
					if (min_distance > neighbor_distance + 1) {
						min_distance = neighbor_distance + 1;
					}
				}

				// update distance map
				m_distance_map[cur_z][cur_y * m_ref->width() + cur_x] = min_distance;
			}
		}
	}

	std::cout << "first pass complete" << std::endl;

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

				short min_distance = m_distance_map[cur_z][cur_y * m_ref->width() + cur_x];

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

					short neighbor_distance = m_distance_map[neighbor_z][neighbor_y * m_ref->width() + neighbor_x];
					if (neighbor_distance == SHRT_MAX) continue; // skip for unvisited point
					if (min_distance > neighbor_distance + 1) {
						min_distance = neighbor_distance + 1;
					}
				}

				// update distance map
				m_distance_map[cur_z][cur_y * m_ref->width() + cur_x] = min_distance;
			}
		}
	}
}

template<typename TYPE>
void IPDT<TYPE>::copy_dt_arr(TYPE** dst) {

	// Temporally write arr to file to test
	std::ofstream write_test_file1("dt.raw", std::ios::binary | std::ios::out);

	for (int i = 0; i < m_ref->depth(); i++) {
		for (int j = 0; j < m_ref->height(); j++) {
			for (int k = 0; k < m_ref->width(); k++) {
				if (write_test_file1.is_open()) {
					short res = m_distance_map[i][j * m_ref->width() + k];
					write_test_file1.write((char*)&res, sizeof(short));
				}
			}
		}
	}
	write_test_file1.close();
}