#include <iostream>

#include <opencv2/imgproc.hpp>
#include <opencv2/video.hpp>
#include <opencv2/opencv.hpp>

using namespace cv;

enum Direction {ShiftUp=0, ShiftRight, ShiftDown, ShiftLeft};

static cv::Mat shiftFrame(cv::Mat frame, int pixels, Direction direction);
static bool isValidRect(const cv::Mat & m, const cv::Rect & roi);

std::vector<std::pair<int, int>> moveGenerator();

int max_shift = 12;
int period = 32;

int main(int argc,  char *argv[]) {

    auto moves = moveGenerator();

//    for(int i=0; i< 32; ++i){
//        std::cout << moves[i].first << " " << moves[i].second << std::endl;
//    }

    cv::VideoCapture cap(argv[1]);
    Mat frame;

    cap >> frame;
    VideoWriter video(argv[2], CV_FOURCC('D', 'I','V','X'),25, frame.size());

    auto framecount =1;
    while(frame.data){
        if((framecount) % period == 0){
            moves = moveGenerator();
        }

        auto move = moves[framecount%period];
        auto shiftedFrame = shiftFrame(frame, move.second, static_cast<Direction>(move.first));

        cv::imshow("frame", frame);
        cv::imshow("shifted frame", shiftedFrame);

        video << shiftedFrame;
        cap >> frame;
        framecount++;

        // Press  ESC on keyboard to  exit
        char c = (char)waitKey(1);
        if( c == 27 )
            break;
    }
    cap.release();
    video.release();
    return 0;
}

std::vector<std::pair<int, int>> moveGenerator() {
    srand(time(nullptr));
    std::vector<std::pair<int, int>> moves; ///Direction, value
    //setup random moves:

    int actual[4]= {0};
    for(int i=0; i< period-2; ++i){
        int direction = rand() %4;
        int shift = rand() % max_shift;
        if(actual[direction]+shift >= max_shift){
            i--; continue;
        }
        moves.emplace_back(std::make_pair(direction, shift));
        actual[direction] += shift;
        actual[(direction+2)%4] -= shift;
    }
    if(actual[0]>=0) moves.emplace_back(std::make_pair(2, actual[0]));
    else moves.emplace_back(std::make_pair(0, actual[2]));

    if(actual[1]>=0) moves.emplace_back(std::make_pair(3, actual[1]));
    else moves.emplace_back(std::make_pair(1, actual[3]));

    return moves;
}


static bool isValidRect(const cv::Mat & m, const cv::Rect & roi){
    return (0 <= roi.x && 0 <= roi.width && roi.x + roi.width <= m.cols && 0 <= roi.y && 0 <= roi.height && roi.y + roi.height <= m.rows);
}


cv::Mat shiftFrame(cv::Mat frame, int pixels, Direction direction)
{
    //create a same sized temporary Mat with all the pixels flagged as invalid (-1)
    cv::Mat temp = cv::Mat::zeros(frame.size(), frame.type());

    cv::Rect src, dst;
    switch (direction)
    {
        case(ShiftUp) :
            src = cv::Rect(0, pixels, frame.cols, frame.rows - pixels);
            dst = cv::Rect(0, 0, temp.cols, temp.rows - pixels);
            if(!(isValidRect(frame, src) && isValidRect(temp, dst))){
                return frame;
            }
            frame(src).copyTo(temp(dst));
            break;
        case(ShiftRight) :
            src = cv::Rect(0, 0, frame.cols - pixels, frame.rows);
            dst = cv::Rect(pixels, 0, frame.cols - pixels, frame.rows);
            if(!(isValidRect(frame, src) && isValidRect(temp, dst))){
                return frame;
            }
            frame(src).copyTo(temp(dst));
            break;
        case(ShiftDown) :
            src = cv::Rect(0, 0, frame.cols, frame.rows - pixels);
            dst = cv::Rect(0, pixels, frame.cols, frame.rows - pixels);
            if(!(isValidRect(frame, src) && isValidRect(temp, dst))){
                return frame;
            }
            frame(src).copyTo(temp(dst));
            break;
        case(ShiftLeft) :
            src = cv::Rect(pixels, 0, frame.cols - pixels, frame.rows);
            dst = cv::Rect(0, 0, frame.cols - pixels, frame.rows);
            if(!(isValidRect(frame, src) && isValidRect(temp, dst))){
                return frame;
            }
            frame(src).copyTo(temp(dst));
            break;
        default:
            std:: cout << "Shift direction is not set properly" << std::endl;
    }

    return temp;
}