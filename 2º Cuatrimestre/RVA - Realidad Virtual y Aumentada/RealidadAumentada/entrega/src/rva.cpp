// (c) Realidad Virtual y Aumentada - Universidad de Cordoba - Manuel J. Marin-Jimenez

#include <iostream>
#include <opencv2/opencv.hpp>

#include "rva.h"

// TASK 1
cv::Mat rva_compute_homography(std::vector<cv::Point2f> points_image1, std::vector<cv::Point2f> points_image2)
{
    cv::Mat H = cv::findHomography(points_image1, points_image2, cv::RANSAC);
    return H;
}

void rva_draw_contour(cv::Mat image, std::vector<cv::Point2f> points, cv::Scalar color, int thickness)
{
    cv::line(image, points[0], points[1], color, thickness);
    cv::line(image, points[1], points[2], color, thickness);
    cv::line(image, points[2], points[3], color, thickness);
    cv::line(image, points[3], points[0], color, thickness);
}

void rva_deform_image(const cv::Mat& im_input, cv::Mat & im_output, cv::Mat homography)
{
    cv::warpPerspective(im_input, im_output, homography, im_output.size());
}

// TASK 2

void rva_calculaKPsDesc(const cv::Mat &img, std::vector<cv::KeyPoint> &keypoints, cv::Mat &descriptors)
{
    //cv::Ptr<cv::ORB> detector = cv::ORB::create(4000);
    cv::Ptr<cv::BRISK> detector = cv::BRISK::create();
    detector->detectAndCompute(img, cv::noArray(), keypoints, descriptors);

}

void rva_matchDesc(const cv::Mat &descriptors1, const cv::Mat &descriptors2, std::vector<cv::DMatch> &matches)
{
    cv::BFMatcher matcher(cv::NORM_HAMMING, true); 
    matcher.match(descriptors1, descriptors2, matches);
    /*
    for (const auto &match : matches)
    {
    	printf ("La distancia es: %f\n", match.distance);
    }
    */

}

void rva_dibujaMatches(cv::Mat &img1, cv::Mat &img2, std::vector<cv::KeyPoint> &keypoints1, std::vector<cv::KeyPoint> &keypoints2, std::vector<cv::DMatch> &matches, cv::Mat &img_matches)
{
   cv::drawMatches(img1, keypoints1, img2, keypoints2, matches, img_matches);
}

// TASK 3

void rva_localizaObj(const cv::Mat &img1, const cv::Mat &img2, const std::vector<cv::KeyPoint> &keypoints1, const std::vector<cv::KeyPoint> &keypoints2, const std::vector<cv::DMatch> &matches, cv::Mat &homography, std::vector<cv::Point2f> &pts_im2)
{
    std::vector<cv::Point2f> pts_im1;
    for (const auto &match : matches)
    {
        pts_im1.push_back(keypoints1[match.queryIdx].pt);
        pts_im2.push_back(keypoints2[match.trainIdx].pt);
    }
    
    if (pts_im1.size() < 4 || pts_im2.size() < 4) {
	    std::cerr << "Error: No hay suficientes puntos para calcular la homografía." << std::endl;
	    exit(EXIT_FAILURE);;
	}

    homography = rva_compute_homography(pts_im1, pts_im2);
    std::vector<cv::Point2f> obj_corners = {
        {0, 0},
        {static_cast<float>(img1.cols), 0},
        {static_cast<float>(img1.cols), static_cast<float>(img1.rows)},
        {0, static_cast<float>(img1.rows)}
    };
    perspectiveTransform(obj_corners, pts_im2, homography);
}
// TASK 4

void rva_filterMatches(const std::vector<cv::DMatch> & matches, const std::vector<cv::KeyPoint> & keypoints_model, const std::vector<cv::KeyPoint> & keypoints_scene, std::vector<cv::DMatch> & good_matches)
{
    if (matches.empty()) return;
    
    double min_dist = std::numeric_limits<double>::max();
    double max_dist = 0;
    
    for (const auto &match : matches) {
        double dist = match.distance;
        if (dist < min_dist) min_dist = dist;
        if (dist > max_dist) max_dist = dist;
    }
    
    // Umbral para el filtrado
    float threshold = std::max(3 * min_dist, 30.0); 
    
    // Filtrado
    for (const auto &match : matches) {
        if (match.distance <= threshold) {
            good_matches.push_back(match);
        }
    }
    
}

void rva_dibujaPatch(const cv::Mat & scene, const cv::Mat & patch, const cv::Mat & H, cv::Mat & output)
{
    scene.copyTo(output);

    cv::Mat patch_warped;
    cv::warpPerspective(patch, patch_warped, H, scene.size());

    cv::Mat mask;
    cv::cvtColor(patch_warped, mask, cv::COLOR_BGR2GRAY);
    cv::threshold(mask, mask, 1, 255, cv::THRESH_BINARY); 

    patch_warped.copyTo(output, mask);
}



void rva_mostrarInfo(cv::Mat& image, const std::vector<cv::String>& info, const std::vector<cv::Point2f>& vertices)
{
    float lineSpacing = 20.0f;
    double tamanoFuente = 0.3;
    for (size_t i = 0; i < info.size(); i++) {
        cv::Point2f posicionText = vertices[0] + cv::Point2f(0, i * lineSpacing);
        // Contorno negro
        cv::putText(image, info[i], posicionText, cv::FONT_HERSHEY_SIMPLEX, tamanoFuente, cv::Scalar(0, 0, 0), 3, cv::LINE_AA);
        // Texto
        cv::putText(image, info[i], posicionText, cv::FONT_HERSHEY_SIMPLEX, tamanoFuente, cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
    }
}

