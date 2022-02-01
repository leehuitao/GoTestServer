package main

import (
	"testserver/DBIO/MysqlManager"
	"testserver/LogService"
	"testserver/Methods"
	"testserver/Network"
)

func main() {
	//日志系统启动
	LogService.StartLogService("log")

	LogService.LogDebug("日志服务启动")
	//Mysql连接池启动
	MysqlManager.StartMysqlService("127.0.0.1", "3306", "root", "qazWSX123.", "gotestdb")
	//方法注册
	Methods.Init()
	//客户端数据初始化
	Methods.ClientInit()
	//网络模块启动
	var server Network.TcpServer

	server.StartListen("127.0.0.1:55555")

}
