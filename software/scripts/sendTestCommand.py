import time
import matplotlib.pyplot as plt

from bFieldControllerInterface import BFieldControllerInterface

dev = BFieldControllerInterface('10.255.6.249')


def getVector():
    val = []
    for n in range(120):
        dev.s.send("test {}\n".format(n).encode())
        n = int(dev._recvLine().strip())
        val.append(  n / 2**16)
    return val

dev.resetFeedforward()
dev.setFeedforward(1, 0, 0.5)
dev.setFeedforward(3, 0, 0.1)
dev.setFeedforward(5, 0, 0.02)
vec = getVector()

    
nVec = range(120)
    
plt.plot(nVec, vec)
plt.show()
