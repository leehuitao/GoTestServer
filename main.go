package main

import "testserver/Network"

func main() {
	println("123")
	Network.NetworkInit()
	var server Network.TcpServer
	server.StartListen("127.0.0.1:12345")

}
