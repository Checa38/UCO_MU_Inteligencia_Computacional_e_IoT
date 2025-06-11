
#include "common_code.hpp"

void 
fsiv_find_min_max_loc_1(cv::Mat const& input,
    std::vector<cv::uint8_t>& min_v, std::vector<cv::uint8_t>& max_v,
    std::vector<cv::Point>& min_loc, std::vector<cv::Point>& max_loc)
{
    CV_Assert(input.depth()==CV_8U);

    //! TODO: do a rows/cols scanning to find the first min/max values. 
    // Hint: use cv::split to get the input image channels.
    std::vector<cv::Mat> channels;
    cv::split(input, channels);
    int num_channels = channels.size();

    min_v.resize(num_channels, 255);
    max_v.resize(num_channels, 0);
    min_loc.resize(num_channels);
    max_loc.resize(num_channels);

    for (int c = 0; c < num_channels; ++c)
    {
        for (int row = 0; row < input.rows; ++row)
        {
            for (int col = 0; col < input.cols; ++col)
            {
                uint8_t point = channels[c].at<uint8_t>(row, col);

                if (point < min_v[c])
                {
                    min_v[c] = point;
                    min_loc[c] = cv::Point(col, row);
                }

                if (point > max_v[c])
                {
                    max_v[c] = point;
                    max_loc[c] = cv::Point(col, row);
                }
            }
        }
    }
    //

    CV_Assert(input.channels()==min_v.size());
    CV_Assert(input.channels()==max_v.size());
    CV_Assert(input.channels()==min_loc.size());
    CV_Assert(input.channels()==max_loc.size());
}

void 
fsiv_find_min_max_loc_2(cv::Mat const& input,
    std::vector<double>& min_v, std::vector<double>& max_v,
    std::vector<cv::Point>& min_loc, std::vector<cv::Point>& max_loc)
{

    //! TODO: Find the first min/max values using cv::minMaxLoc function.    
    // Hint: use cv::split to get the input image channels.
    std::vector<cv::Mat> channels;
    cv::split(input, channels);

    int num_channels = channels.size();

    min_v.resize(num_channels);
    max_v.resize(num_channels);
    min_loc.resize(num_channels);
    max_loc.resize(num_channels);

    for (int c = 0; c < num_channels; ++c)
    {
        cv::minMaxLoc(channels[c], &min_v[c], &max_v[c], &min_loc[c], &max_loc[c]);
    }
    //

    CV_Assert(input.channels()==min_v.size());
    CV_Assert(input.channels()==max_v.size());
    CV_Assert(input.channels()==min_loc.size());
    CV_Assert(input.channels()==max_loc.size());

}

