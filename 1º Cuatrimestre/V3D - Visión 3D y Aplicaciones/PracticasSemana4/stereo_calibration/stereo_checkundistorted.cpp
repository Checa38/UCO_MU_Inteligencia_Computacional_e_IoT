#include <opencv2/opencv.hpp>
#include <iostream>

struct StereoParams {
    cv::Mat mtxL, distL, mtxR, distR;
    cv::Mat Rot, Trns, Emat, Fmat;
};

void rectifyStereoImages(const StereoParams &sti, cv::Mat &left, cv::Mat &right) {
    cv::Mat rect_l, rect_r, proj_mat_l, proj_mat_r, Q;
    cv::Mat Left_Stereo_Map1, Left_Stereo_Map2;
    cv::Mat Right_Stereo_Map1, Right_Stereo_Map2;
		
    cv::stereoRectify(sti.mtxL, sti.distL, sti.mtxR, sti.distR, left.size(),
                      sti.Rot, sti.Trns, rect_l, rect_r, proj_mat_l, proj_mat_r,
                      Q, cv::CALIB_ZERO_DISPARITY, 0);

    cv::initUndistortRectifyMap(sti.mtxL, sti.distL, rect_l, proj_mat_l,
                                left.size(), CV_16SC2, Left_Stereo_Map1, Left_Stereo_Map2);
    cv::initUndistortRectifyMap(sti.mtxR, sti.distR, rect_r, proj_mat_r,
                                left.size(), CV_16SC2, Right_Stereo_Map1, Right_Stereo_Map2);


    cv::remap(left, left, Left_Stereo_Map1, Left_Stereo_Map2, cv::INTER_LANCZOS4);
    cv::remap(right, right, Right_Stereo_Map1, Right_Stereo_Map2, cv::INTER_LANCZOS4);

}

void onMouse(int event, int x, int y, int flags, void* userdata) {
    if (event == cv::EVENT_MOUSEMOVE) {
        cv::Mat* img = reinterpret_cast<cv::Mat*>(userdata);
        cv::Mat display;
        img->copyTo(display);
        cv::line(display, cv::Point(0, y), cv::Point(display.cols, y), cv::Scalar(0, 255, 0), 1);
        cv::imshow("Org", display);
    }
}

void onMouseRectified(int event, int x, int y, int flags, void* userdata) {
    if (event == cv::EVENT_MOUSEMOVE) {
        cv::Mat* img = reinterpret_cast<cv::Mat*>(userdata);
        cv::Mat display;
        img->copyTo(display);
        cv::line(display, cv::Point(0, y), cv::Point(display.cols, y), cv::Scalar(0, 255, 0), 1);
        cv::imshow("Rectified", display);
    }
}

int main(int argc, char** argv) 
{
    if (argc != 3) 
    {
        std::cerr << "Usage: ./stereo_checkundistorted stereo_image.jpg stereocalibrationfile.yml" << std::endl;
        return -1;
    }

    cv::Mat stereo_image = cv::imread(argv[1]);
    if (stereo_image.empty()) 
    {
        std::cerr << "Error: Cannot load the stereo image." << std::endl;
        return -1;
    }
    
    cv::Mat left = stereo_image(cv::Rect(0, 0, stereo_image.cols / 2, stereo_image.rows));
    cv::Mat right = stereo_image(cv::Rect(stereo_image.cols / 2, 0, stereo_image.cols / 2, stereo_image.rows));

    // Cargar parametros de calibracion
    cv::FileStorage fs(argv[2], cv::FileStorage::READ);
    if (!fs.isOpened()) 
    {
        std::cerr << "Error: No se ha podido abrir el fichero de calibracion." << std::endl;
        return -1;
    }

    StereoParams params;
    fs["LEFT_K"] >> params.mtxL;
    fs["LEFT_D"] >> params.distL;
    fs["RIGHT_K"] >> params.mtxR;
    fs["RIGHT_D"] >> params.distR;
    fs["R"] >> params.Rot;
    fs["T"] >> params.Trns;
    
    // Imagenes a rectificar
    cv::Mat left_rect = left.clone();
    cv::Mat right_rect = right.clone();

    rectifyStereoImages(params, left_rect, right_rect);
    
    cv::Mat combined, combined_rect;
    cv::hconcat(left, right, combined);
    cv::hconcat(left_rect, right_rect, combined_rect);

    cv::namedWindow("Org", cv::WINDOW_AUTOSIZE);
    cv::setMouseCallback("Org", onMouse, &combined);
    cv::imshow("Org", combined);

    cv::namedWindow("Rectified", cv::WINDOW_AUTOSIZE);
    cv::setMouseCallback("Rectified", onMouseRectified, &combined_rect);
    cv::imshow("Rectified", combined_rect);

    cv::waitKey(0);
    return 0;
}

