#include "common_code.hpp"

cv::Mat
fsiv_convert_image_byte_to_float(const cv::Mat &img)
{
    CV_Assert(img.depth() == CV_8U);
    cv::Mat out;
    //! TODO
    // Hint: use cv::Mat::convertTo().

    img.convertTo(out, CV_32F, 1.0 / 255.0);

    //
    CV_Assert(out.rows == img.rows && out.cols == img.cols);
    CV_Assert(out.depth() == CV_32F);
    CV_Assert(img.channels() == out.channels());
    return out;
}

cv::Mat
fsiv_convert_image_float_to_byte(const cv::Mat &img)
{
    CV_Assert(img.depth() == CV_32F);
    cv::Mat out;
    //! TODO
    // Hint: use cv::Mat::convertTo()

    img.convertTo(out, CV_8U, 255.0);

    //
    CV_Assert(out.rows == img.rows && out.cols == img.cols);
    CV_Assert(out.depth() == CV_8U);
    CV_Assert(img.channels() == out.channels());
    return out;
}

cv::Mat
fsiv_convert_bgr_to_hsv(const cv::Mat &img)
{
    CV_Assert(img.channels() == 3);
    cv::Mat out;
    //! TODO
    // Hint: use cvtColor.
    // Remember: the input color scheme is assumed to be BGR.

    cvtColor(img, out, cv::COLOR_BGR2HSV);

    //
    CV_Assert(out.channels() == 3);
    return out;
}

cv::Mat
fsiv_convert_hsv_to_bgr(const cv::Mat &img)
{
    CV_Assert(img.channels() == 3);
    cv::Mat out;
    //! TODO
    // Hint: use cvtColor.
    // Remember: the ouput color scheme is assumed to be BGR.
    cvtColor(img, out, cv::COLOR_HSV2BGR);
    //
    CV_Assert(out.channels() == 3);
    return out;
}

cv::Mat
fsiv_cbg_process(const cv::Mat &in,
                 double contrast, double brightness, double gamma,
                 bool only_luma)
{
    CV_Assert(in.depth() == CV_8U);
    cv::Mat out;
    // TODO
    // Hint: convert to float range [0,1] before processing the image.
    // Hint: use cv::pow() to apply the gamma parameter.
    // Hint: if input channels is 3 and only luma is required, convert to HSV
    //       color space and process only de V (luma) channel.

    std::vector<cv::Mat> canales; 
    out = fsiv_convert_image_byte_to_float(in); 
    // Caso 1: Control luna
    if(only_luma && in.channels() == 3)
    {
        cv::Mat hsv = out.clone(); 
        hsv = fsiv_convert_bgr_to_hsv(out); 
        cv::split(hsv,canales); 
        cv::pow(canales[2], gamma, canales[2]);
        canales[2] = (contrast * canales[2]) + brightness;
        cv::merge(canales,hsv); 
        out=fsiv_convert_hsv_to_bgr(hsv);
    }
    // sin control luma
    else
    {
        cv::split(out,canales); 
        for(int i=0; i < canales.size(); i++)
        {
            cv::pow(canales[i], gamma, canales[i]);
            canales[i] = (contrast * canales[i]) + brightness;
        }
        cv::merge(canales,out);
    }

    out = fsiv_convert_image_float_to_byte(out);
    //
    CV_Assert(out.rows == in.rows && out.cols == in.cols);
    CV_Assert(out.depth() == CV_8U);
    CV_Assert(out.channels() == in.channels());
    return out;
}
