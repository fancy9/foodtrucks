
from flask import Flask, request, jsonify, render_template
import requests, json

APP_TOKEN = "egCeE3gSo3QkyTLtBEAsGpnZZ"
API_URL = "http://data.sfgov.org/resource/rqzj-sfat.json"

app = Flask(__name__)

@app.route("/")
def index():
	return render_template('index.html')

@app.route("/hello")
def hello():
	params = {
		'$$app_token': APP_TOKEN,
		'$where': 'blocklot > \'3711017\' AND blocklot < \'3711019\'',
	}

	r = requests.get(API_URL, params=params)

	return r.content

if __name__ == "__main__":
        app.run(host='0.0.0.0', port=80)




















