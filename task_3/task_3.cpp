#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <string>
#include "json.hpp"
#include "mosquitto.h"
#include <sys/types.h>
#include <unistd.h>

#define mqtt_host "localhost"
#define mqtt_port 1883

using json = nlohmann::json;
using namespace cv;
using namespace std;


void avg_pts( std::vector<cv::Point> &points, size_t h)
{
     // size - количество отсчетов интервала усреднения
    double sumx = 0; // сумма отсчетов на интервале
    std::vector<double> mas(h, 0); // массив для хранения size отсчетов
    int index = 0; // индекс элемента массива
    for (int i = 0; i<points.size(); i++) {
        sumx -= mas[index];
        mas[index] = points[i].y;
        sumx += mas[index];
        index++;
        if (index >= h)
            index = 0; // возврат к началу "окна"
        points[i].y = sumx / h;
    }
  return;
}


int main(int argc, char* argv[])
{

    struct mosquitto* msqt;
    mosquitto_lib_init();
    msqt = mosquitto_new("mosquitto_id", 1, NULL);
    mosquitto_connect(	msqt, mqtt_host, mqtt_port, 5);

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
 std::vector<cv::Point> points;
 int count = 1;
 const int json_ratio = 5;

 while (true)
 {
  Mat frame;
  json j;

  bool bSuccess = cap.read(frame);
  if (bSuccess == false) 
  {
   cout << "Found the end of the video" << endl;
   break;
  }
	cv::Mat image_hsv;
	std::vector< std::vector<cv::Point> > contours;
	cv::cvtColor(frame, image_hsv, cv::COLOR_BGR2HSV );
	cv::Mat tmp_img(frame.size(),CV_8U);


	cv::inRange(image_hsv,
	cv::Scalar(20,0,100),
	cv::Scalar(100,100,255),
    tmp_img);

// "Замазать" огрехи в при выделении по цвету
	cv::dilate(tmp_img,tmp_img,cv::Mat(),cv::Point(-1,-1),3);
	cv::erode(tmp_img,tmp_img,cv::Mat(),cv::Point(-1,-1),1);

//Finding contours
    vector<Vec4i> hierarchy;
    findContours( tmp_img, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

    for( size_t i = 0; i< contours.size(); i++ )
    {
//Для каждой области определяем ограничивающий прямоугольник
		cv::Rect rect=cv::boundingRect(contours[i]);
//Adding rectangle bound to frame
		if (rect.area() > 1000){
           cv::rectangle(frame,rect,cv::Scalar(0,250,0),2);
            cv::Point point (rect.x + rect.width/2, rect.y + rect.height/2);
		    points.push_back(point); 
        }
    }
    for ( size_t i = 0; i< points.size()-1; i++ ){
        if (points.size() >= 2){
            cv::line(frame,points[i],points[i+1],cv::Scalar(255,0,0),2);
            if (count  % json_ratio == 0)
            {
                j["raw x"] = points[i].x;
                j["raw y"] = points[i].y;
            }
        }
    }

    int h = 5;
    std::vector<cv::Point> avg_points (points);
    avg_pts( avg_points, h);
    for ( size_t i = h; i< avg_points.size()-1; i++ ){
        if (avg_points.size() >= 2)
        {
            cv::line(frame,avg_points[i],avg_points[i+1],cv::Scalar(0,0,255),2);
            if (count % json_ratio == 0)
            {   
                j["avg x"] = avg_points[i].x;
                j["avg y"] = avg_points[i].y;
            }
        }
    }
    if (count % json_ratio == 0)
    {   
        std::string data = j.dump();
        const char* buf = data.c_str();
        mosquitto_publish(	msqt, NULL,"topic",data.size(), buf, 0, 1);
    }
	imshow( window_name,frame);
    count++;
  if (waitKey(10) == 27)
  {
   cout << "Esc key is pressed by user. Stoppig the video" << endl;
   break;
  }
 }

mosquitto_destroy(msqt);
mosquitto_lib_cleanup();
 return 0;

}