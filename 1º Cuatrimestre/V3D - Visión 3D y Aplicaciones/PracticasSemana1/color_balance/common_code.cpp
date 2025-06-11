#include "common_code.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

cv::Mat fsiv_color_rescaling(const cv::Mat &in, const cv::Scalar &from, const cv::Scalar &to)
{
    CV_Assert(in.type() == CV_8UC3);
    cv::Mat out;
    // TODO
    // HINT: use cv:divide to compute the scaling factor.
    // HINT: use method cv::Mat::mul() to scale the input matrix.
    cv::Scalar rescaling;
    cv::divide(to, from, rescaling);
    out=in.mul(rescaling);
    //
    CV_Assert(out.type() == in.type());
    CV_Assert(out.rows == in.rows && out.cols == in.cols);
    return out;
}

cv::Mat fsiv_gray_world_color_balance(cv::Mat const &in)
{
    CV_Assert(in.type() == CV_8UC3);
    cv::Mat out;
    // TODO
    //  HINT: use cv::mean to compute the mean pixel value.
    // Calcular el valor medio de cada canal (B, G, R)

    cv::Scalar mean = cv::mean(in);
    out = fsiv_color_rescaling(in, mean, cv::Scalar(128,128,128));
    //
    CV_Assert(out.type() == in.type());
    CV_Assert(out.rows == in.rows && out.cols == in.cols);
    return out;
}

cv::Mat fsiv_convert_bgr_to_gray(const cv::Mat &img, cv::Mat &out)
{
    CV_Assert(img.channels() == 3);
    // TODO
    // HINT: use cv::cvtColor()
    cv::cvtColor(img, out, cv::COLOR_BGR2GRAY);
    //
    CV_Assert(out.channels() == 1);
    return out;
}

cv::Mat fsiv_compute_image_histogram(cv::Mat const &img)
{
    CV_Assert(img.type() == CV_8UC1);
    cv::Mat hist;
    // TODO
    // Hint: use cv::calcHist().
    int histSize = 256;  
    float range[] = { 0, 256 }; 
    const float* histRange = { range };
    cv::calcHist(&img, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange);
    //
    CV_Assert(!hist.empty());
    CV_Assert(hist.type() == CV_32FC1);
    CV_Assert(hist.rows == 256 && hist.cols == 1);
    return hist;
}

float fsiv_compute_histogram_percentile(cv::Mat const &hist, float p_value)
{
    CV_Assert(hist.type() == CV_32FC1);
    CV_Assert(hist.cols == 1);
    CV_Assert(0.0 <= p_value && p_value <= 1.0);

    int p = 0;

    // TODO
    // Remember: find the smaller index 'p' such that
    //           sum(h[0], h[1], ... , h[p]) >= p_value*area(hist)
    // Hint: use cv::sum() to compute the histogram area.

    float area = cv::sum(hist)[0];
    float threshold = p_value * area; 
    float cumulative_sum = 0.0f;
    
    for (; p < hist.rows; ++p)
    {
        cumulative_sum += hist.at<float>(p); 
        if (cumulative_sum >= threshold)
        {
            break;
        }
    }
    //

    CV_Assert(0 <= p && p < hist.rows);
    return p;
}

cv::Mat fsiv_white_patch_color_balance(cv::Mat const &in, float p)
{
    CV_Assert(in.type() == CV_8UC3);
    CV_Assert(0.0f <= p && p <= 100.0f);
    cv::Mat out;
    if (p == 0.0)
    {
        // TODO
        // HINT: convert to GRAY color space to get the illuminance.
        // HINT: use cv::minMaxLoc to locate the brightest pixel.
        // HINT: use fsiv_color_rescaling when the "from" scalar was computed.
        cv::Mat gray;
        fsiv_convert_bgr_to_gray(in, gray);
        double minVal, maxVal;
        cv::Point minLoc, maxLoc;
        cv::minMaxLoc(gray, &minVal, &maxVal, &minLoc, &maxLoc);
        cv::Scalar from = in.at<cv::Vec3b>(maxLoc);
        out = fsiv_color_rescaling(in, from, cv::Scalar(255, 255, 255));  
        //
    }
    else
    {
        // TODO
        // HINT: convert to GRAY color space to get the illuminance.
        // HINT: Compute a gray level histogram to find the 100-p percentile.
        // HINT: use operator >= to get the mask with p% brighter pixels and use it
        //        to compute the mean value.
        // HINT: use fsiv_color_rescaling when the "from" scalar was computed.
        cv::Mat gray;
        fsiv_convert_bgr_to_gray(in, gray);

        cv::Mat hist = fsiv_compute_image_histogram(gray);

        float percentile_value = fsiv_compute_histogram_percentile(hist, 1-(p/100));

        cv::Mat mask = gray >= percentile_value;
        cv::Scalar mean_value = cv::mean(in, mask);
        cv::Scalar from(mean_value[0], mean_value[1], mean_value[2]);
        out = fsiv_color_rescaling(in, from, cv::Scalar(255, 255, 255));  
        //
    }

    CV_Assert(out.type() == in.type());
    CV_Assert(out.rows == in.rows && out.cols == in.cols);
    return out;
}
