#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

using namespace cv;
using namespace std;

Mat image, imgHSV;
Mat imgThresholded, threshold_output;
Mat linPolar, grayImage;
Mat grayImage_canny1, grayImage_canny2;
Mat OutputAbsDiff;

int thresh = 100;
int max_thresh = 255;

int ratio = 3;
int kernel_size = 3;
int lowThreshold;
int const max_lowThreshold = 100;

void CannyThreshold(int, void*)
{
  // convert to grayscale
  cvtColor(linPolar, grayImage, CV_BGR2GRAY);

  // edge detection
  Canny(grayImage, grayImage_canny1, 50, 150, kernel_size);

  // edge detection
  Canny(grayImage, grayImage_canny2, lowThreshold, lowThreshold * ratio, kernel_size);

  // Absdiff
  absdiff(grayImage_canny1, grayImage_canny2, OutputAbsDiff);

  // Show in a window
  namedWindow("Canny 1", CV_WINDOW_AUTOSIZE);
  imshow("Canny 1", grayImage_canny1);

  // Show in a window
  namedWindow("Canny 2", CV_WINDOW_AUTOSIZE);
  imshow("Canny 2", grayImage_canny2);

  // Show in a window
  namedWindow("Absolute Difference", CV_WINDOW_AUTOSIZE);
  imshow("Absolute Difference", OutputAbsDiff);
}

void thresh_callback(int, void* )
{
  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;

  // Detect edges using Threshold
  threshold(imgThresholded, threshold_output, thresh, 255, THRESH_BINARY);

  // Find contours
  findContours(threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

  // Approximate contours to polygons + get bounding rects and circles
  vector <vector<Point> > contours_poly(contours.size());
  vector <Rect> boundRect(contours.size());
  vector <Point2f> center(contours.size());
  vector <float> radius(contours.size());

  // Draw polygonal contour + bonding rects + circles
  Mat drawing = Mat::zeros(threshold_output.size(), CV_8UC3);
  Scalar color = Scalar(255, 0, 0);

  for(int i = 0; i < contours.size(); i++)
  {
    approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
    minEnclosingCircle((Mat)contours_poly[i], center[i], radius[i]);
    circle(image, center[i], (int)radius[i], color, 2, 8, 0);
  }

  circle(image, center[0], (int)radius[0] + 150, color, 2, 8, 0);

  Rect r(center[0].x - (radius[0] + 150), center[0].y - (radius[0] + 150), (radius[0] + 150) * 2, (radius[0]+150) * 2);

  // obtain the image ROI:
  Mat roi(image, r);

  // make a black mask, same size:
  Mat mask(roi.size(), roi.type(), Scalar::all(0));

  // with a white, filled circle in it:
  circle(mask, Point(radius[0] + 150, radius[0] + 150), radius[0] + 150, Scalar::all(255), -1);

  // combine roi & mask:
  Mat eye_cropped = roi & mask;

  // process linear polar
  linearPolar(eye_cropped, linPolar, Point2f(eye_cropped.size().height / 2, eye_cropped.size().width / 2), radius[0] + 150, CV_WARP_FILL_OUTLIERS);

  // Create a Trackbar for user to enter threshold
  namedWindow("Canny 2", CV_WINDOW_AUTOSIZE );
  createTrackbar("Min Threshold:", "Canny 2", &lowThreshold, max_lowThreshold, CannyThreshold);

  // Show the image
  CannyThreshold(0, 0);

  // Show in a window
  //namedWindow("Main", CV_WINDOW_AUTOSIZE);
  //imshow("Main", imgThresholded);

  // Show in a window
  namedWindow("Main", CV_WINDOW_AUTOSIZE);
  imshow("Main", image);

  // Show in a window
  namedWindow("Linear Polar", CV_WINDOW_AUTOSIZE);
  imshow("Linear Polar", linPolar);
}


int main(int argc, char** argv)
{
  image = imread("img/iris_500_1.png");

  // If not success, exit program
  if (!image.data)
  {
    cout << "Error loading the image" << endl;
    return -1;
  }

  int iLowH = 0;
  int iHighH = 179;

  int iLowS = 0;
  int iHighS = 255;

  int iLowV = 0;
  int iHighV = 5;

  // Create a window called "Control"
  namedWindow("Main", CV_WINDOW_AUTOSIZE);

  // Trackbars - Hue (0 - 179)
  createTrackbar("LowH", "Main", &iLowH, 179, thresh_callback);
  createTrackbar("HighH", "Main", &iHighH, 179, thresh_callback);

  // Trackbars - Saturation (0 - 255)
  createTrackbar("LowS", "Main", &iLowS, 255, thresh_callback);
  createTrackbar("HighS", "Main", &iHighS, 255, thresh_callback);

  // Trackbars - Value (0 - 255)
  createTrackbar("LowV", "Main", &iLowV, 255, thresh_callback);
  createTrackbar("HighV", "Main", &iHighV, 255, thresh_callback);

  while (true)
  {
    // Convert the captured frame from BGR to HSV
    cvtColor(image, imgHSV, COLOR_BGR2HSV);

    // Threshold the image
    inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded);

    // Morphological opening (remove small objects from the foreground)
    erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
    dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

    // morphological closing (fill small holes in the foreground)
    dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
    erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

    // Wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
    if (waitKey(30) == 27)
    {
      cout << "esc key is pressed by user" << endl;
      break;
    }
  }

  return 0;
}
