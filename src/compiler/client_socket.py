#!/usr/bin/env python

import socket 

HOST = '127.0.0.1'
PORT = 5000

s = socket.socket()
tcp = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
dest = (HOST, PORT)
tcp.connect(dest)

msg = ""

while msg != '\x18':
	r = tcp.recv(100)
	print("Server this is: %s" % r)
	if r == b"200\n" or b"200":
		msg = input("msg:").encode()
		tcp.send(msg)
		
	else:
		break
tcp.close()
