#include <libcamera/libcamera.h>
#include <iostream>
using namespace libcamera;

int main(){
    CameraManager cm;
    cm.start();
    if (cm.cameras().empty()){
        std::cerr << "No cameras available" << std::endl;
        return -1;
    }
    std::shared_ptr<Camera> camera = cm.cameras()[0];
    if (camera->acquire()){
        std::cerr << "Failed to acquire camera" << std::endl;
        return -1;
    }
    std::cout << "Camera acquired: " << camera->id() << std::endl;

    // ここでカメラの設定やキャプチャ処理を追加します
    camera->release();
    cm.stop();
    return 0;
}