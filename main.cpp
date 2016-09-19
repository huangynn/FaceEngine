//
//  main.cpp
//  Face_Recognition
//
//  Created by 黄缨宁 on 9/18/16.
//  Copyright © 2016 黄缨宁. All rights reserved.
//

#include <iostream>
#include "FaceDetection/face_detection.h"
#include "FaceAlignment/face_alignment.h"
#include "FaceIdentification/face_identification.h"


#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/video/background_segm.hpp>

#include <string.h>

#include <glog/logging.h>

int find_connected_componet(cv::Mat& src, cv::Mat& dst, std::vector<cv::Rect>& objects, int min_size, int max_size)
{
    objects.clear();
    assert(src.channels() == 1);
    std::vector<std::vector<cv::Point>> contours;
    cv::Mat operate_img;
    
    //去噪
    cv::erode(src, operate_img, cv::Mat(), cv::Point(-1,-1), 1);
    cv::dilate(operate_img, operate_img, cv::Mat(), cv::Point(-1,-1), 1);
    
    
    findContours(operate_img, contours,
                 CV_RETR_EXTERNAL, //只取最外侧轮廓
                 CV_CHAIN_APPROX_NONE);
    size_t c_size = contours.size();
    
    dst = cv::Mat::zeros(src.size(), CV_8UC1);
    
    for (size_t i = 0; i < c_size; i++)
    {
        cv::Rect cur = boundingRect( cv::Mat(contours[i]));
        if (cur.width <= max_size + 20 &&  cur.height <= max_size +20 && (cur.height >= min_size -20 || cur.width >= min_size -20 ))
        {
            double ratio = static_cast<float>(cur.width)/static_cast<float>(cur.height);
            if (ratio <= 10 && ratio >= 0.2)
            {
                objects.push_back(cur);
                cv::drawContours(dst, contours, static_cast<int>(i), cv::Scalar(255,0,0), CV_FILLED); //填充孔洞
            }
        }
    }
    return objects.size();
}


int main(int argc, const char * argv[]) {
   
    if(argc != 5)
        LOG(FATAL) << "./bin video_file_name result_video_path_to_store min_size max_size";
        
    std::string detector_model_path = "/Users/huangyingning/hyn/projects/seeta_face_test/FaceDetection/model/seeta_fd_frontal_v1.0.bin";
    std::string aligner_model_path = "/Users/huangyingning/hyn/projects/seeta_face_test/FaceAlignment/model/seeta_fa_v1.0.bin";
    std::string identifier_model_path = "/Users/huangyingning/hyn/projects/seeta_face_test/FaceIdentification/model/seeta_fr_v1.0.bin";
    
    std::string video_file_name = argv[1];
    std::string video_dst_path = argv[2];
    
    seeta::FaceDetection face_detector(detector_model_path.c_str());
    seeta::FaceAlignment face_aligner(aligner_model_path.c_str());
    seeta::FaceIdentification face_identifier(identifier_model_path.c_str());
    
    cv::VideoCapture cap(video_file_name);
    
    int min_size = std::atoi(argv[3]), max_size = std::atoi(argv[4]);
    face_detector.SetMinFaceSize(min_size);
    face_detector.SetMaxFaceSize(max_size);
    face_detector.SetScoreThresh(2.f);
    face_detector.SetImagePyramidScaleFactor(0.8f);
    face_detector.SetWindowStep(4, 4);

    cv::Size analyze_size = cv::Size(640, 360);
    cv::VideoWriter writer(video_dst_path, CV_FOURCC('F', 'L', 'V', '1'), cap.get(CV_CAP_PROP_FPS), analyze_size);
    
    cv::Mat cur_image;
    
    cv::Ptr<cv::BackgroundSubtractorMOG2> bg_subtractor = cv::createBackgroundSubtractorMOG2();
    
    int frame_count = 0;
    while(cap.read(cur_image))
    {
        if(cur_image.size() != analyze_size)
            cv::resize(cur_image, cur_image, analyze_size);
        
        cv::Mat img_gray;
        if (cur_image.channels() != 1)
            cv::cvtColor(cur_image, img_gray, cv::COLOR_BGR2GRAY);
        else
            img_gray = cur_image;
        
        cv::Mat foreground;
        seeta::ImageData img_data;
        std::vector<seeta::FaceInfo> faces;
        
        bg_subtractor->apply(img_gray, foreground);
        
        cv::Mat masked_img = foreground;
        
        std::vector<cv::Rect> objects;
        int ans = find_connected_componet(foreground, foreground, objects, min_size, max_size);
        if(ans <=0 )
            goto _show;
        
        
        img_gray.copyTo(masked_img);

        
        img_data.data = masked_img.data;
        img_data.width = masked_img.cols;
        img_data.height = masked_img.rows;
        img_data.num_channels = 1;

        faces = face_detector.Detect(img_data);
        for(auto item:faces)
        {
            cv::Rect face_rect;
            face_rect.x = item.bbox.x;
            face_rect.y = item.bbox.y;
            face_rect.width = item.bbox.width;
            face_rect.height = item.bbox.height;
            cv::rectangle(cur_image, face_rect, cv::Scalar(255, 0, 0));
            cv::putText(cur_image, std::to_string(item.score), face_rect.br(), 0, 1.0, cv::Scalar(255, 0, 0));
        }
    _show:
        
        cv::rectangle(cur_image, cv::Rect(0, 0, min_size, min_size), cv::Scalar(255, 255 , 255), 2);
        cv::rectangle(cur_image, cv::Rect(0, 0, max_size, max_size), cv::Scalar(255, 255 , 255), 2);
        cv::imshow("Seeta", cur_image);
        cv::imshow("masked", masked_img);
        cv::waitKey(1);
        frame_count ++;
        LOG_EVERY_N(INFO, 50) << "frame count: " << frame_count;
        writer << cur_image;
    }
    
}
