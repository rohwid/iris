#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
  // Image as matrix
  Mat image;

  image = imread("img/iris_500.png");

  // If not success, exit program
  if (!image.data)
  {
    cout << "Error loading the image" << endl;
    return -1;
  }

  // Show the original image
  imshow("Original Image", image);

  // Create a window called "Control"
  namedWindow("Control", CV_WINDOW_AUTOSIZE);

  int iLowH = 0;
  int iHighH = 179;

  int iLowS = 0;
  int iHighS = 255;

  int iLowV = 0;
  int iHighV = 255;

  /// Create trackbars in "Control" window
  // Hue (0 - 179)
  cvCreateTrackbar("LowH", "Control", &iLowH, 179);
  cvCreateTrackbar("HighH", "Control", &iHighH, 179);

  // Saturation (0 - 255)
  cvCreateTrackbar("LowS", "Control", &iLowS, 255);
  cvCreateTrackbar("HighS", "Control", &iHighS, 255);

  // Value (0 - 255)
  cvCreateTrackbar("LowV", "Control", &iLowV, 255);
  cvCreateTrackbar("HighV", "Control", &iHighV, 255);

  while (true)
  {
    Mat imgHSV;

    // Convert the captured frame from BGR to HSV
    cvtColor(image, imgHSV, COLOR_BGR2HSV);

    Mat imgThresholded;

    // Threshold the image
    inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded);

    // Morphological opening (remove small objects from the foreground)
    erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
    dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

    //morphological closing (fill small holes in the foreground)
    dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
    erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

    // Show the thresholded image
    imshow("Thresholded Image", imgThresholded);

    // Wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
    if (waitKey(30) == 27)
    {
      cout << "esc key is pressed by user" << endl;
      break;
    }
  }

  return 0;
}
