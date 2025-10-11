"""
Reading raw data from /dev/ttyACM0 port for graphing purposes.
- Franco H
"""


import time
import serial
import matplotlib.pyplot as plt
import numpy as np


ser = serial.Serial(port='/dev/ttyACM0', baudrate=38400, timeout=1)

plt.ion()
fig,ax = plt.subplots()
ax.set_title("MAX30102 Sensor Data")
ax.set_xlabel("Samples")
ax.set_ylabel("Magnitude")

filteredOutData, rawData= [], []
xData = []


line1, = ax.plot(xData, filteredOutData)
line2, = ax.plot(xData, rawData)

ax.set_ylim(bottom=10000.0, top = 11500.0)
inc=0

while True:
    try:
        data = ser.readline().decode().strip().split(',')   
        floatData = [float(x) for x in data]

        inc+=1
        if inc == 500:
            inc = 0
            xData.clear()
            filteredOutData.clear()
            rawData.clear()
        xData.append(inc)



        filteredOutData.append(floatData[0])
        rawData.append(floatData[1])

        line1.set_data(xData, filteredOutData)
        line2.set_data(xData, rawData)
        ax.relim()
        ax.autoscale_view(tight=True, scalex=True, scaley=True)

        fig.canvas.draw()
        fig.canvas.flush_events()

    except KeyboardInterrupt:
        ser.close()
        break
        


