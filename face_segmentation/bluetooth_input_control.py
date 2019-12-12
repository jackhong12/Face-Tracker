import bluetooth
import math
from time import sleep

bd_addr = "98:D3:41:F9:4A:44"  # here need to modify. use hciconfig to get address
port = 1

sock=bluetooth.BluetoothSocket( bluetooth.RFCOMM )
sock.connect((bd_addr, port))

while (1):
    pwm = input()
    sock.send(str(pwm) + "&")
    sleep(0.03)

sock.close()
