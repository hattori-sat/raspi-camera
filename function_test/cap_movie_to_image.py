from picamera2 import Picamera2, Preview
import time
import cv2

# 動画の保存パスとファイル名
video_path = 'video.h264'

# カメラの設定
picam2 = Picamera2()
camera_config = picam2.create_video_configuration(main={"size": (640, 480), "format": "H264"})
picam2.configure(camera_config)
picam2.start()

# 録画開始
picam2.start_recording(video_path)
time.sleep(1)  # 1秒間録画
picam2.stop_recording()

# カメラの停止
picam2.stop()

# 動画から画像を取得する
cap = cv2.VideoCapture(video_path)
frame_count = 0
while cap.isOpened() and frame_count < 60:  # 60枚取得するまで
    ret, frame = cap.read()
    if not ret:
        break
    frame_count += 1
    # 画像を保存する
    image_path = 'image_' + str(frame_count) + '.jpg'
    cv2.imwrite(image_path, frame)

cap.release()
