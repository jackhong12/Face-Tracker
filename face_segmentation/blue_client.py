import bluetooth
import math
from time import sleep

bd_addr = "98:D3:41:F9:4A:44"  # here need to modify. use hciconfig to get address
port = 1

sock=bluetooth.BluetoothSocket( bluetooth.RFCOMM )
sock.connect((bd_addr, port))

time = 0
gain = 1
while (1):
    time += gain 
    if time >= 90:
        gain = -1
    if time <= -90:
        gain = 1
    print(time)
    sock.send(str(time) + "&")
    sleep(0.03)

sock.close()
