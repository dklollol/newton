import sys
import numpy as np
import matplotlib.pyplot as plt
import math as math

def SD(arr):
    mean = np.mean(arr)
    sose = 0.0
    for sample in arr:
        sose += (sample-mean)**2
    return mean, math.sqrt(sose/len(arr))
def main(filename):
    readData = open(filename, "r").read()
    dataList = readData.splitlines()
    sampleData = []
    indexes = []
    count = 0
    for x in dataList:
        if len(x.split()) == 2: # check if "cm thus an index"
            temp = x.split()
            indexes.append(int(temp[0]))
            continue
        y = x.split()
        for sample in y:
            if sample == "inf" or float(sample) > (indexes[count]*1.4) or float(sample) < (indexes[count]*0.6):
                continue
            sampleData.append(float(sample))
        count += 1
    meanSDarray = [(0,0) for y in xrange(len(indexes))]
    count= 0
    for var in meanSDarray:
        meanSDarray[count]= SD(sampleData[count*10:(count+1)*10])
        count += 1
    print len(meanSDarray)
    realDistance = []
    count = 0
    maxindex = max(indexes)
    diffarr = np.diff(indexes)
    for dist in indexes:
        for t in xrange(10):
            if len(realDistance) == len(sampleData):
                continue
            if count == 0:
                realDistance.append(maxindex)
                continue
            realDistance.append(maxindex)
        maxindex += diffarr[count-1]
        count += 1
    a, b = np.polyfit(sampleData, realDistance, 1)
    print "Fit for test: "+str(a) + "*x + " + str(b)
    fit_fn_sample = np.poly1d((a, b))
    # fit_fn is now a function which takes in x and returns an estimate for y
    count = 0
    maxindex = max(indexes)
    # plotting mean and SD into the graph
    for (mean, sd) in meanSDarray:
        if count > 0:
            maxindex += diffarr[count-1]
        plt.errorbar(mean, maxindex, xerr=sd, marker="|", color="red", lw=3,
                     markersize=30, markeredgewidth=3 )
        print maxindex
        count += 1
    # plotting stuffs
    plt.plot(sampleData, realDistance, "o", color="green")
    plt.ylim(-5,max(indexes)*1.3)
    plt.xlim(-5, max(indexes)*1.3)
    sampleData.append(0)
    plt.plot(sampleData, fit_fn_sample(sampleData), color="green", lw=4)
    plt.ylabel("True Distance")
    plt.xlabel("Sampled Distance")
    plt.show()
    


if __name__ == "__main__":
    main(sys.argv[1])
