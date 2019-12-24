import pandas as pd
import matplotlib.pyplot as plt
import sys

# s / 616 * 360 / (0.00526 * 11)
data_1 = pd.read_csv('tune_p128_i2_s1.data', sep=' ', names=['second', '10 deg/s'])
data_5 = pd.read_csv('pi_s5.data', sep=' ', names=['second', '50 deg/s'])
data_10 = pd.read_csv('pi_s10.data', sep=' ', names=['second', '101 deg/s'])
data_15 = pd.read_csv('pi_s15.data', sep=' ', names=['second', '151 deg/s'])
data_20 = pd.read_csv('pi_s20.data', sep=' ', names=['second', '201 deg/s'])


plt.plot('second', '10 deg/s', data=data_1)
plt.plot('second', '50 deg/s', data=data_5)
plt.plot('second', '101 deg/s', data=data_10)
plt.plot('second', '151 deg/s', data=data_15)
plt.plot('second', '201 deg/s', data=data_20)

plt.xlabel("second")
plt.ylabel("velocity (deg/s)")
plt.axis([-0.1, 13, -5, 220])
plt.legend()
plt.grid()
plt.savefig("velocity_control_result.png", dpi=300, format="png", figsize=(800, 800))
plt.show()
