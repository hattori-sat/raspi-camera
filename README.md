# raspi-camera
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