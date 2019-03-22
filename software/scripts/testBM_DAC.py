from bFieldControllerInterface import BFieldControllerInterface
import time
import sys


dev = BFieldControllerInterface('10.255.6.249')


if (len(sys.argv) >= 1):
	dev.setCoarseDac(int(sys.argv[1]))
else:
	dev.setCoarseDac(0)

if (len(sys.argv) >= 2):
	dev.setFineDac(int(sys.argv[2]))
else:
	dev.setFineDac(0)

#time.sleep(60)