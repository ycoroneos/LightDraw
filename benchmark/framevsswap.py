#!/usr/bin/python

import os
import sys
import matplotlib.pyplot as plt

def print_data(filename):
    frametimes=list()
    swaptimes=list()
    with open(filename, 'r') as f:
        for line in f:
            sp=line.split()
            if (len(sp)==0):
                continue
            frametimes.append(float(sp[1]))
            swaptimes.append(float(sp[3]))
    avg_frame = sum(frametimes)/len(frametimes)
    avg_swap=sum(swaptimes)/len(swaptimes)
    print 'file %s:' % (filename)
    print 'avg frametime %f' % (avg_frame)
    print 'avg swaptime %f' % (avg_swap)
    print 'swaptime is %f%% of loop' % (100.0 * (avg_swap/(avg_swap+avg_frame)))
    print ''


if (__name__=="__main__"):
    print_data('196lights_frametime_vs_swaptime_lidr_noshadow_1024x768.txt')
    print_data('196lights_frametime_vs_swaptime_lidr_noshadow_1024x768_withflush.txt')
    print_data('196lights_frametime_vs_swaptime_lidr_noshadow_3840x2160.txt')
    print_data('196lights_frametime_vs_swaptime_lidr_noshadow_3840x2160_withflush.txt')
