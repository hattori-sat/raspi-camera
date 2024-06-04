# raspi-camera

## ※追記事項
raspberrypi4は過去のものと比べて変化点が多く見受けられた．

- configの位置が **/boot/firmware/config.txt** に変更された
- ```sudo vim /boot/firmware/config.txt``` で編集可能（vim -> nanoでも可）
- ラズパイ4で純正カメラの設定がない可能性がある : [設定](https://www.raspberrypi.com/documentation/computers/camera_software.html#configuration)
- [no camera availableの罠](https://forums.raspberrypi.com/viewtopic.php?t=358884)
- [カメラの確認方法が変わった（変化追い切れていない）](https://qiita.com/yutaka_m/items/019d05a19b206cbcf571)

### この辺りで遊ぶとよい
- とりあえず,capture_60hz.py実行してみてダメだったら教えてほしい
- [参考1](https://sozorablog.com/camera-module-v3/)
- [LED 参考](https://tech-and-investment.com/raspberrypi2-2-pwm/)

## ラズパイの操作
色んな操作方法がある．  
おそらく最も簡単なのは，USBキーボードとマウス，そしてHDMIでディスプレイを繋げてPCのように操作する方法である．
しかし，そのような環境がなくても同じwi-fiに繋げていればwindowsやmacなどからも **ssh通信** することが出来る．sshで繋げることが出来れば，そのあとは **VNC Viewer** などを使ってGUI（PCのようにグラフィックに）操作すことが出来る．  
おそらく，これらのキーワードを知っておけばネットで調べて進めることが可能であると思われる．初学者のセットアップは非常に大変だと思われるが，勉強になる．

## raspiのセットアップと主要なLinuxコマンド
RaspberryPiを使うためには，SDカードにOSをインストールする必要がある．  
Raspberry Pi Imagerなど使うとインストールしやすい．

OSはrasbianなどのLinux環境である．なので，Linuxの基本的なコマンドを覚える必要がある．

ラズパイでは基本的にCUIという黒い画面（Windowsでいうターミナル）のようなものを操作することに留意する．

基本的なコマンド

- ディレクトリ（フォルダ）を移動する
```
cd directory_name
```

- ディレクトリを作る
```
mkdir directory_name
```

- ラズパイのアップデート
```
sudo apt update
```

- ラズパイのアップグレード（アップデート後にする）
```
sudo apt upgrade
```

- Githubのリポジトリ（フォルダ）をローカルに保存（ちょっと違うけど）
```
git clone https://github.com/hattori-sat/raspi-camera.git
```
ただし，URLはGithubのWebサイトから取得してくる．

---- 

## pythonのセットアップ
今回使うのはラズパイカメラとLEDである．LEDの操作は何もしなくてもできる（=GPIO操作）ので，ラズパイカメラを使えるようにする必要がある．

基本的には，**picamera** と **cv2** が必要である．
- picameraはラズパイのカメラを使って撮像する
- cv2はopencvというもので画像処理など様々なことが出来る

今回の環境で実行したのは以下それぞれである．なお，参考は[こちら](https://fclout.hateblo.jp/entry/2023/05/06/OpenCV%E3%82%92%E3%82%A4%E3%83%B3%E3%82%B9%E3%83%88%E3%83%BC%E3%83%AB%E3%81%99%E3%82%8B)（見なくても良い）．

```bash
sudo apt update
sudo apt upgrade -y
sudo apt install -y libopencv-dev python3-opencv
```

## memo
コマンド例

```bash
hattori@raspi-hattori:~ $ ls
Bookshelf  Desktop  Documents  Downloads  Music  Pictures  Public  Templates  Videos
hattori@raspi-hattori:~ $ cd Documents
hattori@raspi-hattori:~/Documents $ ls
process_image_light
hattori@raspi-hattori:~/Documents $ ls process_image_light/
sample2.py  sample.py
hattori@raspi-hattori:~/Documents $ git clone https://github.com/hattori-sat/raspi-camera.git
Cloning into 'raspi-camera'...
remote: Enumerating objects: 19, done.
remote: Counting objects: 100% (19/19), done.
remote: Compressing objects: 100% (14/14), done.
import os
remote: Total 19 (delta 3), reused 14 (delta 1), pack-reused 0
Receiving objects: 100% (19/19), 5.28 KiB | 540.00 KiB/s, done.
Resolving deltas: 100% (3/3), done.
hattori@raspi-hattori:~/Documents $ ls
process_image_light  raspi-camera
hattori@raspi-hattori:~/Documents $ cd raspi-camera/
hattori@raspi-hattori:~/Documents/raspi-camera $ ls
function_test  README.md
hattori@raspi-hattori:~/Documents/raspi-camera $ vim
hattori@raspi-hattori:~/Documents/raspi-camera $ ls
function_test  README.md
hattori@raspi-hattori:~/Documents/raspi-camera $ cd function_test/
hattori@raspi-hattori:~/Documents/raspi-camera/function_test $ ls
camera_capture.py
hattori@raspi-hattori:~/Documents/raspi-camera/function_test $ vim camera_capture.py
hattori@raspi-hattori:~/Documents/raspi-camera/function_test $ python camera_capture.py
```