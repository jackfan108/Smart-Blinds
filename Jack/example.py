#!/usr/bin/env python

from flask import Flask, render_template, request
import redis
import json
app = Flask(__name__)

orientation_current_value_html_value = 0
position_current_value_html_value = 0



"""orientation_Target_value value is "?" until a integer is entered.
   The value is stored in redis as "orientation_target_value." 
   The current position is pulled from redis and then displayed on the webpage.
   Template "index.html" is rendered and sent to the browser with two variables passed in."""
@app.route("/",methods = ['GET','POST'])
def homepage():
	#connect to Redis Server
	r1 = redis.Redis("localhost", port = 6379, db = 1)
	if 'orientation_target_value' in request.form.keys():
		try:
			orientation_target_value_html_value = int(request.form['orientation_target_value'])
		except ValueError:
			orientation_target_value_html_value = '?'
	else:
		orientation_target_value_html_value = "?"
	if 'position_target_value' in request.form.keys():
		try:
			position_target_value_html_value = int(request.form['position_target_value'])
		except ValueError:
			position_target_value_html_value = '?'
	else:
		position_target_value_html_value = "?"
	r1.set('orientation_target_value',str(orientation_target_value_html_value))
	print r1.get('orientation_target_value')
	orientation_current_value_html_value = r1.get('orientation_current_value')
	r1.set('position_target_value',str(position_target_value_html_value))
	print r1.get('position_target_value')
	position_current_value_html_value = r1.get('position_current_value')
	print r1.get('tempf')
	tempf = r1.get('tempf')
	print r1.get('tempc')
	tempc = r1.get('tempc')	
	print r1.get('light')
	light = r1.get('light')	
	return render_template('index.html', orientation_target_value_html = orientation_target_value_html_value, orientation_current_value_html = orientation_current_value_html_value, position_target_value_html = position_target_value_html_value, position_current_value_html = position_current_value_html_value, tempf = tempf, tempc = tempc, light = light)

"""This is a test webpage."""
@app.route("/test/")
def test():
	return "Yo Wassup!!!"

"""This page is called every 500 milliseconds after the main page is loaded.
   It sends the current position value pulled from redis when the javascript setInterval() funciton in index.html is called.
   This page returns a json(javascript object notation) object for javascript to interpret."""
@app.route('/orientation_value')
def orientation_value():
	r2 = redis.Redis("localhost", port = 6379, db = 1)
	orcurpos = r2.get('orientation_current_value')
	return json.dumps({"orientation_current_value" : orcurpos})

@app.route('/position_value')
def position_value():
	r3 = redis.Redis("localhost", port = 6379, db = 1)
	poscurpos = r3.get('position_current_value')
	return json.dumps({"position_current_value" : poscurpos})

@app.route('/tempf')
def tempf():
	r4 = redis.Redis("localhost", port = 6379, db = 1)
	tempf = r4.get('tempf')
	return json.dumps({"tempf" : tempf})

@app.route('/tempc')
def tempc():
	r5 = redis.Redis("localhost", port = 6379, db = 1)
	tempc = r5.get('tempc')
	return json.dumps({"tempc" : tempc})

@app.route('/light')
def light():
	r6 = redis.Redis("localhost", port = 6379, db = 1)
	light = r6.get('light')
	return json.dumps({"light" : light})

if __name__ == "__main__":
    app.run("", 5001, debug=True)	