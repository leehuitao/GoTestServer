package Network

import (
	"net"
	"testserver/Methods"
	"testserver/PackManager"
)

var MethodsMap map[int]func(pack *PackManager.Pack, conn net.Conn) (request *PackManager.Pack)

func Init() {
	MethodsMap = make(map[int]func(pack *PackManager.Pack, conn net.Conn) (request *PackManager.Pack))
	Register()
}

// Register 注册方法
func Register() {
	MethodsMap[PackManager.Login] = Methods.SendLogin
	MethodsMap[PackManager.Logout] = Methods.SendLogout
	MethodsMap[PackManager.Msg] = Methods.SendMsg
	MethodsMap[PackManager.StartSendFile] = Methods.SendStartFile
	MethodsMap[PackManager.SendFileData] = Methods.SendFileData
	MethodsMap[PackManager.SendFileCancel] = Methods.SendFileCancel
	MethodsMap[PackManager.SendFileSuccess] = Methods.SendFileEnd

}

func MethodPerform(pack *PackManager.Pack, conn net.Conn) (request *PackManager.Pack) {
	return MethodsMap[pack.Header.Method](pack, conn)
}
