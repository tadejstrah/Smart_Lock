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

def parseEvents(events):
    print(events)
    print(7 & i)
    print(24 & i)
    for i in events:
    	i = str(7 & i) + "." + str(24&i)
    	return events
def chksum(data):
	hash = crc8.crc8()
	hash.update(data[:-2])
	return int(hash.digest()[0])
def ping(addr):
	# Novi dogodki?
	events = []
	r = cmd(addr,[Novo,OKByte])
	if(len(r) == 0):
		print(r)
		return False
	events.append(parseEvents(r))
	if(r[1] == 1):
		q = cmd(addr,[Novo,OKByte])
		return r + q
		events.append(parseEvents(q))
	return events
	
def cmd(addr,data):
	# Večkrat poskuša
	i = 0
	x = [0]
	while(len(x) < 2 and i < 4):
		x = send(addr,data)
		i = i + 1
	return x
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
	if(len(x) < msglen): return [1]
	if(x[msglen] != 10): return [2]
	if(x[0] != Reply): return [3]
	if(x[1] != chksum(a)): return [4]
#	if(x[2] != OKByte): return "okbyte"
	if(x[msglen-1] != chksum(x)): return [5]
	if(a[1] != Novo):
            if(x[2] == OKByte): return list(x)
            else: return [6]
	return list(x)

'''    if(a[1] == Novo):
        events = []
        x = list(x)
        for i in range(3,msglen-1):
            if(x[i] != 0): events.append(x[i])
        sendEvent(events) '''

'''while True:
	ser.write("test\n".encode())
	x = ser.readline()
	print(x)
	sleep(1)'''
