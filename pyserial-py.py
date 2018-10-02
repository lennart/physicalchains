import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import serial

from collections import deque

fig, ax = plt.subplots()
line, = ax.plot(np.random.rand(10))
ax.set_ylim(-10, 0)
xdata, ydata = deque([0]*100), deque([0]*100)
raw = serial.Serial("/dev/ttyUSB2",9600)
if raw.is_open:
    raw.close()
raw.open()

#plt.ion()

# def update(data):
#     line.set_ydata(data)
#     return line,

def run(data):
    t,y = data
    #del xdata[0]
    xdata.popleft()
    #del ydata[0]
    ydata.popleft()
    xdata.append(t)
    ydata.append(y)
    print(y)
    line.set_data(xdata, ydata)
    return line,

def data_gen():
    t = 0
    while True:
        t = (t + 0.1) % 9
        try:
            dat = float(raw.readline())
        except:
            dat = 0
        yield t, dat

ani = animation.FuncAnimation(fig, run, data_gen, interval=0, blit=True)
#plt.axis([0,9,10,-10.0])
plt.show(block=True)
