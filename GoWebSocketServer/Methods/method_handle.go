package Methods

import (
	"dtserver/LogService"
	"dtserver/PackManager"
	"dtserver/UserCache"
	"encoding/base64"
	"encoding/json"
	"github.com/gorilla/websocket"
	"github.com/satori/go.uuid"
)

//登录返回一个key给客户端
func loginHandle(pack PackManager.Message, conn *websocket.Conn) {
	LogService.LogDebug("loginHandle")
	request := PackManager.Request{}
	request.Order = pack.Request.Order
	request.Uuid = pack.Request.Uuid
	request.Key = uuid.NewV4().String()
	marshal, _ := json.Marshal(request)
	dst := base64.StdEncoding.EncodeToString(marshal)
	err := conn.WriteMessage(websocket.TextMessage, []byte(dst))
	UserCache.Uuid2Key[request.Uuid] = request.Key
	if err != nil {
		LogService.LogError(err.Error())
	}
}

// 将链接放到Clients里
func startRcvDataHandle(pack PackManager.Message, conn *websocket.Conn) {
	LogService.LogDebug("startRcvDataHandle")
	if UserCache.Uuid2Key[pack.Request.Uuid] == pack.Request.Key {
		client := UserCache.Client{}
		client.Socket = conn
		client.UUID = pack.Request.Uuid
		client.Send = make(chan []byte)
		ClientManager.Register <- &client
		response(pack, conn)
	}

}

//	接收到mes数据  给所有注册的客户端发送包
func rcvDataHandle(pack PackManager.Message, conn *websocket.Conn) {
	LogService.LogDebug("rcvDataHandle")
	message := PackManager.Message{}
	ClientManager.Write2All(&message)
}

// 将链接从Clients里删除
func stopRcvDataHandle(pack PackManager.Message, conn *websocket.Conn) {
	LogService.LogDebug("stopRcvDataHandle")
	if UserCache.Uuid2Key[pack.Request.Uuid] == pack.Request.Key {
		client := ClientManager.Clients[pack.Request.Uuid]
		response(pack, conn)
		ClientManager.Unregister <- client
	}
}

// 客户端之间发送消息
func sendDataHandle(pack PackManager.Message, conn *websocket.Conn) {
	LogService.LogDebug("sendDataHandle")
}

func response(pack PackManager.Message, conn *websocket.Conn) {
	request := PackManager.Request{}
	request.Order = pack.Request.Order
	request.Uuid = pack.Request.Uuid
	request.Key = pack.Request.Key
	marshal, _ := json.Marshal(request)
	dst := base64.StdEncoding.EncodeToString(marshal)
	err := conn.WriteMessage(websocket.TextMessage, []byte(dst))
	if err != nil {
		LogService.LogError(err.Error())
	}
}
