from socket import *
import redis
HOST = '192.168.51.60'
PORT = 9999
ADDR = (HOST,PORT)
BUFSIZE = 4096                        # maximum bytes to read
serv = socket( AF_INET,SOCK_STREAM)   # creates a server object
serv.bind(ADDR)                       # associate the address/port with the server
serv.listen(1)                        # maximum clients that can connect to the server
print "listening..."
conn,addr = serv.accept()             # accepts a client request to connnect and establishes the connection
conn.send('Hi, you\'ve successfully connnected to the server!!')
print conn 
print ADDR
print 'Connected by ', addr

# creates a redis object that links to the redis database
r1 = redis.Redis("localhost", port = 6379, db = 1)
orientation_current_value = ""
position_current_value = ""
tempf = ""
tempc = ""
light = ""
nextstart = 0
datarecv = ""
datasend = ""


while True:
	#Receives the current position from the arduino and then stores it in the redis database.
	orientation_current_value = ""
	position_current_value = ""
	tempf = ""
	tempc = ""
	light = ""	
	print "come on bitch"
	datarecv = conn.recv(BUFSIZE)
	print "receive data"
	print datarecv
	for i in range(len(datarecv)):
		if (datarecv[i] != 'a'):
			orientation_current_value += datarecv[i]
			continue
		nextstart = i+1
		break
	for i in range(nextstart,len(datarecv)):
		if (datarecv[i] != 'b'):
			position_current_value += datarecv[i]
			continue
		nextstart = i+1
		break
	for i in range(nextstart,len(datarecv)):
		if (datarecv[i] != 'c'):
			tempf += datarecv[i]
			continue
		nextstart = i+1
		break
	for i in range(nextstart,len(datarecv)):
		if (datarecv[i] != 'd'):
			tempc += datarecv[i]
			continue
		nextstart = i+1
		break
	for i in range(nextstart,len(datarecv)):
		if (datarecv[i] != 'e'):
			light += datarecv[i]
			continue
		break
	print "after for loops"
	r1.set('orientation_current_value',orientation_current_value)
	r1.set('position_current_value',position_current_value)
	r1.set('tempf',tempf)
	r1.set('tempc',tempc)
	r1.set('light',light)
	#Sends the target_position it acquires from the redis database to the arduino.
	print "orientation"
	print r1.get('orientation_current_value')
	print "position"
	print r1.get('position_current_value')
	print "tempf"
	print r1.get('tempf')
	print "tempc"
	print r1.get('tempc')
	print "light"
	print r1.get('light')
	datasend = r1.get('orientation_target_value') + 'a' + r1.get('position_target_value') + 'b'
	print datasend
	conn.send(datasend)

conn.close()