#include <iostream>
#include <libcamera/libcamera.h>
#include <libcamera/camera_manager.h>
#include <libcamera/camera.h>
#include <libcamera/framebuffer_allocator.h>
#include <libcamera/request.h>
#include <opencv2/opencv.hpp>

using namespace libcamera;
using namespace std;

int main()
{
    // カメラマネージャーの作成
    CameraManager manager;
    manager.start();

    if (manager.cameras().empty()) {
        cerr << "カメラが見つかりません" << endl;
        return -1;
    }

    shared_ptr<Camera> camera = manager.cameras()[0];
    camera->acquire();

    // カメラの設定
    unique_ptr<CameraConfiguration> config = camera->generateConfiguration({ StreamRole::StillCapture });

    if (config->validate() == CameraConfiguration::Invalid) {
        cerr << "カメラの設定が無効です" << endl;
        return -1;
    }

    // ストリームの設定
    StreamConfiguration &streamConfig = config->at(0);
    streamConfig.size.width = 640;
    streamConfig.size.height = 480;
    streamConfig.pixelFormat = formats::BGR888;

    if (config->validate() == CameraConfiguration::Invalid) {
        cerr << "カメラの設定が無効です" << endl;
        return -1;
    }

    camera->configure(config.get());

    // フレームバッファのアロケータ
    FrameBufferAllocator allocator(camera);
    for (StreamConfiguration &cfg : *config) {
        allocator.allocate(cfg.stream());
    }

    // フレームバッファの取得
    Stream *stream = streamConfig.stream();
    vector<unique_ptr<Request>> requests;
    for (const unique_ptr<FrameBuffer> &buffer : allocator.buffers(stream)) {
        unique_ptr<Request> request = camera->createRequest();
        request->addBuffer(stream, buffer.get());
        requests.push_back(move(request));
    }

    // カメラの開始
    camera->start();

    // 画像のキャプチャ
    for (unique_ptr<Request> &request : requests) {
        camera->queueRequest(request.get());
        request->wait();

        const Request::BufferMap &buffers = request->buffers();
        auto it = buffers.begin();
        const FrameBuffer *buffer = it->second;

        // 画像の取得
        const FrameBuffer::Plane &plane = buffer->planes()[0];
        void *data = plane.mem;
        cv::Mat img(cv::Size(640, 480), CV_8UC3, data, cv::Mat::AUTO_STEP);
        
        // 画像の保存
        cv::imwrite("captured_image.jpg", img);
        cout << "画像を保存しました: captured_image.jpg" << endl;
    }

    // カメラの停止
    camera->stop();
    camera->release();
    manager.stop();

    return 0;
}