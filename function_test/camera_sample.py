from picamera2 import Picamera2

# Picamera2のインスタンスを作成
picam2 = Picamera2()

# カメラの設定を取得
config = picam2.create_still_configuration()

# 設定をカメラに適用
picam2.configure(config)

# カメラの起動
picam2.start()

# 写真を撮影して保存
picam2.capture_file("./test_image.jpg")

# カメラの停止
picam2.stop()
