#!/usr/bin/env python

from flask import Flask, render_template, request, g
app = Flask(__name__)

values = {"position": "?"}
target_position = 0

@app.route("/",methods = ['GET'])
def hello():
  return render_template('example.html', value=values["position"])

@app.route("/test/")
def test():
	return "Yo Wassup!!!"

@app.route("/set/")
def set_position():
  values["position"] = int(request.args.get('position'))
  return ""
@app.route("/target_position/",methods = ['GET','POST'])
def target_position():
	try:
		target_position = int(request.form['target_position'])
	except ValueError:
		return "Please enter numbers!!"
	return request.form['target_position']
	
if __name__ == "__main__":
    app.run("", 5001, debug=True)