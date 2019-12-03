#ifndef refocusing_hpp
#define refocusing_hpp

#define REFOCUSING_THRESHOLD 0
#define REFOCUSING_DYNAMIC 1

#include <iostream>
#include <string>
#include <math.h>
#include <vector>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

class Refocusing
{
public:
    void LoadReferenceImage(Mat referenceImage);
    void LoadReferenceImage(string filename);
    void LoadDepthImage(Mat depthMapping);
    void LoadDepthImage(string filename);
    void Refocus(int option);
    void Refocus(int option, float threshold);

private:
    Mat getGaussianKernel(int rows, int cols, double sigmax, double sigmay);
    int ApplyGaussianToPixel(Point2i point, Mat kernel, Mat image, Size2i kernelSize);
    Mat CreateMask(float threshold);
    Mat CreateMask();
    void RefocusThreshold(float threshold);
    void RefocusDynamic(float threshold);

    Mat refImage, depthMapImage, resultImage;
};

#endif /* refocusing.hpp */