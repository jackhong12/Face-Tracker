# 錄影輔助工具
透過辨識使用者的位置，自動控制手機的角度，使得使用者維持在錄影畫面的正中間。
![demo](./img/demo.gif)

## 使用到的裝置
- PIC18F4520
- L298N (馬達驅動模組)
- TC4013BP (D-type Flip Flop)
- TC4069UBP (Not Gate)
- TTL 線
- DC 馬達
- 雙軌 encoder

## 執行方式
1. 利用 mplab 執行 `./pic/motor_control.c`

2. 執行 face detect，並把位置資訊傳給 PIC18F4520
    ```shell
    $ python ./face_detect/tracking.py
    ```

## 使用到的 library
- python
    1. OpenCV2
    2. [face_recognition](https://github.com/ageitgey/face_recognition)
    3. pySerial

## Usb permission
```shell
$ sudo chmod 777 /dev/ttyUSB0
```
