#include "refocusing.hpp"

void Refocusing::LoadReferenceImage(Mat referenceImage)
{
    refImage = referenceImage.clone();
}

void Refocusing::LoadReferenceImage(string filename)
{
    refImage = imread(filename, CV_LOAD_IMAGE_COLOR);
}

void Refocusing::LoadDepthImage(Mat depthMapping)
{
    depthMapImage = depthMapping.clone();
}

void Refocusing::LoadDepthImage(string filename)
{
    depthMapImage = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);
}

Mat Refocusing::MultMatrices(Mat nChannelsMatrix, Mat singleChannelMatrix)
{
    vector<Mat> channels;
    Mat result;
    split(nChannelsMatrix, channels);

    for(int c = 0; c < channels.size(); c++)
    {
        channels[c] = channels[c].mul(singleChannelMatrix);
    }

    merge(channels, result);

    return result;
}

Mat Refocusing::CreateMask(float threshold, int option)
{
    Mat mask;
    mask = depthMapImage.clone();
    cv::threshold(mask, mask, threshold, 1, option);
    return mask;
}

Mat Refocusing::CreateMask(int option)
{
    return CreateMask(127, option);
}

Mat Refocusing::CreateMask(float threshold)
{
    return CreateMask(threshold, CV_THRESH_BINARY);
}

Mat Refocusing::CreateMask()
{
    return CreateMask(127, CV_THRESH_BINARY);
}

void Refocusing::RefocusThreshold(float threshold)
{
    Mat blurred, nonBlurred, mask, invMask;
    double min, max;

    nonBlurred = refImage.clone();

    mask = CreateMask(threshold, CV_THRESH_BINARY_INV);
    invMask = CreateMask(threshold, CV_THRESH_BINARY);

    GaussianBlur(refImage, blurred, Size(23, 23), 0, 0);

    blurred = MultMatrices(blurred, mask);
    imwrite("blurred.jpg", blurred);
    nonBlurred = MultMatrices(nonBlurred, invMask);
    imwrite("nonblurred.jpg", nonBlurred);

    resultImage = blurred + nonBlurred;
}

Mat Refocusing::GetGaussianKernel(int rows, int cols, double sigmax, double sigmay)
{
    auto gauss_x = cv::getGaussianKernel(cols, sigmax, CV_32F);
    auto gauss_y = cv::getGaussianKernel(rows, sigmay, CV_32F);
    return gauss_x * gauss_y.t();
}

int Refocusing::ApplyGaussianToPixel(Point2i point, Mat kernel, Mat image, Size2i kernelSize)
{
    Size2i imageSize = image.size();
    int ymin, xmin;
    float ac = 0;

    ymin = point.y - (kernelSize.height - 1)/2;
    xmin = point.x - (kernelSize.width - 1)/2;

    for(int i = 0; i < kernelSize.height; i++)
    {
        if(ymin + i < 0 || ymin + i>= imageSize.height)
            continue;

        for(int j = 0; j < kernelSize.width; j++)
        {
            if(xmin + j < 0 || xmin + j >= imageSize.width)
                continue;

            ac += image.at<float>(ymin + i, xmin + j, 0)*(kernel.at<float>(i, j, 0));
        }
    }
    image.at<int>(point.y, point.x) = round(ac);


    return round(ac);
}

void Refocusing::RefocusDynamic(float threshold)
{
    Size2i imageSize = refImage.size();
    vector<Mat> channels;
    int kernelSize = 9;
    Mat gaussianKernel;

    cout << "Criei variaveis" << endl;

    cvtColor(refImage, resultImage, CV_RGB2GRAY);

    cout << "Clonei imagem para resultado" << endl;

    for(int i = 0; i < imageSize.height; i++)
    {
        for(int j = 0; j < imageSize.width; j++)
        {
            // kernelSize = std::abs(-depthMapImage.at<int>(i,j) + 255 - threshold);
            gaussianKernel = Refocusing::GetGaussianKernel(kernelSize, kernelSize, -1, -1);

            ApplyGaussianToPixel(Point2i(j, i), gaussianKernel, resultImage, Size2i(kernelSize, kernelSize));
        }
    }

    cout << "Terminei o loop" << endl;
}

void Refocusing::Refocus(int option, float threshold)
{
    switch(option)
    {
        case REFOCUSING_THRESHOLD:
            RefocusThreshold(threshold);
            break;
        case REFOCUSING_DYNAMIC:
            RefocusDynamic(threshold);
            break;
    }
    namedWindow("Refocused image");
    imshow("Refocused image", resultImage);
    imwrite("refocused.jpg", resultImage);
}

void Refocusing::Refocus(int option)
{
    Refocus(option, 127);
}