import pandas as pd
import matplotlib.pyplot as plt
import sys

data = pd.read_csv(sys.argv[1], sep=' ', names=['second', 'velocity'])

plt.plot('second', 'velocity', data=data)
plt.xlabel("second")
plt.ylabel("velocity")
plt.legend()
plt.grid()
plt.savefig(sys.argv[1] + ".png", dpi=300, format="png")
plt.show()
