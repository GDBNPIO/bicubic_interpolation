//
//  main.cpp
//  opencv1
//
//  Created by Guo on 2023/12/16.
//

#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

float hfuction(float x) {
    float xAbs = abs(x);
    if (xAbs <= 1) {
        return pow(xAbs, 3) - 2*pow(xAbs, 2) + 1;
    } else if (xAbs <= 2) {
        return -pow(xAbs, 3) + 5 * pow(xAbs, 2) - 8 * xAbs + 4;
    } else {
        return 0;
    }
}


float bicubicInterpolate(float p[4][4], float x1,float x2,float x3,float x4, float y1, float y2, float y3, float y4) {
    float hx[4]={x1,x2,x3,x4};
    float hy[4]={y1,y2,y3,y4};
    for(int i=0;i<4;++i){
        hx[i]=hfuction(hx[i]);
        hy[i]=hfuction(hy[i]);
    }
    
    float result = 0.0f;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result += hx[i] * p[i][j] * hy[j];
        }
    }
    return result;
}

//浮点转uchar
uchar saturate_cast_float_to_uchar(float val) {
    int iVal = val + 0.5f; // 加0.5实现四舍五入
    if (iVal < 0) return 0;
    if (iVal > 255) return 255;
    return iVal; // 直接赋值，隐式转换为 uchar
}

Vec3b getBicubicInterpolatedPixel(const Mat& img, float x, float y) {
    Vec3b result;
    float arr[4][4]; // 用于存储周围 16 个像素的值，这些值将用于插值计算

    int ix = floor(x); //ix 和 iy 是 x 和 y 坐标的整数部分
    int iy = floor(y);

    for (int m = 0; m < 3; m++) {
        for (int i = -1; i <= 2; i++) {
            for (int j = -1; j <= 2; j++) {
                int px = min(max(ix + i, 0), img.cols - 1);
                int py = min(max(iy + j, 0), img.rows - 1);
                arr[i + 1][j + 1] = img.at<Vec3b>(py, px)[m];
            }
        }
        result[m] = saturate_cast_float_to_uchar((bicubicInterpolate(arr, 1+x-ix,x-ix,1+ix-x,2+ix-x, 1+y-iy,y-iy,1+iy-y,2+iy-y)));
    }
    return result;
}

Mat resizeBicubic(const Mat& input, int newWidth, int newHeight) {
    Mat output(newHeight, newWidth, input.type());

    float x_ratio = float(input.cols - 1) / newWidth;//计算的是原始图像和新图像在水平和垂直方向上的缩放比例
    float y_ratio = float(input.rows - 1) / newHeight;
    cout << "width:" << input.cols << endl;
    cout << "height:" << input.rows << endl;
    cout << "newWidth:" << newWidth << endl;
    cout << "newHeight:" << newHeight << endl;
    //遍历新图像的每个像素
    for (int i = 0; i < newHeight; i++) {
        for (int j = 0; j < newWidth; j++) {
            float x_mapped = j * x_ratio;
            float y_mapped = i * y_ratio;
            output.at<Vec3b>(i, j) = getBicubicInterpolatedPixel(input, x_mapped, y_mapped);
        }
    }
    
    return output;
}

int main() {
    int aimed_area_width = 700;
    int aimed_area_height= 300;
    
    Mat img = imread("/Users/guoyijun/Desktop/mozu1.jpg");
    if (img.empty()) {
        cout << "Could not open or find the image" << endl;
        return -1;
    }

    Mat resized = resizeBicubic(img, 1024, 720);

    namedWindow("Original Image", WINDOW_AUTOSIZE);
    namedWindow("Resized Image", WINDOW_AUTOSIZE);
    imshow("Original Image", img);
    imshow("Resized Image", resized);
    
    Mat grayImage;
    cvtColor(resized, grayImage, COLOR_BGR2GRAY);
    // 创建一个 4x4 的矩阵来存储像素值
    vector<vector<uchar>> pixelMatrix1(4, vector<uchar>(4));

    // 填充矩阵
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            pixelMatrix1[i][j] = grayImage.at<uchar>(aimed_area_width + i, aimed_area_height + j);
        }
    }
    // 打印特定区域的灰度值
    cout << "4x4 grey value(without opencv function):" << endl;
    for (int i = aimed_area_width; i < aimed_area_width+4; i++) {
        for (int j = aimed_area_height; j < aimed_area_height+4; j++) {
            cout << setw(4) << static_cast<int>(grayImage.at<uchar>(i, j)) << " ";
        }
        cout << endl;
    }

    // print 16 RGB pixels of resized image
//    for (int i = 500; i < 504; i++) {
//        for (int j = 500; j < 504; j++) {
//            Vec3b pixel = resized.at<Vec3b>(i, j);
//            cout << "Pixel at (" << i << ", " << j << "): "
//                 << "B=" << int(pixel[0]) << ", "
//                 << "G=" << int(pixel[1]) << ", "
//                 << "R=" << int(pixel[2]) << endl;
//        }
//    }
    
    //用opencv bicubic函数
    Mat resizedWithOpenCV;
    resize(img, resizedWithOpenCV, Size(1024, 720), 0, 0, INTER_CUBIC);
    namedWindow("Resized by opencv function image", WINDOW_AUTOSIZE);
    imshow("Resized by opencv function image", resizedWithOpenCV);
    
    // print 16 RGB pixels of resized image by Opencv
//    for (int i = 500; i < 504; i++) {
//        for (int j = 500; j < 504; j++) {
//            Vec3b pixel = resizedWithOpenCV.at<Vec3b>(i, j);
//            cout << "Pixel at (" << i << ", " << j << "): "
//                 << "B=" << int(pixel[0]) << ", "
//                 << "G=" << int(pixel[1]) << ", "
//                 << "R=" << int(pixel[2]) << endl;
//        }
//    }
    // 将图像转换为灰度图
    Mat grayImageOpencv;
    cvtColor(resizedWithOpenCV, grayImageOpencv, COLOR_BGR2GRAY);
    // 创建一个 4x4 的矩阵来存储像素值
    vector<vector<uchar>> pixelMatrix(4, vector<uchar>(4));

    // 填充矩阵
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            pixelMatrix[i][j] = grayImageOpencv.at<uchar>(aimed_area_width + i, aimed_area_height + j);
        }
    }
    // 打印灰度值
    cout << "4x4 grey value(opencv function):" << endl;
    for (int i = aimed_area_width; i < aimed_area_width+4; i++) {
        for (int j = aimed_area_height; j < aimed_area_height+4; j++) {
            cout << setw(4) << static_cast<int>(grayImageOpencv.at<uchar>(i, j)) << " ";
        }
        cout << endl;
    }
    // 保存经过双三次插值调整大小的图像
    bool isSaved = imwrite("/Users/guoyijun/Desktop/resized_bicubic.jpg", resized);
    if (!isSaved) {
        cout << "Failed to save the bicubic resized image" << endl;
    }

    // 保存使用 OpenCV 函数调整大小的图像
    isSaved = imwrite("/Users/guoyijun/Desktop/resized_with_opencv.jpg", resizedWithOpenCV);
    if (!isSaved) {
        cout << "Failed to save the OpenCV resized image" << endl;
    }
    waitKey(0);
    return 0;
}

