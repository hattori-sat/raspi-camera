Raspberry PiでC++を使って、60fpsで動画を撮影し、その動画から画像を抽出するプログラムを作成する方法について説明します。C++を使うことで、Pythonよりも高速に処理を行うことができます。

### 必要な準備

まず、Raspberry Piに必要な開発環境を整えます。Raspberry Pi OSがインストールされている前提で進めます。

### 必要なソフトウェアのインストール

1. **開発ツールのインストール**:
    ```sh
    sudo apt update
    sudo apt install -y build-essential cmake git
    ```

2. **OpenCVのインストール**:
    OpenCVは画像処理ライブラリで、動画のキャプチャとフレーム抽出に使用します。

    ```sh
    sudo apt install -y libopencv-dev
    ```

3. **FFmpegのインストール**:
    FFmpegは動画の変換に使用します。

    ```sh
    sudo apt install -y ffmpeg
    ```

### C++プログラムの作成

次に、動画を撮影してフレームを抽出するC++プログラムを作成します。

1. **CMakeLists.txtの作成**:
    プロジェクトのビルド設定を行うためのCMakeファイルを作成します。

    ```sh
    mkdir -p ~/video_capture
    cd ~/video_capture
    nano CMakeLists.txt
    ```

    CMakeLists.txtに以下の内容を記述します。

    ```cmake
    cmake_minimum_required(VERSION 3.10)
    project(VideoCapture)

    set(CMAKE_CXX_STANDARD 14)

    find_package(OpenCV REQUIRED)

    add_executable(VideoCapture main.cpp)
    target_link_libraries(VideoCapture ${OpenCV_LIBS})
    ```

2. **main.cppの作成**:
    実際のプログラムを記述するC++ファイルを作成します。

    ```sh
    nano main.cpp
    ```

    main.cppに以下の内容を記述します。

    ```cpp
    #include <opencv2/opencv.hpp>
    #include <iostream>
    #include <cstdlib>
    #include <cstdio>
    #include <sys/time.h>

    using namespace cv;
    using namespace std;

    // Function to get the current timestamp in milliseconds
    long getTimestamp() {
        struct timeval tv;
        gettimeofday(&tv, nullptr);
        return tv.tv_sec * 1000 + tv.tv_usec / 1000;
    }

    // Function to record a video using the Raspberry Pi Camera Module
    void recordVideo(const string &filename, int duration, int fps) {
        string command = "libcamera-vid -t " + to_string(duration * 1000) +
                         " --framerate " + to_string(fps) + " -o " + filename;
        system(command.c_str());
    }

    // Function to extract frames from a video file and save them as images
    void extractFrames(const string &videoFile, const string &outputDir, int fps) {
        VideoCapture cap(videoFile);
        if (!cap.isOpened()) {
            cerr << "Error opening video file" << endl;
            return;
        }

        int frameRate = static_cast<int>(cap.get(CAP_PROP_FPS));
        int frameInterval = frameRate / fps;
        int frameCount = 0;
        Mat frame;
        long startTime = getTimestamp();

        while (cap.read(frame)) {
            if (frameCount % frameInterval == 0) {
                string filename = outputDir + "/frame_" + to_string(frameCount / frameInterval + 1) + ".jpg";
                imwrite(filename, frame);
            }
            frameCount++;
        }

        long endTime = getTimestamp();
        cout << "Extracted frames in " << (endTime - startTime) << " milliseconds." << endl;

        cap.release();
    }

    int main() {
        string videoFile = "/home/pi/video.h264";
        string mp4File = "/home/pi/video.mp4";
        string outputDir = "/home/pi/images_60hz";

        // Create output directory if it doesn't exist
        string createDirCommand = "mkdir -p " + outputDir;
        system(createDirCommand.c_str());

        // Record video
        recordVideo(videoFile, 1, 60);

        // Convert H.264 video to MP4
        string convertCommand = "ffmpeg -i " + videoFile + " -c copy " + mp4File;
        system(convertCommand.c_str());

        // Extract frames from the MP4 video
        extractFrames(mp4File, outputDir, 60);

        return 0;
    }
    ```

### プログラムのビルドと実行

1. **プログラムのビルド**:
    プロジェクトディレクトリで以下のコマンドを実行してプログラムをビルドします。

    ```sh
    mkdir build
    cd build
    cmake ..
    make
    ```

2. **プログラムの実行**:
    ビルドが完了したら、以下のコマンドでプログラムを実行します。

    ```sh
    ./VideoCapture
    ```

### プログラムの説明

1. **recordVideo関数**:
    - `libcamera-vid`コマンドを使用して、指定されたフレームレートで動画を録画します。

2. **extractFrames関数**:
    - OpenCVを使用して、録画した動画からフレームを抽出し、指定されたディレクトリに保存します。

3. **main関数**:
    - `recordVideo`関数で動画を撮影し、`ffmpeg`コマンドを使ってH.264ファイルをMP4に変換します。その後、`extractFrames`関数でフレームを抽出して保存します。

このようにして、Raspberry PiでC++を使って60fpsで動画を撮影し、フレームを抽出することができます。C++を使うことで、Pythonよりも高速に処理を行うことができます。