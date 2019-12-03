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

Mat Refocusing::CreateMask(float threshold)
{
    Mat mask(depthMapImage.size(), CV_16S);
    vector<Mat> chans;
    for(int i = 0; i < depthMapImage.size().height; i++)
    {
        for(int j = 0; j < depthMapImage.size().width; j++)
        {
            if(depthMapImage.at<float>(i,j) < threshold)
            {
                mask.at<float>(i,j) = 1;
            }
            else
            {
                mask.at<float>(i,j) = 0;
            }
        }
    }
    for(int i = 0; i < 3; i++)
        chans.push_back(mask.clone());
    cv::merge(chans, mask);
    return mask; 
}

Mat Refocusing::CreateMask()
{
    return CreateMask(50);
}

void Refocusing::RefocusThreshold(float threshold)
{
    Mat blurred, nonBlurred, mask;
    mask = CreateMask(threshold);
    cout << "Criei a mascara" << endl;

    GaussianBlur(refImage, blurred, Size(13, 13), 0, 0);

    cout << "Tipo da imagem: " << refImage.type() << endl;

    namedWindow("mask");
    imshow("mask", mask);
    cout << "Criei a imagem borrada" << endl;
    blurred = blurred.mul(mask);
    namedWindow("blurred");
    imshow("blurred", blurred);
    cout << "Criei a parte borrada" << endl;
    nonBlurred = refImage.mul(mask);
    namedWindow("nonblurred");
    imshow("nonblurred", nonBlurred);

    waitKey(0);

    resultImage = blurred + nonBlurred;
}

Mat Refocusing::getGaussianKernel(int rows, int cols, double sigmax, double sigmay)
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

            ac += image.at<float>(ymin + i, xmin + j)*(kernel.at<float>(i, j));
        }
    }
    image.at<int>(point.y, point.x) = round(ac);


    return round(ac);
}

void Refocusing::RefocusDynamic(float threshold)
{
    Size2i imageSize = refImage.size();
    vector<Mat> channels;
    int kernelSize = 3;
    Mat gaussianKernel;

    cout << "Criei variaveis" << endl;

    cvtColor(refImage, resultImage, CV_RGB2GRAY);

    cout << "Clonei imagem para resultado" << endl;

    for(int i = 0; i < imageSize.height; i++)
    {
        for(int j = 0; j < imageSize.width; j++)
        {
            // kernelSize = std::abs(-depthMapImage.at<int>(i,j) + 255 - threshold);
            gaussianKernel = Refocusing::getGaussianKernel(kernelSize, kernelSize, -1, -1);

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
}

void Refocusing::Refocus(int option)
{
    Refocus(option, 127);
}