/**
 * @file augReal.cpp
 * @author Carlos Checa Moreno (i02chmoc@uco.es)
 * @brief Semana 3. Practica 3.1. V3D. Master Universitario en Inteligencia Computacional e Internet de las Cosas
 * @date 19-12-2024
 *
 */
#include <iostream>
#include <exception>

#include <opencv2/core/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>


const cv::String keys =
    "{help h usage ? |      | print this message   }"
    "{rows r         | 6 | filas tablero.}"
    "{cols c         | 9 | columnas tablero.}"
    "{size s         | 1 | size cuadrado tablero.}"
    "{@axis a         | 3 | size de los ejes.}"
    "{@intrinsics    |<none>| fichero de intrinsics.}"
    "{@input         |<none>| input stream (filename or camera idx)}"
    ;

int main (int argc, char* const* argv)
{
    int retCode=EXIT_SUCCESS;
    try
    {
        cv::CommandLineParser parser(argc, argv, keys);
        if (parser.has("help"))
        {
            parser.printMessage();
            return EXIT_SUCCESS;
        }
        int escala_ejes = 5;
        
        cv::String pathToVideo = parser.get<cv::String>("@input");
      
        int rows = parser.get<int>("rows");
        int cols = parser.get<int>("cols");
        int square_size = parser.get<int>("size");
        int axis_size = parser.get<int>("@axis");
        std::cout << "Filas: "<< rows << std::endl;
        std::cout << "Columnas: "<< cols << std::endl;
        std::cout << "Tamaño cuadrado: "<< square_size << std::endl;
        std::cout << "Tamaño ejes: "<< axis_size << std::endl;
        cv::Size board_size(cols-1, rows-1);
        cv::VideoCapture video(pathToVideo);

        float wait_time = 10;
        /*
        * Leer parametros de calibracion
        */
        std::string intrinsics_file = parser.get<std::string>("@intrinsics"); 
        cv::FileStorage fs(intrinsics_file, cv::FileStorage::READ);
        cv::Mat camera_matrix, distCoeffs, rvec, tvec;
        fs["cameraMatrix"] >> camera_matrix;
        fs["distCoeffs"] >> distCoeffs;
        /*
        *
        */

        cv::Mat image;
        std::vector<cv::Point2f> _2dpoints;
        
        while(video.grab())
        {
            video.retrieve(image);
            cv::imshow("VIDEO",image);

                /*
                *   PRIMERO: Detect the board using cv::findChessboardCorners, and refine the corners with cv::cornerSubPix.
                */
                bool was_found = cv::findChessboardCorners(image, board_size, _2dpoints);

                
                if(was_found)
                {
                    //std::cout << "- Esquinas encontradas. \n" << std::endl;
                    cv::Mat grey = image.clone();
                    cv::cvtColor(grey, grey, cv::COLOR_BGR2GRAY);
                    cv::cornerSubPix(grey, _2dpoints, cv::Size(5, 5), cv::Size(-1, -1), cv::TermCriteria());

                    /*
                    *   SEGUNDO: Estimate the camera pose with respect to the board using cv::solvePnP. 
                    */
                    std::vector<cv::Point3f> _3dpoints;
                    
                    for(int i = 1; i <= board_size.height; i++)
                    {
                            for(int j = 1; j <= board_size.width; j++)
                            {
                                _3dpoints.push_back(cv::Point3f(j*square_size, i*square_size, 0));
                            }
                    }

                    cv::solvePnP(_3dpoints, _2dpoints, camera_matrix, distCoeffs, rvec, tvec);
                    //std::cout << "- solverPNP correcto. \n" << std::endl;

                    /*
                    *   TERCERO: Drawing on the image is a simple 3D scene. The 3D axis is placed in the centre of the reference system in colours red (X-axis), 
                    *            green (Y-axis), and blue (Z-axis). The size of the axis should be the same as the board squares. 
                    *            You can use the function cv::projectPoints and cv::line for that.
                    */

                    int line_width = 3;
                    //std::cout << "- Dibujando sobre la imagen...\n" << std::endl;
                    //cv::drawChessboardCorners(image, board_size, _2dpoints, was_found);

                    std::vector<cv::Point3f> points3d;
                    std::vector<cv::Point2f> points2d;

                    cv::Point3f p0(0, 0, 0);
                    cv::Point3f p1(axis_size, 0, 0);
                    cv::Point3f p2(0, axis_size, 0);
                    cv::Point3f p3(0, 0, -axis_size);

                    points3d.push_back(p0);
                    points3d.push_back(p1);
                    points3d.push_back(p2);
                    points3d.push_back(p3);

                    cv::projectPoints(points3d, rvec, tvec, camera_matrix, distCoeffs, points2d);

                    cv::line(image, points2d[0], points2d[1], cv::Scalar(0, 0, 255), line_width);
                    cv::line(image, points2d[0], points2d[2], cv::Scalar(0, 255, 0), line_width);
                    cv::line(image, points2d[0], points2d[3], cv::Scalar(255, 0, 0), line_width);
                  

                    //std::cout << "- Dibujo correcto.\n" << std::endl;

                    /*
                    *   OPTIONAL REQUIREMENTS (40%)
                    *   
                    *   Dibujar cubos 1x1x1 solo en los cuadros negros del tablero de calibración.
                    */

                    std::vector<cv::Point3f> cube_points3D = {
                        {0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0},  // Base del cubo
                        {0, 0, -1}, {1, 0, -1}, {1, 1, -1}, {0, 1, -1} // Parte superior del cubo
                    };

                    for (int i = 0; i <= board_size.height; i++) 
                    {
                        for (int j = 0; j <= board_size.width; j++) 
                        {
                            // Cuadrados negros
                            if ((i + j) % 2 == 0) 
                            {
                                std::vector<cv::Point3f> cube3D;
                                for (auto &p : cube_points3D) 
                                {
                                    cube3D.push_back(cv::Point3f(
                                        p.x + j * square_size, 
                                        p.y + i * square_size, 
                                        p.z
                                    ));
                                }

                                std::vector<cv::Point2f> cube2D;
                                cv::projectPoints(cube3D, rvec, tvec, camera_matrix, distCoeffs, cube2D);

                                // Cubos
                                int edges[12][2] = {{0, 1}, {1, 2}, {2, 3}, {3, 0},  // Base
                                                    {4, 5}, {5, 6}, {6, 7}, {7, 4},  // Top
                                                    {0, 4}, {1, 5}, {2, 6}, {3, 7}}; // Laterales

                                for (auto &edge : edges) 
                                {
                                    cv::line(image, cube2D[edge[0]], cube2D[edge[1]], cv::Scalar(255, 0, 0), 1);
                                }
                            }
                        }
                    }


                }
                else
                {
                    std::cout << "Esquinas no encontradas.\n" << std::endl;
                } 

            cv::imshow("VIDEO", image);
            cv::waitKey(wait_time) & 0xff;

        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Capturada excepcion: " << e.what() << std::endl;
        retCode = EXIT_FAILURE;
    }
    
    return retCode;

}

