from bFieldControllerInterface import BFieldControllerInterface
import time



dev = BFieldControllerInterface('10.255.6.249')
#dev.setCoarseDac(2**16-1)
#dev.setFineDac(2**16-1)
dev.setCoarseDac(0)
dev.setFineDac(0)

#time.sleep(60)

for x in xrange(0,16+1):
  time.sleep(3)
  dev.setCoarseDac(2**x-1)
  print "coarse ",(2**x-1)," - fine 0\n"

time.sleep(3)
dev.setCoarseDac(0)
  
for x in xrange(0,16+1):
  time.sleep(3)
  dev.setFineDac(2**x-1)
  print "coarse ",0," - fine ",(2**x-1),"\n"

time.sleep(1)
dev.setFineDac(0)