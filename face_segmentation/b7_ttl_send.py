import serial
from datetime import datetime
import time
import numpy as np

value = np.int16(0) 
start_time = time.time()
with serial.Serial('/dev/ttyUSB0', 19200) as ser:
    while True:
        value = input()
        value = int(value) 
        upper = ((value >> 14) & 0x0003).to_bytes(1, byteorder='big')
        middle = ((value >> 7) & 0x007F).to_bytes(1, byteorder='big')
        low = ((value & 0x00FF) | 0x0080).to_bytes(1, byteorder='big')
        ser.write(upper)
        ser.write(middle)
        ser.write(low)
