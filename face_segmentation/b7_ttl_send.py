import serial
from datetime import datetime
import time
import numpy as np

value = np.int16(0) 
start_time = time.time()
with serial.Serial('/dev/ttyUSB0', 19200) as ser:
    while True:
        value = input()
        ser.write(chr(((value >> 14) & 0x0003)))
        ser.write(chr(((value >> 7) & 0x007F)))
        ser.write(chr(((value & 0x00FF) | 0x0080)))
