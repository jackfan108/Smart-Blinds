import SocketServer
print "hey"
class Handler(SocketServer.BaseRequestHandler):
	def handle(self):
		print self.request.recv(2048)
		instruction = raw_input("enter your intruction: ")
		self.request.sendall(instruction)

server = SocketServer.TCPServer(("",9999),Handler)
server.serve_forever()