#include <iostream>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <libcamera/libcamera.h>
#include <libcamera/camera_manager.h>
#include <libcamera/camera.h>
#include <libcamera/framebuffer_allocator.h>
#include <opencv2/opencv.hpp>

using namespace libcamera;
using namespace std;

class CameraCapture {
public:
    CameraCapture() : cm_(new CameraManager()) {}

    bool initialize() {
        cm_->start();

        if (cm_->cameras().empty()) {
            cerr << "カメラが見つかりません" << endl;
            return false;
        }

        camera_ = cm_->cameras()[0];
        camera_->acquire();

        config_ = camera_->generateConfiguration({ StreamRole::StillCapture });
        if (config_->validate() == CameraConfiguration::Invalid) {
            cerr << "カメラの設定が無効です" << endl;
            return false;
        }

        StreamConfiguration &streamConfig = config_->at(0);
        streamConfig.size.width = 640;
        streamConfig.size.height = 480;
        streamConfig.pixelFormat = formats::BGR888;

        if (config_->validate() == CameraConfiguration::Invalid) {
            cerr << "カメラの設定が無効です" << endl;
            return false;
        }

        camera_->configure(config_.get());

        allocator_ = make_unique<FrameBufferAllocator>(camera_);
        for (StreamConfiguration &cfg : *config_) {
            allocator_->allocate(cfg.stream());
        }

        stream_ = streamConfig.stream();

        for (const unique_ptr<FrameBuffer> &buffer : allocator_->buffers(stream_)) {
            unique_ptr<Request> request = camera_->createRequest();
            request->addBuffer(stream_, buffer.get());
            requests_.push_back(move(request));
        }

        camera_->requestCompleted.connect(this, &CameraCapture::requestComplete);

        return true;
    }

    bool capture() {
        camera_->start();

        Request *request = requests_.front().get();
        request_done_ = false;
        camera_->queueRequest(request);

        unique_lock<mutex> lock(mutex_);
        condition_variable_.wait(lock, [&] { return request_done_; });

        const Request::BufferMap &buffers = request->buffers();
        auto it = buffers.find(stream_);
        if (it == buffers.end()) {
            cerr << "ストリームのバッファが見つかりません" << endl;
            return false;
        }

        const FrameBuffer *buffer = it->second;
        const FrameBuffer::Plane &plane = buffer->planes()[0];

        void *data = plane.fd->map();
        if (!data) {
            cerr << "バッファのメモリマッピングに失敗しました" << endl;
            return false;
        }

        cv::Mat img(cv::Size(640, 480), CV_8UC3, data, cv::Mat::AUTO_STEP);
        cv::imwrite("captured_image.jpg", img);
        cout << "画像を保存しました: captured_image.jpg" << endl;

        plane.fd->unmap(data);
        camera_->stop();
        camera_->release();

        return true;
    }

    void requestComplete(Request *request) {
        {
            lock_guard<mutex> lock(mutex_);
            request_done_ = true;
        }
        condition_variable_.notify_one();
    }

private:
    unique_ptr<CameraManager> cm_;
    shared_ptr<Camera> camera_;
    unique_ptr<CameraConfiguration> config_;
    unique_ptr<FrameBufferAllocator> allocator_;
    vector<unique_ptr<Request>> requests_;
    Stream *stream_;

    bool request_done_ = false;
    mutex mutex_;
    condition_variable condition_variable_;
};

int main() {
    CameraCapture camera_capture;

    if (!camera_capture.initialize()) {
        cerr << "カメラの初期化に失敗しました" << endl;
        return 1;
    }

    if (!camera_capture.capture()) {
        cerr << "画像のキャプチャに失敗しました" << endl;
        return 1;
    }

    return 0;
}