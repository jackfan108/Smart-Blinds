import SocketServer
class Handler(SocketServer.BaseRequestHandler):
    def handle(self):
        print self.request.recv(2048)
        self.request.sendall("HELLO RIGHT BACK")
        
server = SocketServer.TCPServer(("",9999),Handler)
server.serve_forever()
