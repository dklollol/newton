
import numpy as np
import matplotlib.pyplot as plt
import math as math
# Fit for green test: 81.9183589945*x + -7.9820892106
# Fit for pink test: 98.8597189773*x + -12.1000433767

def SD(arr):
    mean = np.mean(arr)
    sose = 0.0
    for sample in arr:
        sose += (sample-mean)**2
    return mean, math.sqrt(sose/len(arr))
def main():
    readData = open("green.txt", "r").read()
    dataList = readData.splitlines()
    greenData = []
    for x in dataList:
        if len(x.split()) == 2: # check if "cm"
            continue
        y = x.split()
        #avgofY = reduce( lambda x, y : float(x) + float(y), y)/len(y)
        for sample in y:
            if float(sample) == 1.6:
                continue
            greenData.append(float(sample)*100)
    gmean0, gSD0 = SD(greenData[:20])
    gmean10, gSD10 = SD(greenData[20:40])
    gmean20, gSD20 = SD(greenData[40:60])
    gmean30, gSD30 = SD(greenData[60:80])
    gmean40, gSD40 = SD(greenData[80:])
    readData = open("pink.txt", "r").read()
    dataList = readData.splitlines()
    pinkData = []
    for x in dataList:
        if len(x.split()) == 2:
            continue
        y = x.split()
        #avgofY = reduce( lambda x, y : float(x) + float(y), y)/len(y)
        for sample in y:
            if float(sample) == 1.6:
                continue
            pinkData.append(float(sample)*100)
    pmean0, pSD0 = SD(pinkData[:20])
    pmean10, pSD10 = SD(pinkData[20:40])
    pmean20, pSD20 = SD(pinkData[40:60])
    pmean30, pSD30 = SD(pinkData[60:80])
    pmean40, pSD40 = SD(pinkData[80:])
    realDistance = []
    x = -10
    for i in range(5):
        for t in range(20):
            if len(realDistance) == len(greenData):
                continue
            realDistance.append(i*10)
    a, b = np.polyfit(greenData, realDistance, 1)
    print "Fit for green test: "+str(a) + "*x + " + str(b)
    fit_fn_green = np.poly1d((a, b))
    # fit_fn is now a function which takes in x and returns an estimate for y
    #plt.plot(greenData, realDistance, "o", color="green")
    plt.ylim(-5,45)
    #plt.plot(greenData, fit_fn_green(greenData), color="green", lw=4)
    x = -10
    realDistance = []
    for i in range(5):
        for t in range(20):
            if len(realDistance) == len(pinkData):
                continue
            realDistance.append(i*10)
    a, b = np.polyfit(pinkData, realDistance, 1)
    fit_fn_pink = np.poly1d((a, b))
    print "Fit for pink test: "+str(a) + "*x + " + str(b)

    plt.plot(pinkData, realDistance, "o", color="magenta")
    plt.plot(pinkData, fit_fn_pink(pinkData), color="magenta", lw=4)
    # #green error bars
    # plt.errorbar(gmean0, 0, xerr=gSD0, marker="|", color="red", lw=3, markersize=30, markeredgewidth=3 )
    # plt.errorbar(gmean10, 10, xerr=gSD10, marker="|", color="red", lw=3, markersize=30, markeredgewidth=3 )
    # plt.errorbar(gmean20, 20, xerr=gSD20, marker="|", color="red", lw=3, markersize=30, markeredgewidth=3 )
    # plt.errorbar(gmean30, 30, xerr=gSD30, marker="|", color="red", lw=3, markersize=30, markeredgewidth=3 )
    # plt.errorbar(gmean40, 40, xerr=gSD40, marker="|", color="red", lw=3, markersize=30, markeredgewidth=3 )
    # # pink error bars
    plt.errorbar(pmean40, 40, xerr=pSD40, marker="|", color="blue", lw=3, markersize=30, markeredgewidth=5 )
    plt.errorbar(pmean30, 30, xerr=pSD30, marker="|", color="blue", lw=3, markersize=30, markeredgewidth=5 )
    plt.errorbar(pmean20, 20, xerr=pSD20, marker="|", color="blue", lw=3, markersize=30, markeredgewidth=5 )
    plt.errorbar(pmean10, 10, xerr=pSD10, marker="|", color="blue", lw=3, markersize=30, markeredgewidth=5 )
    plt.errorbar(pmean0, 0, xerr=pSD0, marker="|", color="blue", lw=3, markersize=30, markeredgewidth=5 )
    plt.ylabel("True Distance")
    plt.xlabel("Sampled Distance")
    plt.show()
    


if __name__ == "__main__":
    main()
