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
        std::cerr << "Uso: stereo_sparse stereo_image.jpg calibration.yml out.obj" << std::endl;
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

    // 3) Busca keypoints en ambas imágenes usando AKAZE y el descriptor matcher BruteForce-Hamming
    std::vector<cv::KeyPoint> keypoints_query, keypoints_train;
    cv::Mat descriptors_query, descriptors_train;
    std::vector<cv::DMatch> matches;

    auto Detector = cv::AKAZE::create(cv::AKAZE::DESCRIPTOR_MLDB, 0, 3, 1e-4f, 8);
    Detector->detectAndCompute(leftRectified, cv::Mat(), keypoints_query, descriptors_query);
    Detector->detectAndCompute(rightRectified, cv::Mat(), keypoints_train, descriptors_train);
    auto matcher = cv::DescriptorMatcher::create("BruteForce-Hamming");
    matcher->match(descriptors_query, descriptors_train, matches, cv::Mat());

    // 4) Filtra los matches y deja aquellos que están en líneas horizontales. 
    std::vector<cv::DMatch> filtered_matches;
    for (const auto &match : matches) 
    {
        if (std::abs(keypoints_query[match.queryIdx].pt.y - keypoints_train[match.trainIdx].pt.y) < 10.0) 
        {
            filtered_matches.push_back(match);
        }
    }

    // 5) Dibuja los matches obtenidos antes y después del filtrado
    cv::Mat matchesBf, matchesAf;
    cv::drawMatches(leftRectified, keypoints_query, rightRectified, keypoints_train, matches, matchesBf);
    cv::drawMatches(leftRectified, keypoints_query, rightRectified, keypoints_train, filtered_matches, matchesAf);
    
    // 6) Triangula los matches y los guarda en el fichero de salida con el formato OBJ. 
    std::vector<cv::Point3f> points;
    for (const auto &match : filtered_matches) 
    {
        float d = match.distance;
        float Z = params.Trns.at<double>(0, 0) * params.mtxL.at<double>(0, 0) / d;
        float X = (keypoints_query[match.queryIdx].pt.x - params.mtxL.at<double>(0, 2)) 
                    * Z / params.mtxL.at<double>(0, 0);
        float Y = (keypoints_query[match.queryIdx].pt.y - params.mtxL.at<double>(1, 2)) 
                    * Z / params.mtxL.at<double>(1, 1);
        points.push_back(cv::Point3f(X, Y, Z));
    }

    writeToOBJ(argv[3], points);

    cv::imshow("Matches prefiltrado", matchesBf);
    cv::imshow("Matches postfiltrado", matchesAf);
    cv::waitKey(0);
    return 0;
}
