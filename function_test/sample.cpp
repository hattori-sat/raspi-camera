#include <iostream>
#include <thread>
#include <vector>
#include <opencv2/opencv.hpp>
#include "picamera2.hpp"

using namespace std;
using namespace cv;
using namespace picamera2;

constexpr int WIDTH = 640; // 解像度（幅）
constexpr int HEIGHT = 480; // 解像度（高さ）
constexpr int FPS = 60; // フレームレート
constexpr int DURATION = 1; // 撮影する秒数

// 画像を保存する関数
void saveImages(const vector<Mat>& frames) {
    for (size_t i = 0; i < frames.size(); ++i) {
        string filename = "frame_" + to_string(i) + ".jpg";
        imwrite(filename, frames[i]);
    }
}

// 画像処理を行う関数（例としてエッジ検出を行う）
void processImages(const vector<Mat>& frames) {
    for (const auto& frame : frames) {
        Mat edges;
        Canny(frame, edges, 100, 200);
        // 処理結果を保存する
        static int index = 0;
        string filename = "processed_" + to_string(index++) + ".jpg";
        imwrite(filename, edges);
    }
}

int main() {
    Picamera2 picam;
    auto config = picam.createStillConfiguration({WIDTH, HEIGHT});
    config->setFrameRate(FPS);
    picam.configure(config);

    picam.start();

    vector<Mat> frames;
    frames.reserve(FPS * DURATION);

    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < FPS * DURATION; ++i) {
        auto frame = picam.captureFrame();
        Mat img(Size(WIDTH, HEIGHT), CV_8UC3, frame.data(), Mat::AUTO_STEP);
        frames.push_back(img.clone());
        this_thread::sleep_for(chrono::milliseconds(1000 / FPS));
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    cout << "Captured " << frames.size() << " frames in " << elapsed.count() << " seconds." << endl;

    picam.stop();

    // 並列処理で画像を保存および処理する
    thread saveThread(saveImages, ref(frames));
    thread processThread(processImages, ref(frames));

    saveThread.join();
    processThread.join();

    return 0;
}
