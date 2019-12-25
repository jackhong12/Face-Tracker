import face_recognition
import cv2
import numpy as np
import math
from time import sleep
import serial
video = cv2.VideoCapture(0)
scale = 1 

port = 1


with serial.Serial('/dev/ttyUSB0', 19200) as ser:
    while True:
        ret, frame = video.read()
        if frame is None:
            print('webcam open error')
            break
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
            pwm = position / 60
            value = int(pwm)
            print(value)

            upper = ((value >> 14) & 0x0003).to_bytes(1, byteorder='big')
            middle = ((value >> 7) & 0x007F).to_bytes(1, byteorder='big')
            low = ((value & 0x00FF) | 0x0080).to_bytes(1, byteorder='big')
            ser.write(upper)
            ser.write(middle)
            ser.write(low)

        else:
            value = 0
            upper = ((value >> 14) & 0x0003).to_bytes(1, byteorder='big')
            middle = ((value >> 7) & 0x007F).to_bytes(1, byteorder='big')
            low = ((value & 0x00FF) | 0x0080).to_bytes(1, byteorder='big')
            ser.write(upper)
            ser.write(middle)
            ser.write(low)
        
        cv2.imshow('webcam', frame)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

video.release()
cv2.destroyAllWindows()
