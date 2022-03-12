package Network

import (
	"golang.org/x/net/websocket"
	"sync"
)

var numberMutex sync.Mutex

type WebSocketServer struct {
	//当前登录的个数
	currentClientNumber int
	//历史登录个数
	historyClientNumber int
	//监听socket
	server *websocket.Conn
}
