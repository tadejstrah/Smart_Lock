from flask import Flask
from flask import request
from rp import open
app = Flask(__name__)
@app.route('/')
def asd():
    return "Hello World"

@app.route('/assistant', methods=['POST'])
def home():
    data = request.get_json()
    data = data["result"]["action"]
    print("Request_action: ", data)
    if data == "unlock":
        open(1,0)

    elif data == "lock":
    	lock(1,0)

    return "yo!"

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=25565)
