#include <iostream>
#include <raspicam/raspicam.h>

int main(int argc, char **argv) {
    raspicam::RaspiCam Camera;

    // カメラを初期化する
    if (!Camera.open()) {
        std::cerr << "Error opening camera" << std::endl;
        return -1;
    }

    // カメラを起動する
    Camera.grab();

    // 画像データを保存するためのバッファ
    unsigned char *data = new unsigned char[Camera.getImageTypeSize(raspicam::RASPICAM_FORMAT_RGB)];

    // 画像を取得する
    Camera.retrieve(data, raspicam::RASPICAM_FORMAT_RGB);

    // 画像をファイルに保存する (例: image.jpg)
    std::ofstream outFile("image.jpg", std::ios::binary);
    outFile.write(reinterpret_cast<char*>(data), Camera.getImageTypeSize(raspicam::RASPICAM_FORMAT_RGB));
    outFile.close();

    // メモリを解放する
    delete[] data;

    // カメラを閉じる
    Camera.release();

    return 0;
}