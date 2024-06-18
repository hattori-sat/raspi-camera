#include <iostream>
#include <memory>
#include <vector>
#include <opencv2/opencv.hpp>
#include <libcamera/libcamera.h>
#include <libcamera/base/signal.h>
#include <libcamera/base/shared_fd.h>
#include <libcamera/framebuffer_allocator.h>
#include <libcamera/control_ids.h>

using namespace std;
using namespace cv;
using namespace libcamera;

int main() {
    // カメラマネージャを初期化
    CameraManager manager;
    manager.start();

    // 利用可能なカメラを取得
    shared_ptr<Camera> camera = manager.get("camera0");
    if (!camera) {
        cerr << "カメラが見つかりません" << endl;
        return -1;
    }

    // カメラを開く
    camera->acquire();

    // ストリームロールを設定
    const StreamRoles roles = { StreamRole::StillCapture };
    unique_ptr<CameraConfiguration> config = camera->generateConfiguration(roles);
    if (config->size() == 0) {
        cerr << "カメラ設定の生成に失敗しました" << endl;
        camera->release();
        return -1;
    }

    // カメラ設定を適用
    config->at(0).pixelFormat = formats::BGR888;
    config->at(0).size = { 640, 480 };
    config->validate();
    camera->configure(config.get());

    // バッファアロケータを作成
    FrameBufferAllocator allocator(camera);
    for (auto &stream : config->streams()) {
        int ret = allocator.allocate(stream);
        if (ret < 0) {
            cerr << "バッファの割り当てに失敗しました" << endl;
            camera->release();
            return -1;
        }
    }

    // キャプチャリクエストの作成
    vector<Request *> requests;
    for (const unique_ptr<FrameBuffer> &buffer : allocator.buffers(config->at(0).stream())) {
        Request *request = camera->createRequest();
        if (!request) {
            cerr << "リクエストの作成に失敗しました" << endl;
            camera->release();
            return -1;
        }
        request->addBuffer(config->at(0).stream(), buffer.get());
        requests.push_back(request);
    }

    vector<Mat> frames;

    // リクエスト完了シグナルにコネクト
    camera->requestCompleted.connect([&](Request *request) {
        const Request::BufferMap &buffers = request->buffers();
        auto it = buffers.begin();
        const FrameBuffer &buffer = *it->second;

        for (const auto &plane : buffer.planes()) {
            void *data = plane.fd->map();
            if (!data) {
                cerr << "プレーンのメモリマッピングに失敗しました" << endl;
                return;
            }

            Mat img(cv::Size(640, 480), CV_8UC3, data, Mat::AUTO_STEP);
            frames.push_back(img.clone());
        }
    });

    // ストリーム開始
    camera->start();

    // キャプチャリクエストのキューイング
    for (Request *request : requests) {
        camera->queueRequest(request);
    }

    // 適当な待機時間
    this_thread::sleep_for(chrono::seconds(2));

    // ストリーム停止
    camera->stop();

    // カメラのリリース
    camera->release();
    manager.stop();

    // 取得したフレームの表示
    for (const Mat &frame : frames) {
        imshow("Frame", frame);
        waitKey(0);
    }

    return 0;
}