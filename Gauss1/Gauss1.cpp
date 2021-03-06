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
	//cout << gauss.imageCols << " image width \n";
	gauss.imageRows = inputImage->rows;
	//cout << gauss.imageRows << " image height \n";

	gauss.compGaussianMatrix(5, 1.5);

	chrono::steady_clock::time_point start = chrono::high_resolution_clock::now();
	outputImage = gauss.performGauss(*inputImage);
	chrono::steady_clock::time_point stop = chrono::high_resolution_clock::now();
	chrono::duration<double> elapsed = stop - start;
	cout << "Czas: " << elapsed.count();
	//ZMIENIC NA TO: 
	imwrite(gauss.imageOutputPath, *outputImage);
	//imwrite("new.jpeg", *outputImage);
	return 0;
};

//tworzy macierz rozmycia
void Gauss1::compGaussianMatrix(int size, double sigma) {
	gaussFilterSize = size;
	matrix = new double*[size];
	for (int i = 0; i < size; ++i) {
		matrix[i] = new double[size];
	}

	double result = 0.;
	int i, j;
	for (i = 0; i < size; i++) {
		for (j = 0; j < size; j++) {
			matrix[i][j] = exp(-(i*i + j * j) / (2 * sigma*sigma)) / (2 * M_PI *sigma*sigma);
			result += matrix[i][j];
		}
	}

	for (i = 0; i < size; i++) {
		for (j = 0; j < size; j++) {
			matrix[i][j] = matrix[i][j] / result;
			//cout << matrix[i][j] << " ";
		}
		//cout << "\n";
	}
};

//zwraca macierz po obrobce
Mat* Gauss1::performGauss(Mat &inputImage) {
	static Mat newImageMat = Mat(this->imageRows - gaussFilterSize + 1, this->imageCols - gaussFilterSize + 1, inputImage.type());
	int newImageHeight = imageRows - gaussFilterSize;
	int newImageWidth = imageCols - gaussFilterSize;
	int i, j, h, w;

	omp_set_dynamic(0); //disable dinamic thread creation 
	omp_set_num_threads(threadCount); //set number of threads
#pragma omp parallel for private(i,j,h,w)
	for (i = 0; i < newImageWidth; i++) {
		for (j = 0; j < newImageHeight; j++) {
			for (h = i; h < i + gaussFilterSize; h++) {
				for (w = j; w < j + gaussFilterSize; w++) {
					Vec3b *color1 = &newImageMat.at<Vec3b>(Point(i, j));
					Vec3b *color2 = &inputImage.at<Vec3b>(Point(h, w));
					double *matrixVal = &matrix[h - i][w - j];
					color1->val[0] += *matrixVal * color2->val[0];
					color1->val[1] += *matrixVal * color2->val[1];
					color1->val[2] += *matrixVal * color2->val[2];
					//cout << i << " i " << j << " j "<< h <<" h "<< w <<" w\n" ;
				}
			}
		}
	}
	return &newImageMat;
}

//wczytuje parametry z argv
void Gauss1::loadParams(int argc, char** argv) {

	threadCount = 4;
	imageinputPath = "2.jpg";
	imageOutputPath = "out.jpg";

	if (argv[0] != NULL) {
		string tc;
		tc = argv[1];
		threadCount = stoi(tc);
		//cout << threadCount <<" \n";
	}
	if (argv[1] != NULL) {
		imageinputPath = argv[2];
		//cout << imageinputPath << " \n";
	}
	if (argv[2] != NULL) {
		imageOutputPath = argv[3];
		//cout << imageOutputPath << " \n";
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

