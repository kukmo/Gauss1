#pragma once

#include "stdafx.h"
#define _USE_MATH_DEFINES
#include "Gauss1.h"

int main(int argc, char** argv)
{
	Gauss1 gauss;
	gauss.loadParams(argc, argv);
	Mat * inputImage = gauss.loadImage();
	Mat * outputImage;
	gauss.imageCols = inputImage->cols;
	cout << gauss.imageCols << " image width \n";
	gauss.imageRows = inputImage->rows;
	cout << gauss.imageRows << " image height \n";

	gauss.compGaussianMatrix(5, 1.5);
	outputImage = gauss.performGauss(*inputImage);
	imwrite("new.jpeg",*outputImage);
	//cout << (float)inputImage.at<Vec3b>(10, 10).val[0];//pobieranie koloru blue
    return 0;
};

void Gauss1::compGaussianMatrix(int size, double sigma) {
	matrix = new double*[size];
	for (int i = 0; i < size; ++i) {
		matrix[i] = new double[size];
	}

	double result=0.;
	int i, j;
	for (i = 0; i < size; i++) {
		for (j = 0; j < size; j++) {
			matrix[i][j] = exp(-(i*i + j * j) / (2 * sigma*sigma)) / (2 * M_PI *sigma*sigma);
			result += matrix[i][j];
		}
	}

	for (i = 0; i<size; i++) {
		for (j = 0; j<size; j++) {
			matrix[i][j] = matrix[i][j] / result;
			cout << matrix[i][j] <<" ";
		}
		cout << "\n";
	}
};

Mat* Gauss1::performGauss(Mat &inputImage) {
	cout << inputImage.type()<< " type";
	static Mat newImageMat = Mat(this->imageRows - 4, this->imageCols - 4, 16);
	int newImageHeight = imageRows - 4;
	int newImageWidth = imageCols - 4;
	int d, i, j, h, w;
	//for (d = 0; d<3; d++) {
		for (i = 0; i<newImageWidth; i++) {
			for (j = 0; j<newImageHeight ; j++) {
				for (h = i; h<i + 5; h++) {
					for (w = j; w<j + 5; w++) {
						Vec3b *color1 = &newImageMat.at<Vec3b>(Point(i, j));
						Vec3b *color2 = &inputImage.at<Vec3b>(Point(h, w));
						double *matrixVal = &matrix[h-i][w-j];
						color1->val[0] += *matrixVal * color2->val[0];
						color1->val[1] += *matrixVal * color2->val[1];
						color1->val[2] += *matrixVal * color2->val[2];
						newImageMat.at<Vec3b>(Point(i, j)) = *color1;
						//cout << i << " i " << j << " j "<< h <<" h "<< w <<" w\n" ;
						//newImageMat. [d][i][j] += filter[h - i][w - j] * image[d][h][w];
					}
				}
			}
		}
		cout << "done here \n";
	//}

	return &newImageMat;
}

//wczytuje parametry z argv
 void Gauss1::loadParams(int argc, char** argv) {
	if (argv[0] != NULL) {
		string tc;
		tc = argv[1];
		threadCount = stoi(tc);
		cout << threadCount <<" \n";
	}
	if (argv[1] != NULL) {
		imageinputPath = argv[2];
		cout << imageinputPath << " \n";
	}
	if (argv[2] != NULL) {
		imageOutputPath = argv[3];
		cout << imageOutputPath << " \n";
	}
 };

 //zwraca macierz obrazu
 Mat * Gauss1::loadImage() {
	 static Mat inputImage;
	 if (imageinputPath.length() != 0) {
		 inputImage = imread(imageinputPath, IMREAD_COLOR);
		 if (inputImage.empty()) {
			 cout << "Cant load file! \n";
			 return NULL;
		 }
	 }
	 else {
		 cout << "error! Invalid image path!\n";
		 return NULL;
	 }
	 return &inputImage;
 }

