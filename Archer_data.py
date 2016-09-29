# -*- coding: utf-8 -*-
"""
Created on Thu Jul 14 10:26:32 2016

@author: Michael
"""


import numpy as np
from matplotlib import cm
import matplotlib.pyplot as plt
import socket
from time import sleep
import re

message_a1 = 'WP 1 2000.00 2000.00'
message_a1 = 'WP 5 1688.25 667.54 2720.16 1280.30 2464.90 2452.84 1294.06 2715.76 675.93 1687.04'

TCP_IP = '35.2.37.146'
#TCP_PORT1 = 9998
TCP_PORT = 9999
BUFFER_SIZE = 512
MESSAGE = "Hello, World!"

socket.setdefaulttimeout(30)
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#s.bind((TCP_IP, TCP_PORT))
#s.listen(3)
s.connect((TCP_IP, TCP_PORT))
#conn, addr = s.accept()
currentData = np.array([])
xData = np.array([])
yData = np.array([])
hData = np.array([])
sData = np.array([])
rData = np.array([])
#s.send(message_a1)
try:
    while True:
        
        #print 'Connection address:', addr
        data = s.recv(BUFFER_SIZE)
        if (data.find('WP')>=0):
            s.send(message_a1)
            print "sent wp:", message_a1
        #sleep(0.5)
        elif (data.find('Current')==0):
            print "received data:", data
            Data = re.findall( r'\d+\.*\d*', data)
            if Data[0]:
                currentData = np.append(currentData, float(Data[0]))
                xData = np.append(xData, float(Data[1]))
                yData = np.append(yData, float(Data[2]))
                hData = np.append(hData, float(Data[3]))
                sData = np.append(sData, float(Data[4]))
                rData = np.append(rData, float(Data[5]))
                
        #conn.send(MESSAGE)  # echo
        
        # = np.append(speedData, Data[3])
except KeyboardInterrupt:
    pass
s.close()

DATA = np.array([currentData, xData, yData, hData, sData, rData])

"""
N = currentData.size
samples = np.arange(1, N+1)
plt.plot(samples, currentData)
plt.xlabel('Samples')
plt.ylabel('Current from Battery')

N = currentData.size
samples = np.arange(1, N+1)
plt.plot(xData, yData, 'ro')
plt.xlabel('X Position')
plt.ylabel('Y Position')
plt.axis([0, 3000, 0, 3000])
plt.show()

np.savetxt('testwp1.txt', DATA)
"""