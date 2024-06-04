import os
from picamera2 import Picamera2, Preview
import time
import numpy as np
from PIL import Image

# フォルダの作成
if not os.path.exists('imgs_60hz'):
    os.makedirs('imgs_60hz')

# Picamera2のインスタンスを作成
picam2 = Picamera2()
picam2.start_preview(Preview.NULL)  # プレビューを表示しない設定
picam2.configure(picam2.create_still_configuration())

# カメラの初期化を待機
time.sleep(2)

# 1秒間に60枚の画像を取得
images = []
start_time = time.time()

for _ in range(60):
    image = picam2.capture_array()
    images.append(image)
    time.sleep(1/60)  # 60Hzの周期に合わせて待機

elapsed_time = time.time() - start_time
print(f"Captured 60 images in {elapsed_time:.2f} seconds")

# 画像をファイルに保存
for i, img in enumerate(images):
    img = Image.fromarray(img)
    img.save(f"imgs/image_{i:02d}.jpg")

print("Images have been saved in the 'imgs' folder.")
