import pandas as pd
import matplotlib.pyplot as plt
import sys

data_1023 = pd.read_csv('pwm_1023', sep=' ', names=['second', '5 V'])
data_900 = pd.read_csv('pwm_900', sep=' ', names=['second', '4.4 V'])
data_450 = pd.read_csv('pwm_450', sep=' ', names=['second', '2.2 V'])
data_300 = pd.read_csv('pwm_300', sep=' ', names=['second', '1.47 V'])

plt.plot('second', '5 V', data=data_1023)
plt.plot('second', '4.4 V', data=data_900)
plt.plot('second', '2.2 V', data=data_450)
plt.plot('second', '1.47 V', data=data_300)
plt.xlabel("second")
plt.ylabel("velocity (deg/s)")
plt.axis([-0.1, 13, -5, 220])
plt.legend()
plt.grid()
plt.savefig("motor_test_result.png", dpi=300, format="png")
plt.show()
