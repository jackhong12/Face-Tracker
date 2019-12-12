import face_recognition
import cv2
import numpy as np
import bluetooth
import math
from time import sleep

video = cv2.VideoCapture(1)
scale = 1 

print("Start connect to HC-06")
bd_addr = "98:D3:41:F9:4A:44"  # here need to modify. use hciconfig to get address
port = 1
sock=bluetooth.BluetoothSocket( bluetooth.RFCOMM )
sock.connect((bd_addr, port))
print("Success to connect HC-06")

while True:
    ret, frame = video.read()
    height, width, channels = frame.shape
    small_frame = cv2.resize(frame, (0, 0), fx=1/scale, fy=1/scale)
    
    rgb_frame = small_frame[:, :, ::-1]
    face_locations = face_recognition.face_locations(rgb_frame)
    
    isDetect = False
    middle = 0
    
    for top, right, bottom, left in face_locations:
        cv2.rectangle(frame, (scale * left, scale * top), (scale * right, scale * bottom), (0, 0, 255), 2)
        middle = (right + left) * scale / 2
        isDetect = True

    if isDetect:
        position = middle - width / 2
        print(position)
        pwm = - 2 * position
        pwm = int(pwm)
        sock.send(str(pwm) + "&")
    else:
        sock.send(str(0) + "&")
    
    cv2.imshow('webcam', frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

video.release()
cv2.destroyAllWindows()
sock.close
