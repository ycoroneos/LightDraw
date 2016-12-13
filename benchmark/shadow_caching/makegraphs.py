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
    render      = ['lidr']
    shadow      = ['shadow', 'noshadow', 'shadowcache']
    resolution  = ['3840x2160']

    plt.figure(0)
    lidr_noshadow = [map(lambda x: float(x), lights), map(lambda x:get_data(resolution[0], x, 'lidr', 'noshadow'), lights)]
    lidr_shadow = [map(lambda x: float(x), lights), map(lambda x:get_data(resolution[0], x, 'lidr', 'shadow'), lights)]
    lidr_shadowcache = [map(lambda x: float(x), lights), map(lambda x:get_data(resolution[0], x, 'lidr', 'shadowcache'), lights)]
    plt.plot(lidr_noshadow[0], lidr_noshadow[1], 'b.-', label='lidr no shadow no vsync')
    plt.plot(lidr_shadowcache[0], lidr_shadowcache[1], 'g.-', label='lidr shadowcache no vsync')
    plt.plot(lidr_shadow[0], lidr_shadow[1], 'r.-', label='lidr shadow no vsync')
    plt.legend()
    plt.xlabel('# lights')
    plt.ylabel('FPS')
    plt.title(resolution[0]+', 1 dynamic shadow light')
    plt.axis([0, 256, 0, 200])
    plt.show()
