from picamera2 import Picamera2, Preview
from picamera2.encoders import H264Encoder, Quality
import cv2
import os
import time

def record_video(filename, duration, fps=60):
    """
    Record a video using the Raspberry Pi Camera Module.

    Parameters:
    filename (str): The filename to save the video.
    duration (int): The duration of the video in seconds.
    fps (int): Frames per second for the video.
    """
    picam2 = Picamera2()
    video_config = picam2.create_video_configuration({"size": (640, 480)})
    picam2.configure(video_config)
    picam2.start_preview(Preview.QTGL)

    picam2.set_controls({"FrameDurationLimits": (int(1e6 / fps), int(1e6 / fps))})
    encoder = H264Encoder(bitrate=10000000)
    picam2.start_recording(encoder, filename)
    time.sleep(duration)
    picam2.stop_recording()
    picam2.stop_preview()
    picam2.close()

def extract_frames(video_file, output_dir, fps=60):
    """
    Extract frames from a video file and save them as images.

    Parameters:
    video_file (str): The path to the video file.
    output_dir (str): The directory to save the extracted frames.
    fps (int): Frames per second for extracting frames.
    """
    os.makedirs(output_dir, exist_ok=True)

    vidcap = cv2.VideoCapture(video_file)
    if not vidcap.isOpened():
        print(f"Error opening video file: {video_file}")
        return

    frame_rate = vidcap.get(cv2.CAP_PROP_FPS)
    if frame_rate == 0:
        print("Error: Frame rate of video is zero.")
        return

    frame_interval = int(frame_rate / fps)
    success, image = vidcap.read()
    count = 0
    frame_count = 0

    while success:
        if count % frame_interval == 0:
            filename = os.path.join(output_dir, f"frame_{frame_count + 1}.jpg")
            cv2.imwrite(filename, image)
            frame_count += 1
        success, image = vidcap.read()
        count += 1

    vidcap.release()

if __name__ == "__main__":
    # 保存する画像のディレクトリ
    output_dir = "./movie2images/"
    # 動画ファイルのパス
    video_file = os.path.join(output_dir, "video.h264")

    # フォルダの作成
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    # 動画を撮影
    record_video(video_file, duration=1)  # 1秒の動画を撮影

    # 動画からフレームを抽出
    extract_frames(video_file, output_dir)
