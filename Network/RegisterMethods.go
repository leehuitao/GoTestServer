package Network

import "testserver/Methods"

var MethodsMap map[int]func(pack *Pack)(request *Pack)

func Init() {
	MethodsMap = make(map[int]func(pack *Pack)(request *Pack))
	Register()
}

// Register 注册方法
func Register() {
	MethodsMap[Login] 				= Methods.SendLogin
	MethodsMap[Logout] 				= Methods.SendLogout
	MethodsMap[StartSendFile] 		= Methods.SendStartFile
	MethodsMap[SendFileData] 		= Methods.SendFileData
	MethodsMap[SendFileCancel] 		= Methods.SendFileCancel
	MethodsMap[SendFileSuccess] 	= Methods.SendFileEnd

}

func MethodPerform(pack *Pack) (request *Pack) {
	return MethodsMap[pack.header.method](pack)
}
