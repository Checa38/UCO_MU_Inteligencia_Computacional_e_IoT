#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "common_code.hpp"

void fsiv_compute_derivate(cv::Mat const &img, cv::Mat &dx, cv::Mat &dy, int g_r,
                           int s_ap)
{
    CV_Assert(img.type() == CV_8UC1);
    // TODO
    // Remember: if g_r > 0 apply a previous Gaussian Blur operation with kernel size 2*g_r+1.file:///home/student/Escritorio/edge_detector/edge_detector.cpp
    
    // Hint: use Sobel operator to compute derivate.
    cv::Mat blurred_img = img.clone();
    if (g_r > 0) {
        int kernel_size = 2 * g_r + 1; 
        cv::GaussianBlur(img, blurred_img, cv::Size(kernel_size, kernel_size), 0);
    }
    cv::Sobel(blurred_img, dx, CV_32F, 1, 0, s_ap, 1, 0, cv::BORDER_DEFAULT);  
    cv::Sobel(blurred_img, dy, CV_32F, 0, 1, s_ap, 1, 0, cv::BORDER_DEFAULT); 
    //
    CV_Assert(dx.size() == img.size());
    CV_Assert(dy.size() == dx.size());
    CV_Assert(dx.type() == CV_32FC1);
    CV_Assert(dy.type() == CV_32FC1);
}

void fsiv_compute_gradient_magnitude(cv::Mat const &dx, cv::Mat const &dy,
                                     cv::Mat &gradient)
{
    CV_Assert(dx.size() == dy.size());
    CV_Assert(dx.type() == CV_32FC1);
    CV_Assert(dy.type() == CV_32FC1);

    // TODO
    // Hint: use cv::magnitude.
    cv::magnitude(dx, dy, gradient);
    //

    CV_Assert(gradient.size() == dx.size());
    CV_Assert(gradient.type() == CV_32FC1);
}

void fsiv_compute_gradient_histogram(cv::Mat const &gradient, int n_bins, cv::Mat &hist, float &max_gradient)
{
    // TODO
    // Hint: use cv::minMaxLoc to get the gradient range {0, max_gradient}
    // Find the maximum gradient value
    double max_value;
    cv::minMaxLoc(gradient, nullptr, &max_value);
    max_gradient = (float)(max_value);
    
    int histSize[] = {n_bins};
    float range[] = {0,float(max_gradient)};
    const float* histRange[] = {range};
    cv::calcHist(&gradient, 1, 0, cv::Mat(), hist, 1, histSize, histRange, true, false);
    //
    CV_Assert(max_gradient > 0.0);
    CV_Assert(hist.rows == n_bins);
}

int fsiv_compute_histogram_percentile(cv::Mat const &hist, float percentile)
{
    CV_Assert(percentile >= 0.0 && percentile <= 1.0);
    CV_Assert(hist.type() == CV_32FC1);
    CV_Assert(hist.cols == 1);
    int idx = -1;
    // TODO
    // Hint: use cv::sum to compute the histogram area.
    // Remember: The percentile p is the first i that sum{h[0], h[1], ..., h[i]} >= p
    float total_area = static_cast<float>(cv::sum(hist)[0]); // Área total del histograma
    float cumulative_area = 0.0f;

    for (int i = 0; i < hist.rows; ++i) 
    {
        cumulative_area += hist.at<float>(i);  
        if (cumulative_area / total_area >= percentile) 
        {
            idx = i;  
            break;
        }
    }

    if (percentile == 1.0f) 
    {
        idx = hist.rows - 1;
    }
    //
    CV_Assert(idx >= 0 && idx < hist.rows);
    CV_Assert(idx == 0 || cv::sum(hist(cv::Range(0, idx), cv::Range::all()))[0] / cv::sum(hist)[0] < percentile);
    CV_Assert(cv::sum(hist(cv::Range(0, idx + 1), cv::Range::all()))[0] / cv::sum(hist)[0] >= percentile);
    return idx;
}

float fsiv_histogram_idx_to_value(int idx, int n_bins, float max_value,
                                  float min_value)
{
    CV_Assert(idx >= 0);
    CV_Assert(idx < n_bins);
    float value = 0.0;
    // TODO
    // Remember: Map integer range [0, n_bins) into float
    // range [min_value, max_value)

    float bin_width = (max_value - min_value) / n_bins;
    value = min_value + idx * bin_width;
    //
    CV_Assert(value >= min_value);
    CV_Assert(value < max_value);
    return value;
}

void fsiv_percentile_edge_detector(cv::Mat const &gradient, cv::Mat &edges,
                                   float th, int n_bins)
{
    CV_Assert(gradient.type() == CV_32FC1);

    // TODO
    // Remember: user other fsiv_xxx to compute histogram and percentiles.
    // Remember: map histogram range {0, ..., n_bins} to the gradient range
    // {0.0, ..., max_grad}
    // Hint: use "operator >=" to threshold the gradient magnitude image.
    
    cv::Mat hist;
    float max_gradient;
    fsiv_compute_gradient_histogram(gradient, n_bins, hist, max_gradient);

    int percentile_idx = fsiv_compute_histogram_percentile(hist, th);

    float threshold_value = fsiv_histogram_idx_to_value(percentile_idx, n_bins, max_gradient, 0.0f);

    edges = cv::Mat::zeros(gradient.size(), CV_8UC1);
    edges = gradient >= threshold_value;
    //
    CV_Assert(edges.type() == CV_8UC1);
    CV_Assert(edges.size() == gradient.size());
}

void fsiv_otsu_edge_detector(cv::Mat const &gradient, cv::Mat &edges)
{
    CV_Assert(gradient.type() == CV_32FC1);

    // TODO
    // Hint: normalize input gradient into rango [0, 255] to use
    // cv::threshold properly.
    //

    cv::Mat normalized_gradient;
    double min_val, max_val;
    cv::minMaxLoc(gradient, &min_val, &max_val);
    gradient.convertTo(normalized_gradient, CV_8UC1, 255.0 / max_val);

    double otsu_thresh = cv::threshold(normalized_gradient, edges, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

    //
    CV_Assert(edges.type() == CV_8UC1);
    CV_Assert(edges.size() == gradient.size());
}

void fsiv_canny_edge_detector(cv::Mat const &dx, cv::Mat const &dy, cv::Mat &edges,
                              float th1, float th2, int n_bins)
{
    CV_Assert(dx.size() == dy.size());
    CV_Assert(th1 < th2);

    // TODO
    // Hint: convert the intput derivatives to CV_16C1 to be used with canny.
    // Remember: th1 and th2 are given as percentiles so you must transform to
    //           gradient range to be used in canny method.
    // Remember: we compute gradients with L2_NORM so we must indicate this in
    //           the canny method too.

    cv::Mat gradient_magnitude;
    cv::magnitude(dx, dy, gradient_magnitude);

    cv::Mat hist;
    float max_gradient;
    fsiv_compute_gradient_histogram(gradient_magnitude, n_bins, hist, max_gradient);

    int th1_idx = fsiv_compute_histogram_percentile(hist, th1);
    int th2_idx = fsiv_compute_histogram_percentile(hist, th2);

    float threshold1 = fsiv_histogram_idx_to_value(th1_idx, n_bins, max_gradient, 0.0f);
    float threshold2 = fsiv_histogram_idx_to_value(th2_idx, n_bins, max_gradient, 0.0f);

    cv::Mat dx_int, dy_int;
    dx.convertTo(dx_int, CV_16SC1);
    dy.convertTo(dy_int, CV_16SC1);

    cv::Canny(dx_int, dy_int, edges, threshold1, threshold2, true);

    //
    CV_Assert(edges.type() == CV_8UC1);
    CV_Assert(edges.size() == dx.size());
}

void fsiv_compute_ground_truth_image(cv::Mat const &consensus_img,
                                     float min_consensus, cv::Mat &gt)
{
    //! TODO
    // Hint: use cv::normalize to normalize consensus_img into range (0, 100)
    // Hint: use "operator >=" to threshold the consensus image.

    cv::Mat normalized_img;
    cv::normalize(consensus_img, normalized_img, 0.0, 100.0, cv::NORM_MINMAX);

    gt = (normalized_img >= min_consensus);

    gt.convertTo(gt, CV_8UC1, 255.0);
    //
    CV_Assert(consensus_img.size() == gt.size());
    CV_Assert(gt.type() == CV_8UC1);
}

void fsiv_compute_confusion_matrix(cv::Mat const &gt, cv::Mat const &pred, cv::Mat &cm)
{
    CV_Assert(gt.type() == CV_8UC1);
    CV_Assert(pred.type() == CV_8UC1);
    CV_Assert(gt.size() == pred.size());

    // TODO
    // Remember: a edge detector confusion matrix is a 2x2 matrix where the
    // rows are ground truth {Positive: "is edge", Negative: "is not edge"} and
    // the columns are the predictions labels {"is edge", "is not edge"}
    // A pixel value means edge if it is <> 0, else is a "not edge" pixel.

    cm = cv::Mat::zeros(2, 2, CV_32FC1);

    for (int i = 0; i < gt.rows; ++i)
    {
        for (int j = 0; j < gt.cols; ++j)
        {
            bool is_gt_edge = gt.at<uchar>(i, j) != 0;
            bool is_pred_edge = pred.at<uchar>(i, j) != 0;

            if (is_gt_edge && is_pred_edge)
                cm.at<float>(0, 0)++; // TP
            else if (is_gt_edge && !is_pred_edge)
                cm.at<float>(0, 1)++; // FN
            else if (!is_gt_edge && is_pred_edge)
                cm.at<float>(1, 0)++; // FP
            else
                cm.at<float>(1, 1)++; // TN
        }
    }
    //
    CV_Assert(cm.type() == CV_32FC1);
    CV_Assert(cv::abs(cv::sum(cm)[0] - (gt.rows * gt.cols)) < 1.0e-6);
}

float fsiv_compute_sensitivity(cv::Mat const &cm)
{
    CV_Assert(cm.type() == CV_32FC1);
    CV_Assert(cm.size() == cv::Size(2, 2));
    float sensitivity = 0.0;
    // TODO
    // Hint: see https://en.wikipedia.org/wiki/Confusion_matrix

    sensitivity = cm.at<float>(0, 0) / (cm.at<float>(0, 0) + cm.at<float>(0, 1));
    //
    return sensitivity;
}

float fsiv_compute_precision(cv::Mat const &cm)
{
    CV_Assert(cm.type() == CV_32FC1);
    CV_Assert(cm.size() == cv::Size(2, 2));
    float precision = 0.0;
    // TODO
    // Hint: see https://en.wikipedia.org/wiki/Confusion_matrix
    
    precision = cm.at<float>(0, 0) / (cm.at<float>(0, 0) + cm.at<float>(1, 0));
    //
    return precision;
}

float fsiv_compute_F1_score(cv::Mat const &cm)
{
    CV_Assert(cm.type() == CV_32FC1);
    CV_Assert(cm.size() == cv::Size(2, 2));
    float F1 = 0.0;
    // TODO
    // Hint: see https://en.wikipedia.org/wiki/Confusion_matrix
    // Compute sensitivity and precision
    float sensitivity = fsiv_compute_sensitivity(cm);
    float precision = fsiv_compute_precision(cm);
    
    F1 = 2 * (precision * sensitivity) / (precision + sensitivity);
    //
    return F1;
}
