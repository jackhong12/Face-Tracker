import face_recognition
import cv2
import numpy as np

video = cv2.VideoCapture(0)
scale = 1 

while True:
    ret, frame = video.read()
    small_frame = cv2.resize(frame, (0, 0), fx=1/scale, fy=1/scale)
    
    rgb_frame = small_frame[:, :, ::-1]
    face_locations = face_recognition.face_locations(rgb_frame)

    for top, right, bottom, left in face_locations:
        cv2.rectangle(frame, (scale * left, scale * top), (scale * right, scale * bottom), (0, 0, 255), 2)
    cv2.imshow('webcam', frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

video.release()
cv2.destroyAllWindows()
