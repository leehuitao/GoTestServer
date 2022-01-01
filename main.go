package main

import (
	"testserver/LogService"
	"testserver/Network"
)

func main() {
	//日志系统启动
	LogService.StartLogService("log")

	LogService.LogDebug("日志服务启动")

	//网络模块数据结构初始化
	Network.NetworkInit()
	var server Network.TcpServer
	server.StartListen("127.0.0.1:12345")

}
