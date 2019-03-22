from bFieldControllerInterface import BFieldControllerInterface
import matplotlib.pyplot as plt
import time



dev = BFieldControllerInterface('10.255.6.249')

dev.resetFeedforward()
dev.setFeedforward(1, 0, 0.5)
dev.setFeedforward(3, 0, 0.1)
dev.setFeedforward(5, 0, 0.02)

plt.ion()
plt.show()


tVec = []
phaseVec = []
otherVec = []

while 1:
    periodWord, phaseWord = dev.getPhaseLockStatus()
    print("{}\t{}".format(phaseWord, periodWord))
    phaseVec.append(phaseWord)
    otherVec.append(periodWord-6666)
    
    tVec = range(len(phaseVec))
    plt.clf()
    plt.plot(tVec, otherVec)
    plt.draw()
    time.sleep(0.05)