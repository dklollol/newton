import numpy as np
import matplotlib.pyplot as plt
# Fit for green test: 81.9183589945*x + -7.9820892106
# Fit for pink test: 98.8597189773*x + -12.1000433767

def main():
    readData = open("green.txt", "r").read()
    dataList = readData.splitlines()
    greenData = []
    for x in dataList:
        if len(x.split()) == 2: # check if "cm"
            continue
        y = x.split()
        avgofY = reduce( lambda x, y : float(x) + float(y), y)/len(y)
        greenData.append(avgofY)
    #print greenData

    readData = open("pink.txt", "r").read()
    dataList = readData.splitlines()
    pinkData = []
    for x in dataList:
        if len(x.split()) == 2:
            continue
        y = x.split()
        avgofY = reduce( lambda x, y : float(x) + float(y), y)/len(y)
        pinkData.append(avgofY)
    # print pinkData
    realDistance = [0, 10, 20, 30, 40]
    a, b = np.polyfit(greenData, realDistance, 1)
    print "Fit for green test: "+str(a) + "*x + " + str(b)
    fit_fn_green = np.poly1d((a, b))
    a, b = np.polyfit(pinkData, realDistance, 1)
    fit_fn_pink = np.poly1d((a, b))
    print "Fit for pink test: "+str(a) + "*x + " + str(b)
    # fit_fn is now a function which takes in x and returns an estimate for y
    plt.plot(greenData, realDistance)
    plt.plot(greenData, fit_fn_green(greenData))
    plt.plot(pinkData, realDistance)
    plt.plot(pinkData, fit_fn_pink(pinkData))
    plt.ylabel("True Distance")
    plt.xlabel("Sampled Distance")
    plt.show()
    


if __name__ == "__main__":
    main()
