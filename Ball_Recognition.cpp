#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
using namespace cv;

#include <iostream>
#include <stdio.h>
using namespace std;


class balldetection {
    private:
        void EliminateWhite(Mat &isolated_color, Mat subtract_color1, Mat subtract_color2){ 

            subtract(isolated_color, subtract_color1, isolated_color);
            subtract(isolated_color, subtract_color2, isolated_color);

        }
        void Erosion(Mat &src) {
            int erosion_elem = 1;
            int erosion_size = 4;
            int const max_elem = 2;
            int const max_kernel_size = 21;
            int erosion_type = MORPH_RECT;
            Mat element = getStructuringElement(erosion_type, Size(2 * erosion_size + 1, 2 * erosion_size + 1), Point(erosion_size, erosion_size));

            erode(src, src, element);

        }

        void Dilation(Mat &src){
            int dilation_elem = 1;
            int dilation_size = 4;
            int const max_elem = 2;
            int const max_kernel_size = 21;
            int dilation_type = MORPH_RECT;
            Mat element = getStructuringElement(dilation_type, Size(2 * dilation_size + 1, 2 * dilation_size + 1), Point(dilation_size, dilation_size));

            dilate(src, src, element);

        }
    public:
        void CleanFrame(Mat frame, Mat &clean) {
            // take Red, Green, Blue element of a frame

            Mat rgbchannel[3];
            split(frame, rgbchannel);


            Mat blue = rgbchannel[0];
            Mat green = rgbchannel[1];
            Mat red = rgbchannel[2];

            // eliminate multi channel colors

            clean = blue.clone();

            EliminateWhite(clean, green, red);

            uchar threshhold = 2;
            
            for (int x = 0; x < clean.rows; x++) {
                for (int y = 0; y < clean.cols; y++) {
                    // Accesssing values of each pixel
                    if (clean.at<uchar>(x, y) > threshhold) {
                        // Setting the pixel values to 255 if it's above the threshold
                        clean.at<uchar>(x, y) = 255;
                    }
                    else if (clean.at<uchar>(x, y) <= threshhold) {
                        // Setting the pixel values to 255 if it's below the threshold
                        clean.at<uchar>(x, y) = 0;
                    }
                }
            }

            Dilation(clean);
            Dilation(clean);
            Erosion(clean);
            Erosion(clean);
            Erosion(clean);
            Dilation(clean);
        }
};


int main()
{
    Mat frame;
    Mat clean;
    VideoCapture cap;
    cap.open(0);

    int c = 0;

    balldetection balldetector;

    if (!cap.isOpened()) {
        cerr << "ERROR! Unable to open camera\n";
        return -1;
    }

    cout << "Start grabbing" << endl
        << "Press any key to terminate" << endl;
    for (;;)
    {
        cap.read(frame);
        if (frame.empty()) {
            cerr << "ERROR! blank frame grabbed\n";
            break;
        }
        imshow("Live", frame);
        balldetector.CleanFrame(frame, clean);
        imshow("Clean", clean); //return a frame with two values of mostly the assigned color


        Moments m = moments(clean, true);
        Point p(m.m10 / m.m00, m.m01 / m.m00);

        cout << Mat(p) << endl;

        circle(frame, p, 5, Scalar(0, 0, 255), -1);
        imshow("Center", frame);

        c++;
        cout << c << endl;
        if (waitKey(5) >= 0) {
            break;
        }
    }
    return 0;
}
