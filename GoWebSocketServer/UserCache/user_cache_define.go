package UserCache

import (
	"github.com/gorilla/websocket"
	"sync"
)

type Client struct {
	UUID   string `json:"uuid"`
	Socket *websocket.Conn
	Send   chan []byte
}

type ClientManager struct {
	Clients    map[string]*Client
	Register   chan *Client
	Unregister chan *Client
	sync.RWMutex
}

func NewClientManager() (clientManager *ClientManager) {
	clientManager = &ClientManager{
		make(map[string]*Client),
		make(chan *Client, 10),
		make(chan *Client, 10),
		sync.RWMutex{},
	}
	return clientManager
}

var Uuid2Key map[string]string

func Init() {
	Uuid2Key = make(map[string]string)
}
