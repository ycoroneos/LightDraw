#!/usr/bin/python

import os
import sys
import matplotlib.pyplot as plt

class Recorder:
    def __init__(self):
        self.res=dict()
        self.lights=dict()
        self.render=dict()
        self.shadow=dict()
        self.fps=dict()
    def add(self, res, lights, render, shadow, fps):
        pass

def get_data(filename):
    data=list()
    with open(filename, 'r') as f:
        data = map(lambda x: float(x.split()[-2]), f.readlines()[1:-1])
    print data
    return sum(data)/len(data)

if (__name__=="__main__"):
    lights      = ['5', '15', '30', '60', '120', '196']
    render      = ['lidr', 'forward']
    shadow      = ['shadow', 'noshadow']
    resolution  = ['1024x768', '1920x1080', '3840x2160']
    pattern     = '%slights_%s_%s_%s.txt'

    #each resolution has its own graph
    for res in resolution:
        xdata=list()
        ydata=list()
        for l in lights:
            #lights are on x axis
            for rend in render:
                for s in shadow:
                    filename = pattern % (l, rend, s, res)
                    avg_fps = get_data(filename)
                    #plt.plot([float(l)], [avg_fps], 'ro')
                    ydata.append(avg_fps)
                    print "%s %s %s %s -> %d" % (res, l, rend, s, avg_fps)
                plt.plot(map(lambda x: float(x), lights), ydata)
                ydata.clear()
        #graph
        #plt.plot(xdata, ydata, 'ro')
        #reset
        #ydata.clear()
        #xdata.clear()
    plt.show()
