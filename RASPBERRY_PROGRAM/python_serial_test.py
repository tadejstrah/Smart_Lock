import serial
import crc8
from gpiozero import LED
from time import sleep

ser = serial.Serial("/dev/ttyS0")
ser.baudrate = 9600
ser.timeout = 0.2

tx_toggle = LED(18)
tx_toggle.off()

msglen = 11
NfEn = 0xA1
NfAd = 0xA2
NfRm = 0xA3
Novo = 0xDA
Open = 0xCD
OKByte = 0x2A
Reply = 0xEE

BufOvflErr = 0xFE
NfcDuplicateErr = 0xFE

'''
addr - arduino naslov (1. bajt)
num - številka omarice (0-7)
loc - 0 ali 1; prvi ali drugi prostor
uid - 7-bajtni array z UID-jem kartice

FUNKCIJE:
removeUID(addr,num,loc)
uploadUID(addr,num,loc,uid)
open(addr,num)
ping(addr)
==> zapolni events[] z dogodki v string obliki (<addr>.<omarica>.<nacin_odklepa>)

NAČIN ODKLEPA:
1 = ključ
2 = kartica

ERRORS:
0 = function arguments error
1-5 = communication error
6 = ?
7 = card duplicate error
11 = ni odgovora

100 = OK

'''




events = []

def parseEvents(addr,x):
	global events
	for i in x:
		if(i == BufOvflErr):
			events.append("BUFOVFL")
		elif(i != 0):
			events.append(str(addr) + "." + str(7&i) + "." + str((24&i) >> 3))

def chksum(data):
	hash = crc8.crc8()
	hash.update(data[:-2])
	return int(hash.digest()[0])

def removeUID(addr,num,loc):
	if(addr > 200 or num < 0 or num > 7 or loc < 0 or loc > 1): return 0
	x = (loc << 7) | num
	c = cmd(addr,[NfRm,x])
	if(len(c) == 1): return c
	return 100

def uploadUID(addr,num,loc,uid):
	if(addr > 200 or num < 0 or num > 7 or loc < 0 or loc > 1): return 0
	x = (loc << 7) | num
	c = cmd(addr,[NfAd,x] + uid)
	if(len(c) == 1): return c
	return 100

def open(addr,num):
	# num 0-7
	if(num > 7 or addr > 200): return 0
	c = cmd(addr,[Open,num])
	if(len(c) == 1): return c
	return 100

def ping(addr,start=0):
	# Novi dogodki?
	if(addr > 200): return 0
	if(start > 0): r = cmd(addr,[Novo,OKByte])
	else: r = cmd(addr,[Novo])
	if(len(r) == 1): return r
	parseEvents(addr,r[3:-2])
	if(start > 3): return 12
	if(r[2] == 1): ping(addr,start+1)
	return 100

def cmd(addr,data):
	# Večkrat poskuša poslati
	i = 0
	x = [0]
	while(len(x) < 2 and x != [6] and x != [7] and i < 3):
		x = send(addr,data)
		i = i + 1
	return x

def send(addr,data):
	# Napake 1-5: communication error
	# Napaka 11: ni odgovora
	# Napake 6-7: software error (7 = kartica že obstaja)

	if(len(data) > msglen - 2): return
	a = bytearray(msglen + 1)
	a[msglen] = 10
	a[0] = addr
	for i in range(len(data)): a[i+1] = data[i]
	a[msglen-1] = chksum(a)
	print(list(a))
	tx_toggle.on()
	ser.write(a)
	ser.flush()
	tx_toggle.off()
	x = (ser.read(msglen+1))
	if(x == list(bytearray(msglen+1))): return[11]
	print(list(x))
	if(len(x) < msglen): return [1]
	if(x[msglen] != 10): return [2]
	if(x[0] != Reply): return [3]
	if(x[1] != chksum(a)): return [4]
	if(x[msglen-1] != chksum(x)): return [5]
	if(a[1] != Novo):
            if(x[2] == OKByte): return list(x)
            elif(x[2] == NfcDuplicateErr): return [7]
            else: return [6]
	return list(x)
