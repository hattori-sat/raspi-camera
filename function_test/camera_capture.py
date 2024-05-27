import os
# from picamera import PiCamera
from time import sleep
PiCamera=1
# フォルダ名
folder_name = "test_images"

# フォルダの作成
if not os.path.exists(folder_name):
    os.makedirs(folder_name)

# カメラの初期化
camera = PiCamera()

# 写真を撮影して保存
try:
    # カメラの設定
    camera.resolution = (1024, 768)
    
    # 撮影前の待機
    sleep(2)  # カメラの初期化に時間がかかる場合があるので、少し待つ

    # 撮影
    file_path = os.path.join(folder_name, "image.jpg")
    camera.capture(file_path)
    print(f"写真が保存されました: {file_path}")

finally:
    # カメラの解放
    camera.close()
