import serial
import crc8
from time import sleep

ser = serial.Serial("/dev/ttyS0")
ser.baudrate = 9600
ser.timeout = 0.5


msglen = 11
NfEn = 0xA1
NfAd = 0xA2
NfRm = 0xA3
Novo = 0xDA
Open = 0xCD
OKByte = 0x2A
Reply = 0xEE

def sendEvent(events):
    print(events)
    print(7 & i)
    print(24 & i)
def chksum(data):
	hash = crc8.crc8()
	hash.update(data[:-2])
	return int(hash.digest()[0])

def send(addr,data):
	if(len(data) > msglen - 2): return
	a = bytearray(msglen + 1)
	a[msglen] = 10
	a[0] = addr
	for i in range(len(data)): a[i+1] = data[i]
	a[msglen-1] = chksum(a)
	print(list(a))
	ser.write(a)
	sleep(0.05)
	x = (ser.read(msglen+1))
	print(list(x))
	if(len(x) < msglen): return "data"
	if(x[msglen] != 10): return "newline"
	if(x[0] != Reply): return "byte1"
	if(x[1] != chksum(a)): return "byte2"
#	if(x[2] != OKByte): return "okbyte"
	if(x[msglen-1] != chksum(x)): return "crc8"
	if(a[1] != Novo):
            if(x[2] == OKByte): return "ok"
            else: return "okbyte"
            if(a[1] == Novo):
                events = []
                x = list(x)
                for i in range(3,msglen-1):
                    if(x[i] != 0): events.append(x[i])
                sendEvent(events)

'''while True:
	ser.write("test\n".encode())
	x = ser.readline()
	print(x)
	sleep(1)'''
