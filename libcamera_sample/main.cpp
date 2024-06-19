#include <libcamera/libcamera.h>
#include <libcamera/camera_manager.h>
#include <libcamera/framebuffer_allocator.h>
#include <libcamera/request.h>
#include <libcamera/version.h>
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <string.h>

using namespace libcamera;

class CameraApp {
public:
    CameraApp() : cm_(std::make_unique<CameraManager>()) {}
    
    bool initialize() {
        cm_->start();

        if (cm_->cameras().empty()) {
            std::cerr << "No cameras available" << std::endl;
            return false;
        }

        camera_ = cm_->get(cm_->cameras()[0]->id());
        if (camera_->acquire()) {
            std::cerr << "Failed to acquire camera" << std::endl;
            return false;
        }

        return true;
    }

    bool capture() {
        std::unique_ptr<CameraConfiguration> config = camera_->generateConfiguration({ StreamRole::StillCapture });

        if (!config || config->size() != 1) {
            std::cerr << "Failed to generate configuration" << std::endl;
            return false;
        }

        StreamConfiguration &streamConfig = config->at(0);
        streamConfig.pixelFormat = formats::YUV420;
        streamConfig.size.width = 640;
        streamConfig.size.height = 480;

        if (camera_->configure(config.get())) {
            std::cerr << "Failed to configure camera" << std::endl;
            return false;
        }

        FrameBufferAllocator allocator(camera_);
        for (StreamConfiguration &cfg : *config) {
            if (allocator.allocate(cfg.stream())) {
                std::cerr << "Failed to allocate buffers" << std::endl;
                return false;
            }
        }

        camera_->start();

        Request *request = camera_->createRequest();
        if (!request) {
            std::cerr << "Failed to create request" << std::endl;
            return false;
        }

        for (const auto &buffer : allocator.buffers(streamConfig.stream())) {
            if (request->addBuffer(streamConfig.stream(), buffer.get())) {
                std::cerr << "Failed to add buffer to request" << std::endl;
                return false;
            }
        }

        camera_->queueRequest(request);

        while (true) {
            CameraManager::Event *event = cm_->wait();
            if (event->type() == CameraManager::EventRequestComplete) {
                std::cerr << "Captured frame" << std::endl;
                saveFrame(allocator.buffers(streamConfig.stream()).front().get());
                break;
            }
        }

        camera_->stop();
        camera_->release();
        cm_->stop();

        return true;
    }

private:
    void saveFrame(FrameBuffer *buffer) {
        const FrameBuffer::Plane &plane = buffer->planes().front();
        std::ofstream file("image.raw", std::ios::binary);
        file.write(static_cast<char *>(plane.mem), plane.length);
    }

    std::unique_ptr<CameraManager> cm_;
    std::shared_ptr<Camera> camera_;
};

int main() {
    CameraApp app;
    if (!app.initialize()) {
        return -1;
    }

    if (!app.capture()) {
        return -1;
    }

    std::cout << "Image captured and saved as image.raw" << std::endl;
    return 0;
}