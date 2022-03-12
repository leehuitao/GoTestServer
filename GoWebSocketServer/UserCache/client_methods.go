package UserCache

import (
	"dtserver/LogService"
	"dtserver/PackManager"
	"encoding/json"
	"errors"
	"github.com/gorilla/websocket"
)

func (c *Client) Read(close, renewal chan *Client) {
	defer func() {
		close <- c
	}()
	for {
		_, message, err := c.Socket.ReadMessage()
		if err != nil {
			break
		}
		// ...
		LogService.LogDebug(string(message))
	}
}

func (c *Client) Write(close chan *Client) {
	for {
		select {
		case message, ok := <-c.Send:
			if !ok {
				return
			}
			err := c.Socket.WriteMessage(websocket.TextMessage, message)
			if err != nil {
				return
			}

		}
	}
}

// SendOut 发送到注册的客户端
func (c *Client) SendOut(message *PackManager.Message) error {
	content, err := json.Marshal(message.Body)
	if err != nil {
		return err
	}
	c.Send <- content
	return nil
}

// Start 启动注册和解注册函数
func (manager *ClientManager) Start() {
	for {
		select {
		case conn := <-manager.Register:
			manager.Lock()
			manager.Clients[conn.UUID] = conn
			manager.Unlock()
			LogService.LogDebug("new client register")
		case conn := <-manager.Unregister:
			err := conn.Socket.Close()
			if err != nil {
				return
			}
			close(conn.Send)
			delete(manager.Clients, conn.UUID)
		}
	}
}

func (manager *ClientManager) Write(message *PackManager.Message) error {
	manager.RLock()
	client, ok := manager.Clients[message.Recipient]
	manager.RUnlock()
	if !ok {
		return errors.New("client miss [" + message.Recipient + "]")
	}
	return client.SendOut(message)
}

// Write2All 发送到所有注册的客户端
func (manager *ClientManager) Write2All(message *PackManager.Message) {

	for ip, client := range manager.Clients {
		err := client.SendOut(message)
		if err != nil {
			LogService.LogError("send to " + ip + " failed")
			continue
		}
	}

}
