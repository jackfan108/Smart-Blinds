from socket import *
HOST = 'localhost'
PORT = 8000
ADDR = (HOST,PORT)
BUFSIZE = 4096

cli = socket( AF_INET,SOCK_STREAM)
cli.connect((ADDR))

while True:
	print '2'
	data = cli.recv(BUFSIZE)
	print "received ", repr(data)
	reply = raw_input("Reply: ")
	cli.send(reply)

cli.close()