#include "stdafx.h"
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

//#include "json.h"
using namespace std;
using namespace cv;

Mat newrgb, src, outputImage;

void extractArtboard(int x, int y) {
	Vec3b whitepixel = Vec3b(255, 255, 255);
	// User clicked on background
	if (newrgb.at<Vec3b>(y, x) == Vec3b(255, 0, 0)) {
		return;
	}

	// If on boundary dont allow 
	if (outputImage.at<Vec3b>(y, x) == whitepixel) {
		return;
	}

	int left = x, top = y, width = x, height = y;

	while (--left > 0) {
		if (outputImage.at<Vec3b>(y, left) == whitepixel) {
			break;
		}
	}

	while (--top > 0) {
		if (outputImage.at<Vec3b>(top, x) == whitepixel) {
			break;
		}
	}

	while (++width < (outputImage.cols - x)) {
		if (outputImage.at<Vec3b>(y, width) == whitepixel) {
			width = width - left;
			break;
		}
	}

	while (++height < (outputImage.rows - y)) {
		if (outputImage.at<Vec3b>(height, x) == whitepixel) {
			height = height - top;
			break;
		}
	}

	Mat artboard(src, Rect(left, top, width, height - 10));
	namedWindow("artboard", CV_WINDOW_AUTOSIZE);
	imshow("artboard", artboard);
	imwrite("C:/Users/aasgarg/Desktop/shore_fill_artboard.png", artboard);
}

void CallbackFunc(int event, int x, int y, int flags, void* userdata) {
	if (event == EVENT_LBUTTONDOWN) {
		extractArtboard(x, y);
	}
}

void edgeDetection() {
	Vec3b pixelVal = newrgb.at<Vec3b>(0, 0);

	for (int row = 0; row < newrgb.rows; row++) {
		for (int col = 0; col < newrgb.cols; col++) {
			if (newrgb.at<Vec3b>(row, col) == pixelVal) {
				outputImage.at<Vec3b>(row, col) = Vec3b(0, 0, 0);
			}
			else if (col != 0 && (newrgb.at<Vec3b>(row, col - 1) == pixelVal)) {
				outputImage.at<Vec3b>(row, col) = Vec3b(255, 255, 255);
			}
			else if (col != (newrgb.cols - 1) && (newrgb.at<Vec3b>(row, col + 1) == pixelVal)) {
				outputImage.at<Vec3b>(row, col) = Vec3b(255, 255, 255);
			}
			else if (row != 0 && (newrgb.at<Vec3b>(row - 1, col) == pixelVal)) {
				outputImage.at<Vec3b>(row, col) = Vec3b(255, 255, 255);
			}
			else if (row != (newrgb.rows - 1) && (newrgb.at<Vec3b>(row + 1, col) == pixelVal)) {
				outputImage.at<Vec3b>(row, col) = Vec3b(255, 255, 255);
			}
			else {
				outputImage.at<Vec3b>(row, col) = Vec3b(0, 0, 0);
			}
		}
	}
	imwrite("C:/Users/aasgarg/Desktop/shore_fill_edge.png", outputImage);
	namedWindow("trial12", CV_WINDOW_AUTOSIZE);
	imshow("trial12", outputImage);

	setMouseCallback("trial12", CallbackFunc, NULL);
}

int main(int argc, char** argv)
{
	//FileStorage file("C:/Users/aasgarg/Desktop/fill.png", FileStorage::WRITE);
	src = imread("C:/Users/aasgarg/Desktop/masking-1.jpg");
	Mat gray, edge, draw;
	cvtColor(src, gray, CV_BGR2GRAY);
	Canny(gray, edge, 0, 255, 3);

	cvtColor(gray, newrgb, CV_GRAY2BGR);
	// ****************************
	//floodFill(newrgb, Point(0, 0), Scalar(255, 0, 0));
	uchar pixelVal = gray.at<uchar>(0, 0);
	newrgb.at<Vec3b>(0, 0) = Vec3b(255, 0, 0);
	for (int row = 0; row < newrgb.rows; row++) {
		for (int col = 0; col < newrgb.cols; col++) {
			if (row == 0 && col == 0) {
				continue;
			}
			if (abs(gray.at<uchar>(row, col) - pixelVal) <= 20 &&
				((col > 0 && newrgb.at<Vec3b>(row, col - 1) == Vec3b(255, 0, 0)) ||
				(row > 0 && newrgb.at<Vec3b>(row - 1, col) == Vec3b(255, 0, 0)) ||
				(col > 0 && row > 0 && newrgb.at<Vec3b>(row - 1, col - 1) == Vec3b(255, 0, 0)))) {
				newrgb.at<Vec3b>(row, col) = Vec3b(255, 0, 0);
			}
		}
	}


	namedWindow("canny1", CV_WINDOW_AUTOSIZE);
	imshow("canny1", edge);

	namedWindow("canny", CV_WINDOW_AUTOSIZE);
	imshow("canny", newrgb);

	Mat output(src.rows, src.cols, CV_8UC3, Scalar(0, 0, 0));
	outputImage = output;
	edgeDetection();
	/*vector<Mat> bgr_planes;
	split(src, bgr_planes);

	/// Establish the number of bins
	int histSize = 256;

	/// Set the ranges ( for B,G,R) )
	float range[] = { 0, 256 };
	const float* histRange = { range };

	bool uniform = true; bool accumulate = false;

	Mat b_hist, g_hist, r_hist;

	/// Compute the histograms:
	calcHist(&bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate);

	// Draw the histograms for B, G and R
	int hist_w = src.cols; int hist_h = src.rows;
	int bin_w = cvRound((double)hist_w / histSize);

	Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));



	/// Normalize the result to [ 0, histImage.rows ]
	/*normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
	normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
	normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());*/

	/// Draw for each channel
	/*for (int i = 1; i < 2; i++)
	{
	//Rect rect(Point(bin_w*(i - 1), 0), Point(bin_w*(i), hist_h));
	//rectangle(histImage, rect, Scalar(255, 255, 255), 2);
	/*line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(b_hist.at<float>(i - 1))),
	Point(bin_w*(i), hist_h - cvRound(b_hist.at<float>(i))),
	Scalar(255, 0, 0), 7, 8, 0);
	line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(g_hist.at<float>(i - 1))),
	Point(bin_w*(i), hist_h - cvRound(g_hist.at<float>(i))),
	Scalar(0, 255, 0), 7, 8, 0);
	line(histImage, Point(0,0),
	Point(cvRound(r_hist.at<float>(i)), cvRound(r_hist.at<float>(i))),
	Scalar(0, 0, 255), 7, 8, 0);
	}*/
	/*int sum = 0;
	for (int i = 0; i < histSize; i++)
	{
	std::cout << cvRound(b_hist.at<float>(i)) << ",";
	std::cout << cvRound(g_hist.at<float>(i)) << ",";
	std::cout << cvRound(r_hist.at<float>(i)) << endl;
	}*/


	//namedWindow("edge", CV_WINDOW_NORMAL);
	//imshow("edge", histImage);
	// ****************************
	/*Mat parsedImg(src.rows, src.cols, CV_8UC1);
	int numBins = 256;
	int range[256] = { 0 };
	CvHistogram *hist = cvCreateHist(1, &numBins, CV_HIST_ARRAY, ranges, 1);
	cvClearHist(hist);
	Mat imgRed(src.rows, src.cols, CV_8UC1);
	Mat imgGreen(src.rows, src.cols, CV_8UC1);
	Mat imgBlue(src.rows, src.cols, CV_8UC1);
	//cvtColor(src, gray, CV_BGR2GRAY);
	uchar val = gray.at<uchar>(0, 0);
	for (int row = 0; row < src.rows; row++) {
	for (int col = 0; col < src.cols; col++) {
	if (gray.at<uchar>(row, col) == val) {
	parsedImg.at<uchar>(row, col) = 255;
	}
	else {
	parsedImg.at<uchar>(row, col) = 0;
	}
	}
	}*/
	//Canny(gray, edge, 150, 200, 3);
	/*cvtColor(edge, output, COLOR_GRAY2BGR);
	edge.convertTo(draw, CV_8U);
	vector<Vec4i> lines;
	HoughLinesP(draw, lines, 1, CV_PI/180, 50, 50, 10);
	for (size_t i = 0; i < lines.size(); i++) {
	Vec4i l = lines[i];
	line(output, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, CV_AA);
	}*/


	//namedWindow("draw", CV_WINDOW_NORMAL);
	//imshow("draw", parsedImg);

	/*Json::CharReaderBuilder builder;
	Json::CharReader* reader = builder.newCharReader();
	Json::Value root;
	std::string errors;
	// Below represents the JSON object for left bound of each screen
	std::string xPos = "{ \"1\": 175, \"2\": 452, \"3\": 761, \"4\": 220, \"5\": 473}";

	// Below represents the JSON object for top bound of each screen
	std::string yPos = "{ \"1\": 79, \"2\": 212, \"3\": 357, \"4\": 463, \"5\": 547}";

	// Below represents the JSON object for top bound of each screen
	std::string boundsSize = "{ \"1\": {2,2}, \"2\": {q,q}, \"3\": {e,e}, \"4\": {3,1}, \"5\": {4,2}}";


	if (reader->parse(boundsSize.c_str(), boundsSize.c_str() + boundsSize.size(), &root, &errors)) {
	for (Json::Value::const_iterator outer = root.begin(); outer != root.end(); outer++) {
	cout << outer.key() << ":" << *outer << endl;
	}
	}*/
	waitKey();
	return 0;
}
