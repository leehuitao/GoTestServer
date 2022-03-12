package Network

import (
	"dtserver/LogService"
	"dtserver/Methods"
	"encoding/base64"
	"fmt"
	"github.com/gin-gonic/gin"
	"github.com/gorilla/websocket"
	"net/http"
)

// ServerInit websocket服务启动
func ServerInit(name, port string) {
	r := gin.Default()
	r.GET(name, NewConnection)
	err := r.Run("127.0.0.1" + port)
	if err != nil {
		LogService.LogError(err.Error())
		return
	}

}

func receiveMessage(ws *websocket.Conn) {
	// 开始通信
	defer ws.Close()
	for {
		fmt.Println("start transfer message")
		_, msg, err := ws.ReadMessage()
		if err != nil {
			fmt.Println("webSocket read error")
			return
		}
		if len(msg) > 0 {
			decodeString, err := base64.StdEncoding.DecodeString(string(msg))
			if err != nil {
				return
			}
			Methods.MethodPerform(decodeString, ws)
		}
	}
}

func NewConnection(c *gin.Context) {
	ws, err := websocket.Upgrade(c.Writer, c.Request, nil, 1024, 1024)
	if err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{
			"msg": "服务端错误",
		})
		return
	}
	go receiveMessage(ws)
}
