#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <fstream>

struct StereoParams {
    cv::Mat mtxL, distL, R_L, T_L;
    cv::Mat mtxR, distR, R_R, T_R;
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

void writeToOBJ(std::string path,std::vector<cv::Point3f> points){

   std::ofstream file(path,std::ios::binary);
   for(auto p:points)
       file<<"v "<<p.x<<" "<<p.y<<" "<<p.z<<std::endl;
}


int main(int argc, char *argv[]) 
{
    if (argc != 4) 
    {
        std::cerr << "Uso: stereo_disparity stereo_image.jpg calibration.yml out.obj" << std::endl;
        return -1;
    }

    // Leer parámetros de calibración
    StereoParams params;
    cv::FileStorage fs(argv[2], cv::FileStorage::READ);
    if (!fs.isOpened()) 
    {
        std::cerr << "Error: Could not open the calibration file." << std::endl;
        return -1;
    }

    fs["LEFT_K"] >> params.mtxL;
    fs["LEFT_D"] >> params.distL;
    fs["RIGHT_K"] >> params.mtxR;
    fs["RIGHT_D"] >> params.distR;
    fs["R"] >> params.Rot;
    fs["T"] >> params.Trns;
    fs["E"] >> params.Emat;
    fs["F"] >> params.Fmat;

    // 1) Cargar la imágen estéreo
    cv::Mat stereoImage = cv::imread(argv[1]);

    if (stereoImage.empty()) 
    {
        std::cerr << "Error: Could not read the stereo image." << std::endl;
        return -1;
    }

    // 2) Rectificar las imágenes
    cv::Mat left = stereoImage.colRange(0, stereoImage.cols / 2);
    cv::Mat right = stereoImage.colRange(stereoImage.cols / 2, stereoImage.cols);

    cv::Mat leftRectified = left.clone();
    cv::Mat rightRectified = right.clone();

    rectifyStereoImages(params, leftRectified, rightRectified);

    cv::cvtColor(leftRectified, leftRectified, cv::COLOR_BGR2GRAY);
    cv::cvtColor(rightRectified, rightRectified, cv::COLOR_BGR2GRAY);


    cv::Mat R1, R2, P1, P2, Q;
    cv::stereoRectify(params.mtxL, params.distL, params.mtxR, params.distR, leftRectified.size(), params.Rot, params.Trns, R1, R2, P1, P2, Q);

    
    cv::Mat rectifiedImage;
    cv::hconcat(left, right, rectifiedImage); 

    int disparidades = 16 * 4;
    int blockSize = 15;

    cv::Mat disparidad;
    // 3) Utilizar la clase cv::StereoBM para realizar el cálculo de la disparidad.
    cv::Ptr<cv::StereoBM> stereo = cv::StereoBM::create(disparidades, blockSize);
    stereo->compute(leftRectified, rightRectified, disparidad);

    // 4) Convierta la disparidad obtenida a valores 32bits flotantes
    // Converting disparity values to CV_32F from CV_16S
    disparidad.convertTo(disparidad,CV_32F, 1.0);
    disparidad=disparidad/16.f;

    float cx = params.mtxL.at<double>(0, 2); 
    float cy = params.mtxL.at<double>(1, 2); 
    float leftFLX = params.mtxL.at<double>(0, 0); 
    float leftFLY = params.mtxL.at<double>(1, 1); 
    float f = (leftFLX + leftFLY) / 2.0;

    double T = cv::norm(params.Trns, cv::NORM_L1);

    std::vector<cv::Point3f> points;
    for (int y = 0; y < disparidad.rows; y++) 
    {
        for (int x = 0; x < disparidad.cols; x++) 
        {
            float d = disparidad.at<float>(y, x);
            // 5) Para aquellos puntos con disparidad > 10, triangule usado las ecuaciones básica del par estéreo: Z= |T|*f /d; X= (x-cx)*Z/f ; Y= (y-cy)*Z/f;
            if (d > 10) 
            {
                float Z = T * f / d;
                float X = (x - cx) * Z / f;
                float Y = (y - cy) * Z / f;
                points.push_back(cv::Point3f(X, Y, Z));
            }
        }
    }
    
    // 6) Guarde los puntos a formato obj. Puede utilizar la siguiente función para ello:
    writeToOBJ(argv[3], points);


    return 0;
}
