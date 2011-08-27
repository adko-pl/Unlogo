/*
 *  moustachizer.cpp
 *  moustachizer
 *
 *  Created by Jeffrey Crouse on 2/22/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */

#include "moustachizer.h"



// ------------------------------
int Moustachizer::init(char* argstr) {

    char* tok = strtok(argstr, ":");
    vector<string> args;
    while ( tok != NULL ) {
        args.push_back( tok );
        tok = strtok(NULL, ":");
    }
    if(args.size() < 4)
    {
        cout << "must supply 4 arguments" << endl;
        return 1;
    }
    
    cout << "moustache: " << args[1] << endl;
    cout << "mask: " << args[2] << endl;
    cout << "model dir: " << args[3] << endl;
    
    stache = imread(args[1], 1);
    mask = imread(args[2], 1);
	
    if(stache.empty()) 
    {
        cout << "couldn't find " << args[1] << endl;
        return -1;
    }
    
    if(mask.empty()) 
    {
        cout << "couldn't find " << args[2] << endl;
        return -1;
    }
    
	string ftFile = args[3]+"/face2.tracker";
	string triFile = args[3]+"/face.tri";
	string conFile = args[3]+"/face.con";
    fcheck = true;  // check for whether the tracking failed
    scale = 1; 
    fpd = -1;       // how often to skip frames
    show = false;
    
    //set other tracking parameters
    wSize1.resize(1); 
    wSize1[0] = 7;
    wSize2.resize(3); 
    wSize2[0] = 11; 
    wSize2[1] = 9; 
    wSize2[2] = 7;
    
    nIter = 20;  // [1-25] 1 is fast and inaccurate, 25 is slow and accurate
    clamp=1;    // [0-4] 1 gives a very loose fit, 4 gives a very tight fit
    //fTol=0.01;  // [.01-1] match tolerance
    fTol=0.5;
    model.Load(ftFile.c_str());
    tri=IO::LoadTri(triFile.c_str());
    con=IO::LoadCon(conFile.c_str());
    

    failed = true;
    
    
    /*
    fps=0;
    t1,t0 = cvGetTickCount(); 
    fnum=0;
    */
    
	return 0;
}

// ------------------------------
void Moustachizer::process(Mat frame) {
	
    /*
	//circle(frame, Point(300,300), 300, Scalar(255,0,0), 3);
	Mat grayFrame = frame.clone();
	cvtColor(frame, grayFrame, CV_RGB2GRAY);
	equalizeHist(grayFrame, grayFrame);
	imshow("grayFrame", grayFrame);
    */
	
    
    if(scale == 1)
        im = frame; 
    else 
        cv::resize(frame,im,cv::Size(scale*frame.cols, scale*frame.rows));
    
    //cv::flip(im, im, 1); 
    cv::cvtColor(im, gray, CV_BGR2GRAY);
    

    //track this image
    std::vector<int> wSize; 
    if(failed)
        wSize = wSize2; 
    else 
        wSize = wSize1; 
    
    if(model.Track(gray,wSize,fpd,nIter,clamp,fTol,fcheck) == 0)
    {
       // int idx = model._clm.GetViewIdx(); 
        failed = false;
        //Draw(im, model._shape,con,tri,model._clm._visi[idx]);
        
        
        int n = model._shape.rows/2;
        Point p1 = Point(model._shape.at<double>(48,0), model._shape.at<double>(48+n, 0));
        Point p2 = Point(model._shape.at<double>(54,0), model._shape.at<double>(54+n, 0));
        //cv::line(im,p1,p2,CV_RGB(0,255,0),1);
        float width = p2.x - p1.x;
        
       
        double angle = atan2(p2.y-p1.y, p2.x-p1.x) * 180 / 3.14159;
     
        //cout << "rotating " << angle << endl;
        float scale =  width / stache.size().width / .75;
		

        Mat stache_rotated = rotateImage(stache, 360-angle);
        Mat mask_rotated = rotateImage(mask, 360-angle);
        
		Mat stache_resized;
		Mat mask_resized;
		resize(stache_rotated, stache_resized, Size(), scale, scale);
		resize(mask_rotated, mask_resized, Size(), scale, scale);
		
		Rect pos = Rect(p1.x, p1.y-stache_resized.size().height, stache_resized.size().width, stache_resized.size().height);
		
		Mat bg = frame(pos);
		stache_resized.copyTo(bg, mask_resized);
        
    }
    else
    {
        if(show)
        {
            cv::Mat R(im, cvRect(0,0,150,50)); 
            R = cv::Scalar(0,0,255);
        }
        model.FrameReset(); 
        failed = true;
    } 

    //show image and check for user input
    //imshow("Face Tracker",im); 
    

	//cvtColor(input, input, CV_GRAY2RGB);
	//imshow("preview", frame);
	
	cvWaitKey(1);
}


Mat Moustachizer::rotateImage(const Mat& source, double angle)
{
    Point2f src_center(source.cols/2.0F, source.rows/2.0F);
    Mat rot_mat = getRotationMatrix2D(src_center, angle, 1.0);
    Mat dst;
    warpAffine(source, dst, rot_mat, source.size());
    return dst;
}

void Moustachizer::Draw(cv::Mat &image,cv::Mat &shape,cv::Mat &con,cv::Mat &tri,cv::Mat &visi)
{
    int i,n = shape.rows/2; cv::Point p1,p2; cv::Scalar c;
    
    //draw triangulation
    c = CV_RGB(0,0,0);
    for(i = 0; i < tri.rows; i++)
    {
        if(visi.at<int>(tri.at<int>(i,0),0) == 0 ||
           visi.at<int>(tri.at<int>(i,1),0) == 0 ||
           visi.at<int>(tri.at<int>(i,2),0) == 0)continue;
        p1 = cv::Point(shape.at<double>(tri.at<int>(i,0),0),
                       shape.at<double>(tri.at<int>(i,0)+n,0));
        p2 = cv::Point(shape.at<double>(tri.at<int>(i,1),0),
                       shape.at<double>(tri.at<int>(i,1)+n,0));
        cv::line(image,p1,p2,c);
        p1 = cv::Point(shape.at<double>(tri.at<int>(i,0),0),
                       shape.at<double>(tri.at<int>(i,0)+n,0));
        p2 = cv::Point(shape.at<double>(tri.at<int>(i,2),0),
                       shape.at<double>(tri.at<int>(i,2)+n,0));
        cv::line(image,p1,p2,c);
        p1 = cv::Point(shape.at<double>(tri.at<int>(i,2),0),
                       shape.at<double>(tri.at<int>(i,2)+n,0));
        p2 = cv::Point(shape.at<double>(tri.at<int>(i,1),0),
                       shape.at<double>(tri.at<int>(i,1)+n,0));
        cv::line(image,p1,p2,c);
    }
    
    
    //draw connections
    c = CV_RGB(0,0,255);
    for(i = 0; i < con.cols; i++)
    {
        if(visi.at<int>(con.at<int>(0,i),0) == 0 ||
           visi.at<int>(con.at<int>(1,i),0) == 0)continue;
        p1 = cv::Point(shape.at<double>(con.at<int>(0,i),0),
                       shape.at<double>(con.at<int>(0,i)+n,0));
        p2 = cv::Point(shape.at<double>(con.at<int>(1,i),0),
                       shape.at<double>(con.at<int>(1,i)+n,0));
        cv::line(image,p1,p2,c,1);
    }
    
   
    
    //draw points
    for(i = 0; i < n; i++)
    {    
        if(visi.at<int>(i,0) == 0)continue;
        p1 = cv::Point(shape.at<double>(i,0),shape.at<double>(i+n,0));
        c = CV_RGB(255,0,0); 
        cv::circle(image,p1,2,c);
    }

    
    return;
}
