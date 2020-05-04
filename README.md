# SFND 2D Feature Tracking

<img src="images/keypoints.png" width="820" height="248" />

The idea of the camera course is to build a collision detection system - that's the overall goal for the Final Project. As a preparation for this, you will now build the feature tracking part and test various detector / descriptor combinations to see which ones perform best. This mid-term project consists of four parts:

* First, you will focus on loading images, setting up data structures and putting everything into a ring buffer to optimize memory load. 
* Then, you will integrate several keypoint detectors such as HARRIS, FAST, BRISK and SIFT and compare them with regard to number of keypoints and speed. 
* In the next part, you will then focus on descriptor extraction and matching using brute force and also the FLANN approach we discussed in the previous lesson. 
* In the last part, once the code framework is complete, you will test the various algorithms in different combinations and compare them with regard to some performance measures. 

See the classroom instruction and code comments for more details on each of these parts. Once you are finished with this project, the keypoint matching part will be set up and you can proceed to the next lesson, where the focus is on integrating Lidar points and on object detection using deep-learning. 

## Dependencies for Running Locally
* cmake >= 2.8
  * All OSes: [click here for installation instructions](https://cmake.org/install/)
* make >= 4.1 (Linux, Mac), 3.81 (Windows)
  * Linux: make is installed by default on most Linux distros
  * Mac: [install Xcode command line tools to get make](https://developer.apple.com/xcode/features/)
  * Windows: [Click here for installation instructions](http://gnuwin32.sourceforge.net/packages/make.htm)
* OpenCV >= 4.1
  * This must be compiled from source using the `-D OPENCV_ENABLE_NONFREE=ON` cmake flag for testing the SIFT and SURF detectors.
  * The OpenCV 4.1.0 source code can be found [here](https://github.com/opencv/opencv/tree/4.1.0)
* gcc/g++ >= 5.4
  * Linux: gcc / g++ is installed by default on most Linux distros
  * Mac: same deal as make - [install Xcode command line tools](https://developer.apple.com/xcode/features/)
  * Windows: recommend using [MinGW](http://www.mingw.org/)

## Basic Build Instructions

1. Clone this repo.
2. Make a build directory in the top level directory: `mkdir build && cd build`
3. Compile: `cmake .. && make`
4. Run it: `./2D_feature_tracking`.

## My Notes

#### [Different Feature Detectors and Descriptor](https://docs.opencv.org/master/db/d27/tutorial_py_table_of_contents_feature2d.html)

1. **Harris**: old method, don't use. It finds corners and uses it as features
2. **ShiTomasi** aka GoodFeaturesToTrack: old method, don't use also
3. **SIFT**: slow and patented, don't use. Uses intensity gradient description aka HOG (Histogram of Intensity Gradient)
4. **FAST, BRIEF, ORB, BRISK, FREAK, KAZE**: free and easy to use. Uses binary description
5. [Full list here](https://docs.opencv.org/3.4.9/d0/d13/classcv_1_1Feature2D.html)

#### [Descriptor Matcher](https://docs.opencv.org/3.4/db/d39/classcv_1_1DescriptorMatcher.html)

1. **BF Matcher**: brute force matcher, basically compare everything with O(n^2) complexity. If description is in HOG format (e.g. SIFT), must use cv::NORM_L2 distance, if binary (most of them are) use cv::NORM_HAMMING for the normType
2. **FLANN Matcher**: uses magic aka deep learning to match. Fast but slightly less accurate. Have bug in openCV, must convert descriptions to CV_32F first to prevent runtime error

##### Matching Filter

1. **[NN](https://docs.opencv.org/3.4/db/d39/classcv_1_1DescriptorMatcher.html#a0f046f47b68ec7074391e1e85c750cba)**: nearest neighbour, easy and straightforward, but can have more false positives
2. **[kNN](https://docs.opencv.org/3.4/db/d39/classcv_1_1DescriptorMatcher.html#a378f35c9b1a5dfa4022839a45cdf0e89)**: kth nearest neighbour usually 2, to compare the best vs 2nd best matches, if they are too near, means not a good match. Good threshold is 80% ratio. Slower but more accurate

