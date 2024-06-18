#include <iostream>
#include <vector>
#include <thread>
#include <opencv2/opencv.hpp>
#include <libcamera/libcamera.h>
#include <libcamera/framebuffer_allocator.h>
#include <libcamera/camera_manager.h>
#include <libcamera/camera.h>
#include <libcamera/control_ids.h>
#include <libcamera/formats.h>

using namespace std;
using namespace cv;
using namespace libcamera;

constexpr int WIDTH = 640; // 解像度（幅）
constexpr int HEIGHT = 480; // 解像度（高さ）
constexpr int FPS = 30; // フレームレート
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
    CameraManager cm;
    cm.start();

    shared_ptr<Camera> camera = cm.get("libcamera");
    if (!camera) {
        cerr << "Error: Camera not found" << endl;
        return -1;
    }

    camera->acquire();

    // StreamRoleが正しく定義されているかを確認
    const std::vector<StreamRole> roles = { StreamRole::StillCapture };
    unique_ptr<CameraConfiguration> config = camera->generateConfiguration(roles);
    if (!config) {
        cerr << "Error: Camera configuration failed" << endl;
        return -1;
    }
    config->at(0).pixelFormat = formats::RGB888;
    config->at(0).size = { WIDTH, HEIGHT };
    config->at(0).bufferCount = 4;

    if (camera->configure(config.get()) != 0) {
        cerr << "Error: Camera configuration failed" << endl;
        return -1;
    }

    FrameBufferAllocator allocator(camera);
    for (StreamConfiguration &cfg : *config) {
        Stream *stream = cfg.stream();
        if (allocator.allocate(stream) < 0) {
            cerr << "Error: Buffer allocation failed" << endl;
            return -1;
        }
    }

    vector<shared_ptr<Request>> requests;
    for (unsigned int i = 0; i < allocator.buffers(config->at(0).stream()).size(); ++i) {
        shared_ptr<Request> request = camera->createRequest();
        if (!request) {
            cerr << "Error: Request creation failed" << endl;
            return -1;
        }

        const unique_ptr<FrameBuffer> &buffer = allocator.buffers(config->at(0).stream())[i];
        if (request->addBuffer(config->at(0).stream(), buffer.get()) != 0) {
            cerr << "Error: Adding buffer to request failed" << endl;
            return -1;
        }
        requests.push_back(move(request));
    }

    if (camera->start() != 0) {
        cerr << "Error: Camera start failed" << endl;
        return -1;
    }

    vector<Mat> frames;
    frames.reserve(FPS * DURATION);

    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < FPS * DURATION; ++i) {
        if (camera->queueRequest(requests[i % requests.size()].get()) != 0) {
            cerr << "Error: Queueing request failed" << endl;
            return -1;
        }

        camera->requestCompleted.connect([&](Request *request) {
            const Request::BufferMap &buffers = request->buffers();
            auto it = buffers.begin();
            const FrameBuffer &buffer = *it->second;

            const FrameBuffer::Plane &plane = buffer.planes().front();
            const unsigned char *data = static_cast<const unsigned char *>(plane.memmap());

            Mat img(cv::Size(WIDTH, HEIGHT), CV_8UC3, (void*)data, Mat::AUTO_STEP);
            frames.push_back(img.clone());
        });

        this_thread::sleep_for(chrono::milliseconds(1000 / FPS));
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    cout << "Captured " << frames.size() << " frames in " << elapsed.count() << " seconds." << endl;

    camera->stop();
    camera->release();

    // 並列処理で画像を保存および処理する
    thread saveThread(saveImages, ref(frames));
    thread processThread(processImages, ref(frames));

    saveThread.join();
    processThread.join();

    cm.stop();

    return 0;
}