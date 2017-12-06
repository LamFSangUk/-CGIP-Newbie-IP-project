#include "../Common/memory.h"
#include "../Common/image3d.h"
#include "../Core/raw_io.h"
#include "../Core/raw_io_exception.h"
#include <memory>
#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>
#include <limits>
#include <fstream>
#include <ctime>
#include <omp.h>

template <class T>
std::unique_ptr<mc::image3d<T>> load_image(const std::string& path, const unsigned int w, const unsigned int h, const unsigned int d)
{
	using namespace mc;

	image3d<T>* pImg = nullptr;

	try {
		raw_io<T> io(path.c_str());
		io.setEndianType(raw_io<T>::EENDIAN_TYPE::BIG);
		pImg = io.read(w, h, d);
	}
	catch (raw_io_exception& e) {
		std::cerr << e.what() << std::endl;
	}

	return std::make_unique<image3d<T>>(*pImg);
}

int main()
{
	// read moving & target images for registration.
	std::unique_ptr<mc::image3d<short>> img1 = load_image<short>("volume1_512x512x56.raw", 512, 512, 56);
	std::unique_ptr<mc::image3d<short>> img2 = load_image<short>("volume2_512x512x58.raw", 512, 512, 58);

	/*
	*	EXAMPLES.
	* access data array :
	*	short** image_array = img1->data();
	* see more details at "../Common/image3d.h"
	*/

	// TODO #0 (optional) : Semi-isotropic image generation. (insert 4 slices for each)

	// TODO #1 : segmentation of lung region. (use thresholding, CCA)

	// TODO #1-1 : Initial transformation parameter calculation.

	// TODO #2 : edge extraction for both images.

	// TODO #3 : Distance transformation.

	// TODO #4 : Perform iterative REGISTRATION.

	// TODO #5 : Transform moving (floating) image with estimated transformation parameter & generate subtraction image.

	// TODO #6 : store subtraction image (visual purpose).

	// TODO :
	// perform Intensity-based registration (using similarity measure metric with original intensities).
	// Do #[4, 5, 6] and compare with surface-based method.

	return EXIT_SUCCESS;
}