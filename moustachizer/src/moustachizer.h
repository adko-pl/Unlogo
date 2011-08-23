/*
 *  moustachizer.h
 *  moustachizer
 *
 *  Created by Jeffrey Crouse on 2/22/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */

#include "opencv2/opencv.hpp"
using namespace std;
using namespace cv;

#include "Tracker.h"
using namespace FACETRACKER;

class Moustachizer {
	public:
        Moustachizer() {};
		~Moustachizer() {};
		int init(const char* argstr);
		void process(Mat frame);
        void Draw(cv::Mat &image,cv::Mat &shape,cv::Mat &con,cv::Mat &tri,cv::Mat &visi);
        Mat rotateImage(const Mat& source, double angle);
    
	private:
		Mat stache, mask;
     
        double scale;
        Tracker model;
        cv::Mat frame,gray,im;
        std::vector<int> wSize1;
        std::vector<int> wSize2;
        bool failed;
        cv::Mat tri, con;
        bool show;
        bool fcheck; 
        int fpd; 
        int nIter; 
        double clamp;
        double fTol; 
        double fps; 
        //char sss[256]; 
        //std::string text; 
        //int64 t1,t0; 
        //int fnum;
        int idx;
    
    
};
