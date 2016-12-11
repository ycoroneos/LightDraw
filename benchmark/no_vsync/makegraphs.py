#!/usr/bin/python

import os
import sys
import matplotlib.pyplot as plt

def get_data(resolution, lights, renderer, shadows):
    pattern     = '%slights_%s_%s_%s.txt'
    filename = pattern % (lights, renderer, shadows, resolution)
    with open(filename, 'r') as f:
        data = map(lambda x: float(x.split()[-2]), f.readlines()[1:-1])
    return sum(data)/len(data)

if (__name__=="__main__"):
    lights      = ['5', '15', '30', '60', '120', '196']
    render      = ['lidr', 'forward']
    shadow      = ['shadow', 'noshadow']
    resolution  = ['1024x768', '1920x1080', '3840x2160']

    for i in range(3):
        plt.figure(i)
        lidr_noshadow = [map(lambda x: float(x), lights), map(lambda x:get_data(resolution[i], x, 'lidr', 'noshadow'), lights)]
        lidr_shadow = [map(lambda x: float(x), lights), map(lambda x:get_data(resolution[i], x, 'lidr', 'shadow'), lights)]
        forward_noshadow = [map(lambda x: float(x), lights), map(lambda x:get_data(resolution[i], x, 'forward', 'noshadow'), lights)]
        forward_shadow = [map(lambda x: float(x), lights), map(lambda x:get_data(resolution[i], x, 'forward', 'shadow'), lights)]
        plt.plot(lidr_noshadow[0], lidr_noshadow[1], 'b.-', label='lidr no shadow')
        plt.plot(lidr_shadow[0], lidr_shadow[1], 'g.-', label='lidr shadow')
        plt.plot(forward_noshadow[0], forward_noshadow[1], 'r.-', label='forward no shadow')
        plt.plot(forward_shadow[0], forward_shadow[1], 'c.-', label='forward shadow')
        plt.legend()
        plt.xlabel('# lights')
        plt.ylabel('FPS')
        plt.title(resolution[i])
        plt.axis([0, 256, 0, 500])
    plt.show()
