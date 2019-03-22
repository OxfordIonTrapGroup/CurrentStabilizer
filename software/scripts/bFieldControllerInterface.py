import socket

class BFieldControllerInterface:
    def __init__(self, addr, port=9760):
        self.s = None
        self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.s.connect((addr, port))

    def identify(self):
        self.s.send("idn?\n".encode())
        return self._recvLine().strip()
        
    def _recvLine(self):
        response = ""
        while( '\n' not in response):
            response += self.s.recv(1).decode()
        return response
        
    def comTest(self, str):
        """Sends a (short, no whitespace) string and confirm the device echos it back"""
        self.s.send("comTest {}\n".format(str).encode())
        response = self._recvLine()
        if str == response[8:].strip():
            return True
        else:
            print("Error: str='{}', got '{}'".format(str, response))
            return False
            
    def setCoarseDac(self, val):
        """Set the coarse setpoint DAC. val must be between 0 and 65535"""
        if val < 0 or val > 65535:
            raise ValueError("DAC value must be a uint16")
        self.s.send("setCoarseDac {}\n".format(int(val)).encode())
        
    def setFineDac(self, val):
        """Set the fine setpoint DAC. val must be between 0 and 65535"""
        if val < 0 or val > 65535:
            raise ValueError("DAC value must be a uint16")
        self.s.send("setFineDac {}\n".format(int(val)).encode())
        
    def setFeedforward(self, harmonic, component, amplitude):
        if harmonic < 1 or harmonic > 5:
            raise ValueError("Harmonic should be between 1 and 5")
        if component != 0:
            comp = 'c'
        else:
            comp = 's'
            
        if abs(amplitude) > 1:
            raise ValueError("Amplitude should be between -1 and 1")
            
        self.s.send("setFeedforward {} {} {}\n".format(harmonic, comp, int(amplitude*(2**15-1))).encode())
        
    def resetFeedforward(self):
        self.s.send("resetFeedforward\n".encode())
        
    def getErrorSignal(self):
        self.s.send("getErrorSignal\n".encode())
        return int(self._recvLine())
        
    def getPhaseLockStatus(self):
        self.s.send("phaseLockStatus\n".encode())
        str1 = self._recvLine()
        str2 = self._recvLine()
        return int(str1[9:]), int(str2[8:])

    def close(self):
        self.s.close()