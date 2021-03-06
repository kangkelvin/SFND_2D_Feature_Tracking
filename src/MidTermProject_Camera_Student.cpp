/* INCLUDES FOR THIS PROJECT */
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <sstream>
#include <vector>

#include "dataStructures.h"
#include "matching2D.hpp"

using namespace std;

/* MAIN PROGRAM */
int main(int argc, const char *argv[]) {
  /* INIT VARIABLES AND DATA STRUCTURES */

  vector<string> detectorTypes = {"SHITOMASI", "HARRIS", "FAST", "BRISK",
                                  "ORB",       "AKAZE",  "SIFT"};
  vector<string> descriptorTypes = {"BRISK", "SIFT", "BRIEF", "FREAK", "ORB"};

  // Permute over different detector + descriptor, except for AKAZE AKAZE combo
  for (auto detectorType : detectorTypes) {
    for (auto descriptorType : descriptorTypes) {
      // data location
      string dataPath = "../";

      // camera
      string imgBasePath = dataPath + "images/";
      // left camera, color
      string imgPrefix = "KITTI/2011_09_26/image_00/data/000000";
      string imgFileType = ".png";
      int imgStartIndex = 0;  // first file index to load (assumes Lidar and
                              // camera names have identical naming convention)
      int imgEndIndex = 9;    // last file index to load
      // no. of digits which make up the file index (e.g. img-0001.png)
      int imgFillWidth = 4;

      // misc
      int dataBufferSize = 2;  // no. of images which are held in memory (ring
                               // buffer) at the same time
      vector<DataFrame> dataBuffer;  // list of data frames which are held in
                                     // memory at the same time
      bool bVis = true;              // visualize results

      /* MAIN LOOP OVER ALL IMAGES */

      for (size_t imgIndex = 0; imgIndex <= imgEndIndex - imgStartIndex;
           imgIndex++) {
        /* LOAD IMAGE INTO BUFFER */

        // assemble filenames for current index
        ostringstream imgNumber;
        imgNumber << setfill('0') << setw(imgFillWidth)
                  << imgStartIndex + imgIndex;
        string imgFullFilename =
            imgBasePath + imgPrefix + imgNumber.str() + imgFileType;

        // load image from file and convert to grayscale
        cv::Mat img, imgGray;
        img = cv::imread(imgFullFilename);
        cv::cvtColor(img, imgGray, cv::COLOR_BGR2GRAY);

        //// STUDENT ASSIGNMENT
        //// TASK MP.1 -> replace the following code with ring buffer of size
        /// dataBufferSize
        while (dataBuffer.size() >= dataBufferSize) {
          dataBuffer.erase(dataBuffer.begin());
        }

        // push image into data frame buffer
        DataFrame frame;
        frame.cameraImg = imgGray;
        dataBuffer.push_back(frame);

        //// EOF STUDENT ASSIGNMENT
        cout << "#1 : LOAD IMAGE INTO BUFFER done" << endl;

        /* DETECT IMAGE KEYPOINTS */

        // extract 2D keypoints from current image
        // create empty feature list for current image
        vector<cv::KeyPoint> keypoints;
        vector<cv::KeyPoint> raw_keypoints;

        //// STUDENT ASSIGNMENT
        //// TASK MP.2 -> add the following keypoint detectors in file
        /// matching2D.cpp and enable string-based selection based on
        /// detectorType /
        ///-> HARRIS, FAST, BRISK, ORB, AKAZE, SIFT

        bVis = false;

        double detector_t = (double)cv::getTickCount();

        if (detectorType.compare("SHITOMASI") == 0) {
          detKeypointsShiTomasi(raw_keypoints, imgGray, bVis);
        } else if (detectorType.compare("HARRIS") == 0) {
          detKeypointsHarris(raw_keypoints, imgGray, bVis);
        } else {
          detKeypointsModern(raw_keypoints, imgGray, detectorType, bVis);
        }

        detector_t =
            ((double)cv::getTickCount() - detector_t) / cv::getTickFrequency();
        detector_t = 1000 * detector_t / 1.0;
        //// EOF STUDENT ASSIGNMENT

        //// STUDENT ASSIGNMENT
        //// TASK MP.3 -> only keep keypoints on the preceding vehicle

        // only keep keypoints on the preceding vehicle
        bool bFocusOnVehicle = true;
        cv::Rect vehicleRect(535, 180, 180, 150);
        if (bFocusOnVehicle) {
          std::for_each(raw_keypoints.begin(), raw_keypoints.end(),
                        [&vehicleRect, &keypoints](cv::KeyPoint keypoint) {
                          // insert only keypoints inside box
                          if (vehicleRect.contains(keypoint.pt)) {
                            keypoints.push_back(keypoint);
                          }
                        });
        }
        raw_keypoints.clear();  // free up memory

        bool writeDetectorResult = false;
        if (writeDetectorResult) {
          string fullDir = "../data/output.txt";
          std::fstream out_file;
          out_file.open(fullDir, ios::app);
          if (out_file.is_open()) {
            out_file << detectorType << ", " << imgIndex << ", "
                     << keypoints.size() << ", " << detector_t << std::endl;
          }
          out_file.close();
        }

        //// EOF STUDENT ASSIGNMENT

        // optional : limit number of keypoints (helpful for debugging and
        // learning)
        bool bLimitKpts = false;
        if (bLimitKpts) {
          int maxKeypoints = 50;

          if (detectorType.compare("SHITOMASI") ==
              0) {  // there is no response info, so keep the first 50 as they
                    // are sorted in descending quality order
            keypoints.erase(keypoints.begin() + maxKeypoints, keypoints.end());
          }
          cv::KeyPointsFilter::retainBest(keypoints, maxKeypoints);
          cout << " NOTE: Keypoints have been limited!" << endl;
        }

        // push keypoints and descriptor for current frame to end of data buffer
        (dataBuffer.end() - 1)->keypoints = keypoints;
        cout << "#2 : DETECT KEYPOINTS done with " << detectorType << endl;

        /* EXTRACT KEYPOINT DESCRIPTORS */

        //// STUDENT ASSIGNMENT
        //// TASK MP.4 -> add the following descriptors in file matching2D.cpp
        /// and
        /// enable string-based selection based on descriptorType / -> BRIEF,
        /// ORB, FREAK, AKAZE, SIFT

        cv::Mat descriptors;

        double descriptor_t = (double)cv::getTickCount();

        descKeypoints((dataBuffer.end() - 1)->keypoints,
                      (dataBuffer.end() - 1)->cameraImg, descriptors,
                      descriptorType);

        descriptor_t = ((double)cv::getTickCount() - descriptor_t) /
                       cv::getTickFrequency();
        descriptor_t = 1000 * descriptor_t / 1.0;

        //// EOF STUDENT ASSIGNMENT

        // push descriptors for current frame to end of data buffer
        (dataBuffer.end() - 1)->descriptors = descriptors;

        cout << "#3 : EXTRACT DESCRIPTORS done with " << descriptorType << endl;

        if (dataBuffer.size() >
            1)  // wait until at least two images have been processed
        {
          /* MATCH KEYPOINT DESCRIPTORS */

          vector<cv::DMatch> matches;
          string matcherType = "MAT_BF";  // MAT_BF, MAT_FLANN
          string descriptorDataType =
              descriptorType.compare("SIFT") == 0 ? "DES_HOG" : "DES_BINARY";
          string selectorType = "SEL_KNN";  // SEL_NN, SEL_KNN

          //// STUDENT ASSIGNMENT
          //// TASK MP.5 -> add FLANN matching in file matching2D.cpp
          //// TASK MP.6 -> add KNN match selection and perform descriptor
          /// distance
          /// ratio filtering with t=0.8 in file matching2D.cpp

          matchDescriptors((dataBuffer.end() - 2)->keypoints,
                           (dataBuffer.end() - 1)->keypoints,
                           (dataBuffer.end() - 2)->descriptors,
                           (dataBuffer.end() - 1)->descriptors, matches,
                           descriptorDataType, matcherType, selectorType);

          bool writeMatchResult = true;
          if (writeMatchResult) {
            string fullDir = "../data/output.txt";
            std::fstream out_file;
            out_file.open(fullDir, ios::app);
            if (out_file.is_open()) {
              out_file << detectorType << ", " << descriptorType << ", "
                       << imgIndex << ", " << matches.size() << ", "
                       << detector_t << ", " << descriptor_t << std::endl;
            }
            out_file.close();
          }

          //// EOF STUDENT ASSIGNMENT

          // store matches in current data frame
          (dataBuffer.end() - 1)->kptMatches = matches;

          cout << "#4 : MATCH KEYPOINT DESCRIPTORS done" << endl;

          // visualize matches between current and previous image
          bVis = true;
          if (bVis) {
            cv::Mat matchImg = ((dataBuffer.end() - 1)->cameraImg).clone();
            cv::drawMatches((dataBuffer.end() - 2)->cameraImg,
                            (dataBuffer.end() - 2)->keypoints,
                            (dataBuffer.end() - 1)->cameraImg,
                            (dataBuffer.end() - 1)->keypoints, matches,
                            matchImg, cv::Scalar::all(-1), cv::Scalar::all(-1),
                            vector<char>(),
                            cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

            string windowName = "Matching keypoints between two camera images";
            cv::namedWindow(windowName, 7);
            cv::imshow(windowName, matchImg);
            cout << "Press key to continue to next image" << endl;
            cv::waitKey(0);  // wait for key to be pressed
          }
          bVis = false;
        }
      }  // eof loop over all images
    }
  }
  return 0;
}
