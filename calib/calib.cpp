#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

const size_t center_window_interval = 500;
const Size windowSize(800, 600);

void recogniseStickersByThreshold(Mat &frame,Point &QRCenter)
{

    Mat mask(frame.size(),CV_8U);
    cvtColor(frame, mask, COLOR_BGR2GRAY); 
    threshold(mask, mask, 180, 255, THRESH_BINARY);

    Mat mask2(frame.size(),CV_8U);
    cvtColor(frame, mask2, COLOR_BGR2GRAY); 
    threshold(mask2, mask2, 75, 255, THRESH_BINARY_INV);

    vector<vector<Point>> contours; 
    vector<Vec4i> hierarchy;
    findContours(mask, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    size_t biggest_contour = -1;
    size_t max_area = 0;
    for(size_t i = 0; i < contours.size(); i++)
    {
        size_t cur_area = contourArea(contours[i]);
        if (cur_area > max_area) 
        {
            biggest_contour = i;
            max_area = cur_area;
        }
    }
    if (biggest_contour != -1)
    {
        Rect rect = boundingRect(contours[biggest_contour]);
        rectangle(frame,rect, Scalar(0,255,255),2);
        QRCenter.x = rect.x + rect.width/2;
        QRCenter.y = rect.y + rect.height/2;
    }
}

void sendConrolSignal(const Point &QRCenter, const Size windowSize)
{

}

void Calibration()
{
    int CHECKERBOARD[2] = {7,7};
    //int criteria[3] = {TERM_CRITERIA_EPS + TERM_CRITERIA_MAX_ITER, 30, 0.001};
    // Создание вектора для хранения векторов 3D точек для каждого изображения шахматной доски
    vector<vector<Point3f>> objpoints; 
    // Создание вектора для хранения векторов 2D точек для каждого изображения шахматной доски
    vector<vector<Point2f>> imgpoints;

    // Определение мировых координат для 3D точек
    vector<Point3f> objp;

    for (int i=0;i<CHECKERBOARD[1];i++)
        for (int j=0;j<CHECKERBOARD[0];j++)
            objp.push_back(Point3f(j,i,0));
            
    Mat frame,gray;
    vector<Point2f> corner_pts;
    bool success;

    vector<String> images;
    string path = "./images/*.jpg";
    glob(path, images);

    for(int i=0; i<images.size(); i++)
    {
        frame = imread(images[i]);
        cvtColor(frame,gray,COLOR_BGR2GRAY);

        success = findChessboardCorners(gray, Size(CHECKERBOARD[0], CHECKERBOARD[1]), corner_pts, CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_FAST_CHECK | CALIB_CB_NORMALIZE_IMAGE);
        if(success)
        {
            TermCriteria criteria(TermCriteria::EPS | TermCriteria::COUNT, 30, 0.001);
            cornerSubPix(gray,corner_pts,Size(11,11), Size(-1,-1),criteria);
            objpoints.push_back(objp);
            imgpoints.push_back(corner_pts);
        }
        
    }
    Mat cameraMatrix,distCoeffs,R,T;
    //calibrateCamera(objpoints, imgpoints, Size(gray.rows,gray.cols), cameraMatrix, distCoeffs, R, T);
    cout << "Matrix: " << cameraMatrix << endl;
    cout << "distCoeffs: " << distCoeffs << endl;
    cout << "Rotation vector: " << R << endl;
    cout << "Translation vector: " << T << endl;

}

int main() 
{
    Calibration();
    VideoCapture cap("1.mp4"); // open the video file for reading
    if (!cap.isOpened()) return -1;
    vector<vector<Point>> vLine;
    vector<Point> x;
    for (int i=0;i<2;i++)
    {
        vLine.push_back(x);
    }

    namedWindow("MyVideo",WINDOW_AUTOSIZE); //create a window called "MyVideo"
    while(1) 
    {        
        Mat frame;
        bool flag =cap.read(frame);
        if (!flag) break;

        //Point QRCenter;
        //recogniseStickersByThreshold(frame,QRCenter);
        //cout << "x: " << QRCenter.x << " y: " << QRCenter.y << endl;
        //sendConrolSignal(QRCenter, windowSize);

        //Calibration(frame);
        //resize(frame,frame,windowSize);
        imshow("MyVideo", frame); //show the frame in "MyVideo" window
        if(waitKey(30) == 27)  break;
    }
    return 0;
}