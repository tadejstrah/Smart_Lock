from flask import Flask
from flask import request
from rp import open
app = Flask(__name__)
@app.route('/')
def asd():
    return "Hello World"
locked = False
@app.route('/assistant', methods=['POST'])
def home():
    global locked
    data = request.get_json()
    data = data["result"]["action"]
    print("Request_action: ", data)
    if data == "unlock" and locked==True:
        open(1,0)
        locked =  False
    elif data == "lock" and locked==False:
        open(1,0)
        locked = True
    return "yo!"

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=25565)
