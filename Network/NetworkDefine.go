package Network

import (
	"fmt"
	"net"
	"strconv"
	"strings"
	"sync"
	"testserver/LogService"
	"testserver/PackManager"
	"testserver/Utils"
	"time"
)

var numberMutex sync.Mutex

type TcpServer struct {
	//当前登录的个数
	currentClientNumber int
	//历史登录个数
	historyClientNumber int
	//监听socket
	server net.Listener
}
type TcpClient struct {
	//ip
	ip string
	//端口
	port int
	//连接时间
	loginTime time.Time
	//客户端socket
	conn net.Conn
}

type ClientManager struct {
	//当前登录的个数
	currentClientNumber int
	//历史登录个数
	historyClientNumber int

	ClientMap map[string]TcpClient
}

var ClientManagerHandle ClientManager

func NetworkInit() {
	ClientManagerHandle.ClientMap = make(map[string]TcpClient)
}

// UpdateCurrentClientNumber 更新当前登录的个数
func (server *TcpServer) UpdateCurrentClientNumber(updateNumber int) {
	numberMutex.Lock()
	defer numberMutex.Unlock()
	server.currentClientNumber = server.currentClientNumber + updateNumber

}

// GetCurrentClientNumber 获取当前登录的个数
func (server *TcpServer) GetCurrentClientNumber() (currentClientNumber int) {
	return server.currentClientNumber
}

// AddHistoryClientNumber 新用户登录时增加一个
func (server *TcpServer) AddHistoryClientNumber() {
	server.historyClientNumber = server.historyClientNumber + 1
}

// GetHistoryClientNumber 获取历史登录个数
func (server *TcpServer) GetHistoryClientNumber() (historyClientNumber int) {
	return server.historyClientNumber
}

// StartListen 服务开始监听 address (127.0.0.1:8000)
func (server *TcpServer) StartListen(address string) {

	listen, err := net.Listen("tcp", address)
	server.server = listen
	if err != nil {
		fmt.Println("start listen error ", err.Error(), " listen address ", address)
		return
	}
	LogService.LogDebug("网络服务启动 address = " + address)
	defer server.server.Close()
	for {
		// 监听客户端连接请求

		conn, err := server.server.Accept()
		if err != nil {
			fmt.Println("listener.Accept err", err)
			return
		}
		// 设置一下连接的参数
		var client TcpClient
		client.SetConn(conn)
		// 存入缓存map key为ip:port
		ClientManagerHandle.ClientMap[conn.RemoteAddr().String()] = client
		// 开始获取数据
		go client.StartRead(client.conn)
	}
}

// StopListen 服务停止监听
func (server *TcpServer) StopListen() {
	server.server.Close()
}

// SetConn 设置客户端连接以及其他信息
func (client *TcpClient) SetConn(conn net.Conn) {

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

var number int

func (client *TcpClient) StartRead(conn net.Conn) {
	// 循环读取客户端发送数据
	number = 0
	for {
		buf := make([]byte, 4096*10)
		decodeSize := 0
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
			var decode *PackManager.Pack
			decode = PackManager.Decode(decodeSize, buf)
			go MethodPerform(decode, conn)
			decodeSize = decodeSize + decode.Header.PackSize
			number = number + 1
			fmt.Println("decodeSize = ", decodeSize, "n = ", n)
			fmt.Println("服务器读到数据：", string(decode.Body), "收到包个数：", number)
		}

	}
}

func (client *TcpClient) SendData(data []byte) {
	client.conn.Write(data)
}
