package main

import (
	"testserver/DBIO/MysqlManager"
	"testserver/LogService"
	"testserver/Network"
)

func main() {
	//日志系统启动
	LogService.StartLogService("log")

	LogService.LogDebug("日志服务启动")
	//Mysql连接池启动
	MysqlManager.StartMysqlService("", "", "", "", "")

	Network.Init()
	//网络模块启动
	Network.NetworkInit()

	var server Network.TcpServer
	server.StartListen("127.0.0.1:12345")

}
