import os
from picamera2 import Picamera2
from picamera2.encoders import JpegEncoder
from threading import Thread
import queue
import time
 
# 画像を保存するための関数
def save_image(image_queue, save_dir):
    while True:
        item = image_queue.get()  # キューから画像データを取得
        if item is None:
            break
        index, image = item
        image_path = os.path.join(save_dir, f"image_{index}.jpg")
        with open(image_path, "wb") as img_file:
            img_file.write(image)
        image_queue.task_done()
 
# 保存ディレクトリの作成
save_dir = "images_60fps"
os.makedirs(save_dir, exist_ok=True)
 
# Picamera2のインスタンスを作成
picam2 = Picamera2()
 
# カメラの設定
config = picam2.create_still_configuration(main={"size": (1920, 1080), "format": "XRGB8888"})
picam2.configure(config)
 
# JPEGエンコーダの設定
jpeg_encoder = JpegEncoder(quality=85)
picam2.encoder = jpeg_encoder
 
# 画像保存用キュー
image_queue = queue.Queue()
 
# スレッドの設定と起動
thread = Thread(target=save_image, args=(image_queue, save_dir))
thread.start()
 
# カメラの起動
picam2.start()
 
try:
    num_frames = 300
    start_time = time.time()
    for i in range(num_frames):
        # 画像をキャプチャ
        buffer = picam2.capture_buffer()
        image_queue.put((i, buffer))
 
        # FPSを維持
        wait_time = (i + 1) / 60 - (time.time() - start_time)
        if wait_time > 0:
            time.sleep(wait_time)
 
finally:
    # キャプチャ終了
    picam2.stop()
    image_queue.put(None)  # スレッドを終了させるためにNoneをキューに入れる
    thread.join()  # スレッドの終了を待つ
 
print("撮影完了")