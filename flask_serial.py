from flask import Flask
from flask import request
from multiprocessing import Process, Value
import serial
import time

ser = serial.Serial("/dev/ttyS0")
ser.baudrate=9600
ser.timeout=0.3
app = Flask(__name__)

#raspi-config -> enable dev/ttyS0


#python3 flask_serial.py


def cmd(command):
    ser.write((str(command)+"\n").encode())
    x = ser.readline().decode()
    if(x==''): return "read_err" #ni odgovora
    if(x[-1]!='\n' and x[-2]!='\r'): return'read_err'
    return x[:-2]

@app.route('/sendcmd')
def hello_world():
    user = request.args.get('cmd') #localhost:5000/sendcmd?cmd=<CMD>
    if(user==None): return "error"
    stop.value = True
    while(work.value==True):
        time.sleep(0.001)
        print("CAKAM")
    tmp = cmd(user)
    stop.value = False
    return tmp #vrni odgovor


def record_loop(loop_on):
   while True:
        if(loop_on.value==True and stop.value==False):
            work.value = True
            print(cmd("get-1")) #arduino 1, ali je kaj novega? (+ npr. ce je, poslji na server)
            work.value = False
        time.sleep(1)
        
        
        
recording_on = Value('b', True)
stop = Value('b', False)
work = Value('b', False)
p = Process(target=record_loop, args=(recording_on,))
p.start()  
app.run(debug=True, use_reloader=False)
p.join()
