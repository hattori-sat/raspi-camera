#include <libcamera/libcamera.h>
#include <libcamera/camera_manager.h>
#include <libcamera/camera.h>
#include <libcamera/framebuffer_allocator.h>
#include <libcamera/request.h>
#include <libcamera/control_ids.h>
#include <iostream>
#include <fstream>

using namespace libcamera;

int main() {
    // カメラマネージャーを作成
    CameraManager cm;
    cm.start();

    // 使用可能なカメラをリストアップして選択
    if (cm.cameras().empty()) {
        std::cerr << "No cameras available" << std::endl;
        return -1;
    }

    std::shared_ptr<Camera> camera = cm.get(cm.cameras()[0]);
    if (!camera) {
        std::cerr << "Failed to get camera" << std::endl;
        return -1;
    }

    // カメラをアクティブにする
    if (camera->acquire()) {
        std::cerr << "Failed to acquire camera" << std::endl;
        return -1;
    }

    // カメラ構成を設定
    std::unique_ptr<CameraConfiguration> config = camera->generateConfiguration({ StreamRole::Viewfinder });
    if (config->size() == 0) {
        std::cerr << "Failed to generate configuration" << std::endl;
        return -1;
    }

    config->at(0).pixelFormat = libcamera::formats::YUV420;
    config->at(0).size.width = 640;
    config->at(0).size.height = 480;

    if (camera->configure(config.get()) < 0) {
        std::cerr << "Failed to configure camera" << std::endl;
        return -1;
    }

    // フレームバッファのアロケータを作成
    FrameBufferAllocator allocator(camera);
    for (StreamConfiguration &cfg : *config) {
        if (allocator.allocate(cfg.stream()) < 0) {
            std::cerr << "Failed to allocate buffers" << std::endl;
            return -1;
        }
    }

    // リクエストを作成してキューに追加
    std::vector<Request *> requests;
    for (unsigned int i = 0; i < 4; i++) {
        Request *request = camera->createRequest();
        if (!request) {
            std::cerr << "Failed to create request" << std::endl;
            return -1;
        }

        const std::vector<std::unique_ptr<FrameBuffer>> &buffers = allocator.buffers(config->at(0).stream());
        request->addBuffer(config->at(0).stream(), buffers[i].get());

        requests.push_back(request);
    }

    // カメラのスタート
    if (camera->start()) {
        std::cerr << "Failed to start camera" << std::endl;
        return -1;
    }

    // リクエストをサブミット
    for (Request *request : requests) {
        if (camera->queueRequest(request)) {
            std::cerr << "Failed to queue request" << std::endl;
            return -1;
        }
    }

    // キャプチャしたフレームを処理
    while (true) {
        std::unique_ptr<Request> request = camera->dequeueRequest();
        if (request->status() == Request::RequestComplete) {
            const FrameBuffer *buffer = request->buffers().begin()->second;
            std::ofstream outputFile("output.raw", std::ios::binary);
            outputFile.write(reinterpret_cast<const char *>(buffer->planes()[0].mem), buffer->planes()[0].length);
            outputFile.close();
            break;
        }
    }

    // カメラの停止
    camera->stop();

    // カメラの解放
    camera->release();

    // カメラマネージャーの停止
    cm.stop();

    return 0;
}