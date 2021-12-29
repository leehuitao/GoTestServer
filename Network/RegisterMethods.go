package Network

import (
	"testserver/Methods"
	"testserver/PackManager"
)

var MethodsMap map[int]func(pack *PackManager.Pack) (request *PackManager.Pack)

func Init() {
	MethodsMap = make(map[int]func(pack *PackManager.Pack) (request *PackManager.Pack))
	Register()
}

// Register 注册方法
func Register() {
	MethodsMap[Login] = Methods.SendLogin
	MethodsMap[Logout] = Methods.SendLogout
	MethodsMap[StartSendFile] = Methods.SendStartFile
	MethodsMap[SendFileData] = Methods.SendFileData
	MethodsMap[SendFileCancel] = Methods.SendFileCancel
	MethodsMap[SendFileSuccess] = Methods.SendFileEnd

}

func MethodPerform(pack *PackManager.Pack) (request *PackManager.Pack) {
	pack.CreateBuffer()
	return MethodsMap[pack.Header.Method](pack)
}
