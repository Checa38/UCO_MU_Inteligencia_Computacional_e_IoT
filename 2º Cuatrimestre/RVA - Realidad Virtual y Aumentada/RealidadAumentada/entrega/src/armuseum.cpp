// This program reads a model image, a video and a patch image, and overlays the patch on the model object in the video frames.

// (c) Realidad Virtual y Aumentada - Universidad de Cordoba - Manuel J. Marin-Jimenez

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/calib3d.hpp>

#include "rva.h"

using namespace std;

const cv::String keys =
"{help h usage ? |      | print this message   }"
"{@model         |<none>| Path to image model.}"
"{@scene         |<none>| Path to image scene.}"
"{patch         |<none>| Path to image patch.}"
"{video         |<none>| Path to playable video.}"

;

// Main function
int main(int argc, char ** argv) {

    // Get the arguments: model, video and patch using OpenCv parser
    cv::CommandLineParser parser(argc, argv, keys);
    if (parser.has("help")) {
        parser.printMessage();
        return 0;
    }
    string model_path = parser.get<string>(0);
    string scene_path = parser.get<string>(1);
    
    // Patch argument is available?
    string patch_path = parser.get<string>("patch");

    // Second video argument is available?
    string video_path = parser.get<string>("video");

    // Video2 has priority over patch
    bool use_video = !video_path.empty();
    bool use_patch = !patch_path.empty() && !use_video;
    bool use_info_text = !use_video && !use_patch;
    
    // *** TODO: define tu texto aqui ***
    std::vector<cv::String> info_text;
    info_text.push_back("Titulo: Escuela de Atenas");
    info_text.push_back("Autor: Rafael Sanzio");
    info_text.push_back("Tecnica: Pintura al fresco");
    info_text.push_back("Estilo: Renacimiento");

    // Load the images in color
    cv::Mat img_model = cv::imread(model_path, cv::IMREAD_COLOR);

    cv::Mat img_patch;
    if (use_patch)
        img_patch = cv::imread(patch_path, cv::IMREAD_COLOR);

    // Check if the images are loaded
    // *** TODO: COMPLETAR ***
    if (img_model.empty()) {
        cout << "Error: image not loaded." << endl;
        return -1;
    }
    if (use_patch && img_patch.empty()) {
        cout << "Error: Patch image not loaded." << endl;
        return -1;
    }

    // Change resolution of the image model to half
    cv::resize(img_model, img_model, cv::Size(), 0.5, 0.5);

    // Resize the patch to the size of the model
    if (use_patch)
        cv::resize(img_patch, img_patch, img_model.size());

    // Load the scene image
    // *** TODO : COMPLETAR ***
    cv::Mat img_scene = cv::imread(scene_path, cv::IMREAD_COLOR);
    if (img_scene.empty()) {
        cout << "Error: Scene image not loaded." << endl;
        return -1;
    }

    // If use video, create the video capture
    cv::VideoCapture cap2;
    if (use_video) {
        // Check if video_path is a the webcam index and open it
        // *** TODO: COMPLETAR ***
        if (isdigit(video_path[0]) && video_path.length() == 1) {
            cap2.open(stoi(video_path));
        } else {
            // Open video file to display
            // *** TODO: COMPLETAR ***
            cap2.open(video_path);
        }

        if (!cap2.isOpened()) {
            cout << "Error: video not loaded: " + video_path << endl;
            return -1;
        }
    }

    /*cv::imshow("Modelo", img_model);
    cv::imshow("Escena", img_scene);
    cv::waitKey(0);*/

    // Pre-compute keypoints and descriptors for the model image
    std::vector<cv::KeyPoint> keypoints_model;
    cv::Mat descriptors_model;
    cv::Mat img_gray_model;
    cv::cvtColor(img_model, img_gray_model, cv::COLOR_BGR2GRAY);
    rva_calculaKPsDesc(img_gray_model, keypoints_model, descriptors_model);
    printf ("Hay %li keypoints en el modelo\n", keypoints_model.size());
    // For the scene image, detect the object and overlay the patch    

    // To speed up processing, resize the image to half
    cv::resize(img_scene, img_scene, cv::Size(), 0.5, 0.5);

    // Compute keypoints and descriptors for the scene image
    std::vector<cv::KeyPoint> keypoints_scene;
    cv::Mat descriptors_scene;
    cv::Mat img_gray_scene;
    cv::cvtColor(img_scene, img_gray_scene, cv::COLOR_BGR2GRAY);
    rva_calculaKPsDesc(img_gray_scene, keypoints_scene, descriptors_scene);
    printf ("Hay %li keypoints en la escena\n", keypoints_scene.size());
    // Match the descriptors
    std::vector<cv::DMatch> matches;
    rva_matchDesc(descriptors_model, descriptors_scene, matches);
	printf("Hay %li matches, ", matches.size());
    // Filter out outliers
    // *** TODO: COMPLETAR ***
    std::vector<cv::DMatch> good_matches;
    rva_filterMatches(matches, keypoints_model, keypoints_scene, good_matches);

    // Compute the bounding-box of the model in the scene
    cv::Mat H;
    std::vector<cv::Point2f> pts_obj_in_scene;
    rva_localizaObj(img_gray_model, img_gray_scene, keypoints_model, keypoints_scene, good_matches, H, pts_obj_in_scene);
    // Draw the bounding-box 
	if (use_info_text)	
		rva_draw_contour(img_scene, pts_obj_in_scene, cv::Scalar(0, 255, 0), 4);
    
    if (use_info_text)
    {
        rva_mostrarInfo(img_scene, info_text, pts_obj_in_scene);
   
        // Show the result
        cv::imshow("AugmentedReality-Info", img_scene);

        // Check pressed keys to take action
        // *** TODO: COMPLETAR ***
        cv::waitKey(0);
    }
    else if (use_patch)
    {
        std::cout << "Aplicando patch...\n";
        rva_dibujaPatch(img_scene, img_patch, H, img_scene);

        // Show the result
        cv::imshow("AugmentedReality-Patch", img_scene);

        // Check pressed keys to take action
        // *** TODO: COMPLETAR ***
        cv::waitKey(0);         
    }
    else {
        std::cout << "Aplicando video...\n";

        // Grab frames from the video and overlay the frame on the object region
        cv::Mat frame;
        while (cap2.read(frame)) {
            // Warp the read frame to the object region and overlay it
            // *** TODO: COMPLETAR ***
            cv::resize(frame, frame, img_model.size());
            cv::Mat output;
            rva_dibujaPatch(img_scene, frame, H, output);

            // Show the result
            // *** TODO: COMPLETAR ***
            cv::imshow("AugmentedReality-Video", output);
            if (cv::waitKey(30) >= 0) break;
        }

    }
        
    // The camera will be deinitialized automatically in VideoCapture destructor
	cv::destroyAllWindows();
    if (cap2.isOpened())
        cap2.release();
	
    return 0;
}

