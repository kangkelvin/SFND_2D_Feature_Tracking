## MP.1 Data Buffer Optimization

MidTermProject_Camera_Student.cpp:74-76

Check databuffer size, if it's larger than threshold, delete the earliest data

## MP.2 Keypoint Detection

matching2D_Student.cpp:123-188

## MP.3 Keypoint Removal

MidTermProject_Camera_Student.cpp:120-131

Check for each raw keypoints detected if it is inside the Rect using the cv::Rect::contains function, and push_back the keypoints inside to a separate container

## MP.4 Keypoint Descriptors

matching2D_Student.cpp:58-81

## MP.5 Descriptor Matching

matching2D_Student.cpp:23-41

## MP.6 Descriptor Distance Ratio

matching2D_Student.cpp:43-50

## MP.7 Performance Evaluation 1

Raw data is at `data/results.ods` under `MP 7` tab

1. SHITOMASI: spreads evenly on the vehicle
2. HARRIS: very few features captured, mostly on the upper are of vehicle
3. SIFT: evenly spreads throughout vehicle, but captures features on other objects too
4. AKAZE: only captures the perimeter of the vehicle
5. ORB: clusters around the brake lights and top-right corner of vehicle
6. BRISK: spreads quite evenly around vehicles
7. FAST: spreads around the vehicles but captures features on  other objects too

## MP.8 Performance Evaluation 2

raw data located at `data/results.ods` under `MP 8 and 9` tab


## MP.9 Performance Evaluation 3

Feature tracking for autonomous vehicle is time-sensitive, hence the best combinations are chosen by the ratio of keypoints matches to total time taken.

The second criteria is based on a good and even distributions of keypoints detected on the preceding vehicles, and FAST produced a more even spread than ORB

Hence, the best combinations is

| Detector | Descriptor | kpts match / time(ms) |
| -------- | ---------- | --------------------- |
| FAST     | ORB        | 37.7                  |
| FAST     | BRIEF      | 35.4                  |
| ORB      | BRIEF      | 9.2                   |

