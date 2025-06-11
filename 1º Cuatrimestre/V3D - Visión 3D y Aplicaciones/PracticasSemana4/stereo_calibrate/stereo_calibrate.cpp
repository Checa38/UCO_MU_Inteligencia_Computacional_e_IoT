#include <opencv2/opencv.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include "dirreader.h"

cv::Size CheckerBoardSize = {7, 5};
double SquareSize = 0.02875;

int main(int argc, char *argv[]) 
{
    if (argc < 3) 
    {
        std::cerr << "Uso: ./stereo_calibrate dir_with_images out.yml" << std::endl;
        return -1;
    }

    std::string imageDir = argv[1];
    std::string outputFile = argv[2];

    // Vector para almacenar puntos 3D y 2D para las dos cámaras
    std::vector<std::vector<cv::Point3f>> objpoints;
    std::vector<std::vector<cv::Point2f>> imgpointsLeft, imgpointsRight;

    // Definir coordenadas del mundo para los puntos 3D
    std::vector<cv::Point3f> objp;
    for (int i = 0; i < CheckerBoardSize.height; i++) 
    {
        for (int j = 0; j < CheckerBoardSize.width; j++) 
        {
            objp.push_back(cv::Point3f(j * SquareSize, i * SquareSize, 0));
        }
    }

    // Leer imágenes usando DirReader
    DirReader Dir;
    auto files = Dir.read(imageDir, ".jpg", DirReader::Params(true));

    for (const auto &file : files) 
    {
        cv::Mat img = cv::imread(file, cv::IMREAD_GRAYSCALE);
        if (img.empty()) 
        {
            std::cerr << "No se pudo cargar la imagen: " << file << std::endl;
            continue;
        }

        // Dividir la imagen en dos vistas 
        cv::Mat leftImg = img(cv::Range::all(), cv::Range(0, img.cols / 2));
        cv::Mat rightImg = img(cv::Range::all(), cv::Range(img.cols / 2, img.cols));

        // Esquinas
        std::vector<cv::Point2f> cornersLeft, cornersRight;
        bool foundLeft = cv::findChessboardCorners(leftImg, CheckerBoardSize, cornersLeft,
                                                   cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE);
        bool foundRight = cv::findChessboardCorners(rightImg, CheckerBoardSize, cornersRight,
                                                    cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE);

        if (foundLeft && foundRight) 
        {
        
            cv::cornerSubPix(leftImg, cornersLeft, cv::Size(11, 11), cv::Size(-1, -1),
                             cv::TermCriteria(cv::TermCriteria::EPS | cv::TermCriteria::MAX_ITER, 60, 1e-6));
            cv::cornerSubPix(rightImg, cornersRight, cv::Size(11, 11), cv::Size(-1, -1),
                             cv::TermCriteria(cv::TermCriteria::EPS | cv::TermCriteria::MAX_ITER, 60, 1e-6));

            objpoints.push_back(objp);
            imgpointsLeft.push_back(cornersLeft);
            imgpointsRight.push_back(cornersRight);
        }
    }

    // Calibración estéreo
    cv::Mat cameraMatrixLeft, distCoeffsLeft, cameraMatrixRight, distCoeffsRight;
    cv::Mat R, T, E, F;

    cv::stereoCalibrate(objpoints, imgpointsLeft, imgpointsRight,
                        cameraMatrixLeft, distCoeffsLeft,
                        cameraMatrixRight, distCoeffsRight,
                        cv::Size(imgpointsLeft[0][0].x, imgpointsLeft[0][0].y),
                        R, T, E, F, 0,
                        cv::TermCriteria(cv::TermCriteria::MAX_ITER + cv::TermCriteria::EPS, 60, 1e-6));

    // Guardar parámetros
    cv::FileStorage fs(outputFile, cv::FileStorage::WRITE);
    fs << "LEFT_K" << cameraMatrixLeft;
    fs << "LEFT_D" << distCoeffsLeft;
    fs << "RIGHT_K" << cameraMatrixRight;
    fs << "RIGHT_D" << distCoeffsRight;
    fs << "R" << R;
    fs << "T" << T;
    fs << "E" << E;
    fs << "F" << F;
    fs.release();

    std::cout << "Calibración estéreo completada. Parámetros guardados en " << outputFile << std::endl;
    return 0;
}
