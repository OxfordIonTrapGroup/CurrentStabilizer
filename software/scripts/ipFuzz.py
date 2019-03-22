# Rough attempt to make sure TCP/IP stack and command interpreter are working properly
# We send a random number of (random) loop-back commands to the device and ensure the correct response.
# We then disconnect, wait for a random period of time, then repeat...
#
# With commit 53f6988 this ran for 7000 connections and 3.6 million commands without failure


from bFieldControllerInterface import BFieldControllerInterface
import time
import random
import string

addr = '192.168.1.100'

def randomString(nMax=100):
    """Generates a random string of random length 1..nMax"""
    N = 1+random.randrange(0,nMax)
    return ''.join(random.choice(string.ascii_uppercase + string.digits) for _ in range(N))

def fuzzDevice(maxCommands=1000):
    """Sends a random number (0..maxCommands) of random length comTest commands to a device"""
    global nFailures
    global nCommands
    global nConnects
    dev = BFieldControllerInterface(addr)
    for _ in range(random.randrange(0,maxCommands+1)):
        if dev.comTest(randomString()) is not True:
            nFailures += 1
        nCommands += 1
    dev.close()
    nConnects += 1

    
nCommands = 0
nConnects = 0
nFailures = 0

while 1:
    fuzzDevice()
    print("Connections = {}\tCommands = {}\tFailures = {}".format(nConnects, nCommands, nFailures))
    time.sleep(2)