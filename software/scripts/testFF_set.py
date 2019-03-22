from bFieldControllerInterface import BFieldControllerInterface
import matplotlib.pyplot as plt
import time
import sys



dev = BFieldControllerInterface('10.255.6.249')


if ((len(sys.argv) >= 2) and int(sys.argv[1]) == 0):
    print("Reset Feedforward ...")
    dev.resetFeedforward()
    print("... success")
    exit()

if (len(sys.argv) <= 3):
    raise ValueError("3 arguments required (# harmonic, cosine?, amplitude)\n")

harmonic = int(sys.argv[1])
cos = bool(int(sys.argv[2]))
amplitude = float(sys.argv[3])

dev.setFeedforward(harmonic, cos, amplitude)