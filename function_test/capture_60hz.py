from picamera2 import Picamera2
import time
import cv2
import numpy as np
import os

def capture_images(fps=60, num_images=60):
    """
    Capture images from the Raspberry Pi Camera Module at a specified frame rate (fps) and save them to a directory.

    Parameters:
    fps (int): Frames per second to capture.
    num_images (int): Number of images to capture.

    The captured images will be saved in the '/home/pi/images_60hz/' directory.
    """
    # カメラの設定
    picam2 = Picamera2()
    config = picam2.create_still_configuration(main={"size": (640, 480)})
    picam2.configure(config)
    picam2.start()
    
    # 保存先ディレクトリ
    save_dir = '/home/pi/images_60hz/'
    os.makedirs(save_dir, exist_ok=True)
    
    # 画像キャプチャ用のバッファ設定
    buffer = np.empty((480, 640, 3), dtype=np.uint8)
    
    start_time = time.time()
    
    for i in range(num_images):
        capture_start_time = time.time()
        
        # 画像をキャプチャ
        buffer = picam2.capture_array()
        
        # OpenCVで画像を保存
        filename = f"{save_dir}image_{i+1}.jpg"
        cv2.imwrite(filename, buffer)
        
        # 1秒間に撮影するフレーム数に合わせてスリープ
        elapsed_time = time.time() - capture_start_time
        sleep_time = max(0, (1 / fps) - elapsed_time)
        time.sleep(sleep_time)
    
    end_time = time.time()
    print(f"Captured {num_images} images in {end_time - start_time} seconds.")
    
    picam2.stop()

if __name__ == "__main__":
    # 画像を撮影して保存する
    capture_images()