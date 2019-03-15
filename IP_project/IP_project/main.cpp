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

// User-define lib
#include "ip_threshold.h"
#include "ip_registration.h"
#include "ip_registration.cpp"
#include "ip_ccl.h"
#include "ip_edge_detection.h"
#include "ip_dt.h"
#include "ip_substraction.h"
#include "ip_substraction.cpp"

#define SET_START_TICK before = clock()
#define GET_TICK (double)(clock() - before) / CLOCKS_PER_SEC

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
	clock_t before;
	double result;

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

	std::cout << "****************************************" << std::endl;
	std::cout << "***** START THRESHOLDING ***************" << std::endl<<std::endl;
	SET_START_TICK;

	// Thresholding
	
	auto threshold = new IPThreshold<short>();
	threshold->setMaxThresholdValue(-400);
	threshold->setMinThresholdValue(-1024);

	// Image1
	threshold->setTarget(img1.get(), img1_thresholded);
	threshold->thresholding();

	// Image2
	threshold->setTarget(img2.get(),img2_thresholded);
	threshold->thresholding();

	delete threshold;

	result = GET_TICK;
	std::cout << "***** TIME = "<< result << "s "<< "************" << std::endl;
	std::cout << "***** FINISH THRESHOLDING **************" << std::endl<<std::endl;

	std::cout << "****************************************" << std::endl;
	std::cout << "***** START CCA ************************" << std::endl<<std::endl;
	SET_START_TICK;

	// CCA
	auto cca1 = new IPCCL<short>(img1_thresholded);
	cca1->analyze();
	cca1->bg_pruning(337);
	//cca1->result();

	auto cca2 = new IPCCL<short>(img2_thresholded);
	cca2->analyze();
	cca2->bg_pruning(476);
	//cca2->result();

	result = GET_TICK;
	std::cout << "***** TIME = " << result << "s " << "************" << std::endl;
	std::cout << "***** FINISH CCA ***********************" << std::endl << std::endl;


	// TODO #1-1 : Initial transformation parameter calculation.
	auto registration = new IPRegistration<short>();
	registration->setRefImg(img1.get());
	registration->setFltImg(img2.get());
	registration->setRefObject(img1_thresholded);
	registration->setFltObject(img2_thresholded);

	registration->calculateInit();

	// TODO #2 : edge extraction for both images.

	std::cout << "****************************************" << std::endl;
	std::cout << "***** START Edge Detection *************" << std::endl << std::endl;
	SET_START_TICK;

	auto edge1 = new IPEdge<short>(img1_thresholded);
	edge1->detect();

	auto edge2 = new IPEdge<short>(img2_thresholded);
	edge2->detect();

	delete edge1;
	delete edge2;

	result = GET_TICK;
	std::cout << "***** TIME = " << result << "s " << "************" << std::endl;
	std::cout << "***** FINISH Edge Detection ************" << std::endl << std::endl;

	// TODO #3 : Distance transformation.
	std::cout << "****************************************" << std::endl;
	std::cout << "***** START Distance Map Calculation ***" << std::endl << std::endl;
	SET_START_TICK;

	auto dt = new IPDT<short>(img1_thresholded,img2_thresholded);
	short** distance_map = SAFE_ALLOC_VOLUME(short, img1_depth, img1_height*img1_width);
	dt->construct_distance_map();
	dt->copyDistanceMap(distance_map);

	result = GET_TICK;
	std::cout << "***** TIME = " << result << "s " << "************" << std::endl;
	std::cout << "***** FINISH Distance Map Calculation **" << std::endl << std::endl;

	// TODO #4 : Perform iterative REGISTRATION.
	registration->makeFltObjectPointList();

	registration->setRefDistanceMap(distance_map);
	//registration->calculateSimilarity();
	registration->iterate();
	//registration->calculateSimilarity();

	// TODO #5 : Transform moving (floating) image with estimated transformation parameter & generate subtraction image.
	auto substraction = new IPSubstraction<short>(img1.get(),img2.get());
	
	//registration->transformFltImg();
	//substraction->substract();
	//substraction->save();

	// TODO #6 : store subtraction image (visual purpose).


	/* Test output
	std::ofstream write_test_file1("img1_edge.raw", std::ios::binary | std::ios::out);

	write_test_file1.write((char*)&img1_thresholded_arr, img1_depth*img1_height*img1_width * sizeof(short));

	for (int i = 0; i < img1_depth; i++) {
		for (int j = 0; j < img1_height; j++) {
			for (int k = 0; k < img1_width; k++) {
				if (write_test_file1.is_open()) {
					short res = img1_thresholded_arr[i][j * img1_width + k];
					write_test_file1.write((char*)&res, sizeof(short));
				}
			}
		}
	}
	write_test_file1.close();

	std::ofstream write_test_file2("img2_edge.raw",std::ios::binary | std::ios::out);

	for (int i = 0; i < img2_depth; i++) {
		for (int j = 0; j < img2_height; j++) {
			for (int k = 0; k < img2_width; k++) {
				if (write_test_file2.is_open()) {
					short res = img2_thresholded_arr[i][j * img2_width + k];
					write_test_file2.write((char*)&res, sizeof(short));

				}
			}
		}
	}
	write_test_file2.close();*/

	// TODO :
	// perform Intensity-based registration (using similarity measure metric with original intensities).
	// Do #[4, 5, 6] and compare with surface-based method.

	return EXIT_SUCCESS;
}