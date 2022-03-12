package Methods

import (
	"dtserver/LogService"
	"dtserver/PackManager"
	"dtserver/UserCache"
	"encoding/json"
	"github.com/gorilla/websocket"
)

var MethodMap map[int]func(pack PackManager.Message, conn *websocket.Conn)

var ClientManager *UserCache.ClientManager

func Init() {
	MethodMap = make(map[int]func(pack PackManager.Message, conn *websocket.Conn))

	ClientManager = UserCache.NewClientManager()

	go ClientManager.Start()

	Register()
}
func MethodPerform(pack []byte, conn *websocket.Conn) {
	//decode header
	message := PackManager.Message{}
	message.Request = decodeHeader(pack)
	message.Body = pack
	if message.Request.Order != -1 {
		MethodMap[message.Request.Order](message, conn)
	}

}

// Register 注册方法
func Register() {
	MethodMap[PackManager.OrderLogin] = loginHandle
	MethodMap[PackManager.OrderStartRcvData] = startRcvDataHandle
	MethodMap[PackManager.OrderRcvData] = rcvDataHandle
	MethodMap[PackManager.OrderStopRcvData] = stopRcvDataHandle
	MethodMap[PackManager.OrderSendData] = sendDataHandle
}

func decodeHeader(data []byte) PackManager.Request {
	request := PackManager.Message{}
	err := json.Unmarshal(data, &request)
	if err != nil {
		request.Request.Order = -1
		LogService.LogError(err.Error())
	}
	return request.Request
}
