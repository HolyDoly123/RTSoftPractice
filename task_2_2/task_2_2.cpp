#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

using namespace cv;
using namespace std;



int main(int argc, char* argv[])
{
 //open the video file for reading
 VideoCapture cap("video/example.MOV"); 

 // if not success, exit program
 if (cap.isOpened() == false)  
 {
  cout << "Cannot open the video file" << endl;
  cin.get(); //wait for any key press
  return -1;
 }

 //Uncomment the following line if you want to start the video in the middle
 cap.set(CAP_PROP_POS_MSEC, 300); 


 String window_name = "My Video";
 namedWindow(window_name, WINDOW_AUTOSIZE); //create a window

// Canny filter params and trackbar
int lowThreshold = 30;
 int ratio = 3;
 //createTrackbar( "Min Threshold:", window_name, &lowThreshold, 1000);
 // createTrackbar( "Min Ratio:", window_name, &ratio, 100);  

 while (true)
 {
  Mat frame;
  
  bool bSuccess = cap.read(frame);
  if (bSuccess == false) 
  {
   cout << "Found the end of the video" << endl;
   break;
  }
	std::vector<cv::Mat> stickers;
	cv::Mat image_hsv;
	std::vector< std::vector<cv::Point> > contours;
	cv::cvtColor(frame, image_hsv, cv::COLOR_BGR2HSV );
	cv::Mat mask1(frame.size(),CV_8U), mask2(frame.size(),CV_8U), tmp_img(frame.size(),CV_8U);
// Выделение шарика
	cv::inRange(image_hsv,
	cv::Scalar(10,40,210),
	cv::Scalar(30,255,255),
mask1);

//  второй шарик
	cv::inRange(image_hsv,
	cv::Scalar(20,0,100),
	cv::Scalar(100,100,255),
mask2); 
    cv::bitwise_or ( mask1, mask2, tmp_img	) ;		
// "Замазать" огрехи в при выделении по цвету
	cv::dilate(tmp_img,tmp_img,cv::Mat(),cv::Point(-1,-1),3);
	cv::erode(tmp_img,tmp_img,cv::Mat(),cv::Point(-1,-1),1);

//Gaussian blur for noise reduction
/*
    Mat nemat;
	int MAX_KERNEL_LENGTH = 5;
    for ( int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2 )
    {
        GaussianBlur(frame,nemat, Size( i, i ), 0, 0 );
    }
*/
//Canny filter
	Mat canny_output;
    Canny( tmp_img, canny_output,lowThreshold,lowThreshold*ratio);

//Finding contours
    vector<Vec4i> hierarchy;
    findContours( canny_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

    for( size_t i = 0; i< contours.size(); i++ )
    {
		cv::Mat sticker;
//Для каждой области определяем ограничивающий прямоугольник
		cv::Rect rect=cv::boundingRect(contours[i]);
//Adding rectangle bound to frame
		if (rect.area() > 1000) cv::rectangle(frame,rect,cv::Scalar(0,250,0),2);
		frame(rect).copyTo(sticker);
		stickers.push_back(sticker); //Добавить к массиву распознанных стикеров
//Drawing contours
        drawContours( frame, contours, (int)i, Scalar( 0, 0, 0), 2, LINE_8, hierarchy, 0 );
    }

//Итог сделан через поиск контуров с фильтром Кенни
	imshow( window_name, frame);
  if (waitKey(10) == 27)
  {
   cout << "Esc key is pressed by user. Stoppig the video" << endl;
   break;
  }
 }

 return 0;

}