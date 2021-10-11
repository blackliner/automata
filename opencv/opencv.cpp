#include <iostream>

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"

using namespace cv;
using std::cout;
const int alpha_slider_max = 100;
int alpha_slider;
double alpha;
double beta;

static void on_trackbar(int, void*) {
  alpha = (double)alpha_slider / alpha_slider_max;
  beta = (1.0 - alpha);
}

int main(void) {
  alpha_slider = 0;
  Mat frame = imread(samples::findFile("opencv/WindowsLogo.jpg"));

  namedWindow("Linear Blend", WINDOW_AUTOSIZE);  // Create Window
  std::string TrackbarName = "Alpha x " + std::to_string(alpha_slider_max);
  createTrackbar(TrackbarName, "Linear Blend", &alpha_slider, alpha_slider_max, on_trackbar);
  on_trackbar(alpha_slider, 0);
  while (waitKey(50) != 'q') {
    Mat dest = frame.clone();
    Point start{0, 0};
    Point end{0, alpha_slider};
    line(dest, start, end, Scalar(0, 0, 0), 5, LINE_AA);
    imshow("Linear Blend", dest);
  }

  return 0;
}
