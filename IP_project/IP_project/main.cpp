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
	std::unique_ptr<mc::image3d<short>> img1 = load_image<short>("volume1_512x512x56.raw", 512, 512, 56); // 2 bytes
	std::unique_ptr<mc::image3d<short>> img2 = load_image<short>("volume2_512x512x58.raw", 512, 512, 58);

	/*
	*	EXAMPLES.
	* access data array :
	*	short** image_array = img1->data();
	* see more details at "../Common/image3d.h"
	*/

	// Read raw data of images
	short** img1_arr = img1->data();
	short** img2_arr = img2->data();

	// The basic info of images
	int img1_width = img1->width();
	int img1_height = img1->height();
	int img1_depth = img1->depth();
	int img2_width = img2->width();
	int img2_height = img2->height();
	int img2_depth = img2->depth();

	// The thresholded images
	auto img1_thresholded = new mc::image3d<short>(img1_width, img1_height, img1_depth);
	auto img2_thresholded = new mc::image3d<short>(img2_width, img2_height, img2_depth);

	// Initialize
	img1_thresholded->zeroImage();
	img2_thresholded->zeroImage();

	short** img1_thresholded_arr = img1_thresholded->data();
	short** img2_thresholded_arr = img2_thresholded->data();


	// TODO #0 (optional) : Semi-isotropic image generation. (insert 4 slices for each)

	// TODO #1 : segmentation of lung region. (use thresholding, CCA)

	// Thresholding
	short min_threshold = -1024;
	short max_threshold = -400;

	// Image1
	for (int i = 0; i < img1_depth; i++) {
		for (int j = 0; j < img1_height; j++) {
			for (int k = 0; k < img1_width; k++) {
				short val = img1->get(k, j, i);
				if (min_threshold <= val
					&& val <= max_threshold) {
					img1_thresholded_arr[i][j + k * img1_width] = 1;
				}
			}
		}
	}

	// Image2
	for (int i = 0; i < img2_depth; i++) {
		for (int j = 0; j < img2_height; j++) {
			for (int k = 0; k < img2_width; k++) {
				short val = img2->get(k, j, i);
				if (min_threshold <= val
					&& val <= max_threshold) {
					img2_thresholded_arr[i][j + k * img2_width] = 1;
				}
			}
		}
	}
	

	// Test output
	std::ofstream write_test_file1("test1.raw");

	for (int i = 0; i < img1_depth; i++) {
		for (int j = 0; j < img1_height; j++) {
			for (int k = 0; k < img1_width; k++) {
				if (write_test_file1.is_open()) {
					short res = img1_thresholded_arr[i][j + k * img1_width];
					write_test_file1.write((char*)&res, sizeof(short));
				}
			}
		}
	}
	write_test_file1.close();

	std::ofstream write_test_file2("test2.raw");

	for (int i = 0; i < img2_depth; i++) {
		for (int j = 0; j < img2_height; j++) {
			for (int k = 0; k < img2_width; k++) {
				if (write_test_file2.is_open()) {
					short res = img2_thresholded_arr[i][j + k * img2_width];
					write_test_file2.write((char*)&res, sizeof(short));
				}
			}
		}
	}
	write_test_file2.close();

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