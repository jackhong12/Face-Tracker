import pandas as pd
import matplotlib.pyplot as plt
import sys

data = pd.read_csv(sys.argv[1], sep=' ', names=['second', 'velocity'])

data = data - [data['second'][0], 0]

time_gap = (data['second'][data.shape[0] -1] - data['second'][0]) / (data.shape[0] -1)

for i in range(data.shape[0]):
    second = data['second'][i]
    velocity = data['velocity'][i] / 616 * 360 / (time_gap * 50) 
    print(str(second) + " " + str(velocity))
print(time_gap)

