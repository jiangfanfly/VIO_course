#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <thread>
#include <iomanip>

#include <cv.h>
#include <opencv2/opencv.hpp>
#include <highgui.h>
#include <eigen3/Eigen/Dense>
#include "System.h"

using namespace std;
using namespace cv;
using namespace Eigen;

const int nDelayTimes = 2;
string sData_path="/home/jiangfan/vio_course/ch7/simulation_data/";
string sConfig_path="../config/";
double image_w = 640;
double image_h = 640;
double t = 0;

std::shared_ptr<System> pSystem;

void PubImuData()
{
	// string sImu_data_file = sConfig_path + "imu_pose.txt";
    string sImu_data_file = sConfig_path + "imu_pose_noise.txt";
	cout << "1 PubImuData start sImu_data_filea: " << sImu_data_file << endl;
	ifstream fsImu;
	fsImu.open(sImu_data_file.c_str());
	if (!fsImu.is_open())
	{
		cerr << "Failed to open imu file! " << sImu_data_file << endl;
		return;
	}

	std::string sImu_line;
	double dStampNSec = 0.0;
	Vector3d vAcc;
	Vector3d vGyr;
    Eigen::Quaterniond q;
    Eigen::Vector3d t;
	while (std::getline(fsImu, sImu_line) && !sImu_line.empty()) // read imu data
	{
		std::istringstream ssImuData(sImu_line);
		ssImuData >> dStampNSec >> q.w() >> q.x() >> q.y() >> q.z() >> t(0)>> t(1) >> t(2) >> vGyr.x() >> vGyr.y() >> vGyr.z() >> vAcc.x() >> vAcc.y() >> vAcc.z();
		// cout << "Imu t: " << fixed << dStampNSec << " gyr: " << vGyr.transpose() << " acc: " << vAcc.transpose() << endl;
		pSystem->PubImuData(dStampNSec, vGyr, vAcc);
		usleep(5000*nDelayTimes);
	}
	fsImu.close();
}

void PubImageData()
{
	string sImage_file = sConfig_path + "keyframe/all_points_";
	cout << "1 PubImageData start sImage_file: " << sImage_file << endl;
    string config_file = sConfig_path + "simu_config.yaml";
    cv::FileStorage fsSettings(config_file, cv::FileStorage::READ);
    if (!fsSettings.isOpened())
    {
        cerr << "1 readParameters ERROR: Wrong path to settings!" << endl;
        return;
    }
            cv::FileNode n = fsSettings["projection_parameters"];
        double fx = static_cast<double>(n["fx"]);
        double fy = static_cast<double>(n["fy"]);
        double cx = static_cast<double>(n["cx"]);
        double cy = static_cast<double>(n["cy"]);

    string featuer_file;
    vector<cv::Point2f> points;
    for(int i =0; i<600;i++)
    {
        featuer_file = sImage_file + to_string(i) + ".txt";
        ifstream fsImage;
        fsImage.open(featuer_file.c_str());
        if (!fsImage.is_open())
        {
            cerr << "Failed to open image feature file! " << featuer_file << endl;
            return;
        }
        t += 1.0/30;
        cv::Point3d p;
        cv::Point2f feature,feature2;
        vector<cv::Point2f> pointsa;
        double u,v;
        int num;
        Mat image(image_w, image_h, CV_8UC1, Scalar(0,0,0));
        // cv::Vec3b a(255,255,255);
        while(fsImage >> p.x >> p.y >> p.z >> num >> feature.x >> feature.y)
        {
            //fsImage >> p.x >> p.y >> p.z >> num >> feature.x >> feature.y;
            //cout<<p.x <<" "<<p.y <<" "<<p.z <<" "<<num <<" "<<feature.x<<" "<<feature.y<<endl;
            u=fx*feature.x+cx;
            v=fy*feature.y+cy;
             feature2.x = u;
             feature2.y = v;
            points.push_back(feature);
            pointsa.push_back(feature2);
            // if(u< 640 && u > 0 && v > 0 && v < 640 )
            // {
            //     image.at<cv::Vec3b>(v,u) = a;
            //     cout<< u <<" "<< v <<endl;
            // }
        }
        fsImage.close();
        // cout<<"1-----"<<endl;
        // for(size_t i=0;i<points.size(); i++)
        // {
        //     cout<< points[i].x<<" "<<points[i].y<<endl;
        // }
        
        pSystem->PubSimuImageData(t,image, points);

        //pSystem->PubImageData(t,image);
        for ( int j = 0; j< pointsa.size(); j++)
        {
            if(pointsa[j].x< 640 && pointsa[j].x > 0 && pointsa[j].y > 0 && pointsa[j].y < 640 )
			    cv::circle(image, pointsa[j], 2, cv::Scalar(255 , 0, 255), 2);
		}
        cv::namedWindow("IMAGE2", CV_WINDOW_AUTOSIZE);
		cv::imshow("IMAGE2", image);
        cv::waitKey(1);
        usleep(50000*nDelayTimes);
        points.clear();
    }
}


int main(int argc, char **argv)
{
	if(argc != 3)
	{
		cerr << "./simulation PATH_TO_FOLDER/keyframe PATH_TO_CONFIG/config \n" 
			<< "For example: ./simulation /home/jiangfan/vio_course/ch7/simulation_data/ ../config/"<< endl;   //simulation_data.yaml
		return -1;
	}

	sData_path = argv[1];
	sConfig_path = argv[2];
    //std::cout<<sConfig_path<<endl;

	pSystem.reset(new System(sConfig_path));
	
	std::thread thd_BackEnd(&System::ProcessBackEnd, pSystem);
		
	// sleep(5);
	std::thread thd_PubImuData(PubImuData);

	std::thread thd_PubImageData(PubImageData);
	
	std::thread thd_Draw(&System::Draw, pSystem);
	
	thd_PubImuData.join();
	thd_PubImageData.join();
                                           
	thd_BackEnd.join();
	thd_Draw.join();

	cout << "main end... see you ..." << endl;
	return 0;
}
