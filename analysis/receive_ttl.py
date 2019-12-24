import serial
from datetime import datetime
import time
start_time = time.time()
with serial.Serial('/dev/ttyUSB0', 19200) as ser:
    while True:
        line = ser.readline()
        now_time = time.time()
        second = now_time - start_time 
        print(str(second) + " " + line.strip())
        ser.write(line.upper() + '\n')
