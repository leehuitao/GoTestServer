package Network

import (
	"fmt"
	"gotestserver/Utils"
	"net"
	"strconv"
	"strings"
	"sync"
	"time"
)

var numberMutex sync.Mutex

type TcpServer struct {
	//当前登录的个数
	currentClientNumber int
	//历史登录个数
	historyClientNumber int
	//监听socket
	server 				net.Listener
}
type TcpClient struct {
	//ip
	ip 				string
	//端口
	port 			int
	//连接时间
	loginTime		time.Time
	//客户端socket
	conn 			net.Conn
}

var ClientMap map[string]TcpClient

func init(){
	ClientMap = make(map[string]TcpClient)
}

// UpdateCurrentClientNumber 更新当前登录的个数
func (server *TcpServer) UpdateCurrentClientNumber(updateNumber int) () {
	numberMutex.Lock()
	defer numberMutex.Unlock()
	server.currentClientNumber = server.currentClientNumber + updateNumber
}

// GetCurrentClientNumber 获取当前登录的个数
func (server *TcpServer) GetCurrentClientNumber( ) (currentClientNumber int) {
	return server.currentClientNumber
}

// AddHistoryClientNumber 新用户登录时增加一个
func (server *TcpServer) AddHistoryClientNumber() () {
	server.historyClientNumber = server.historyClientNumber + 1
}

// GetHistoryClientNumber 获取历史登录个数
func (server *TcpServer) GetHistoryClientNumber() (historyClientNumber int) {
	return server.historyClientNumber
}

// StartListen 服务开始监听 address (127.0.0.1:8000)
func (server *TcpServer) StartListen(address string)  {

	listen, err := net.Listen("tcp", address)
	server.server = listen
	if err != nil {
		fmt.Println("start listen error ",err.Error()," listen address ",address)
		return
	}
	defer server.server.Close()
	for {
		// 监听客户端连接请求
		fmt.Println("服务器等待客户端连接")
		conn, err := server.server.Accept()
		if err != nil {
			fmt.Println("listener.Accept err", err)
			return
		}
		// 设置一下连接的参数
		var client TcpClient
		client.SetConn(conn)
		// 存入缓存map key为ip:port
		ClientMap[conn.RemoteAddr().String()] = client
		// 开始获取数据
		go client.StartRead(client.conn)
	}
}

// StopListen 服务停止监听
func (server *TcpServer) StopListen() () {
	server.server.Close()
}

// SetConn 设置客户端连接以及其他信息
func (client *TcpClient) SetConn(conn net.Conn) () {

	addr := conn.RemoteAddr().String()
	split := strings.Split(addr, ":")
	client.ip = split[0]
	port, err := strconv.Atoi(split[1])
	if err != nil {
		fmt.Println("strconv.Atoi err", err)
		return
	}
	client.port = port
	client.loginTime = Utils.GetCurrentTime()
	client.conn = conn
}

func (client *TcpClient) StartRead(conn net.Conn) () {
	// 循环读取客户端发送数据
	buf := make([]byte, 4096*4)
	for {
		var decodeSize int
		n, err := conn.Read(buf)
		if n == 0 {
			fmt.Println("客户端已关闭，断开连接")
			// 这里关闭的只是go线程，main线程还是在for循环的
			return
		}
		if err != nil {
			fmt.Println("conn Read err", err)
			return
		}
		for decodeSize != n {
			decode := Decode(buf[:n])
			MethodPerform(decode)
			buf = buf[decode.header.packSize:n]
			decodeSize = decodeSize + decode.header.packSize
		}
		fmt.Println("服务器读到数据：", string(buf[:n]))
	}
}

func (client *TcpClient) SendData(data []byte) () {
	client.conn.Write(data)
}