package Methods

import (
	"bytes"
	"encoding/binary"
	"encoding/json"
	"fmt"
	"net"
	"strconv"
	"strings"
	"testserver/PackManager"
	"testserver/UserCache"
	"testserver/Utils"
	"time"
)

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

	clientMap map[string]TcpClient
}

var ClientManagerHandle ClientManager

func ClientInit() {
	ClientManagerHandle.clientMap = make(map[string]TcpClient)
}
func (clientManager *ClientManager) AddConn(tcpClient TcpClient) {
	ClientManagerHandle.clientMap[tcpClient.conn.RemoteAddr().String()] = tcpClient
}

func (clientManager *ClientManager) DelConn(ip string) {
	delete(ClientManagerHandle.clientMap, ip)
}

func (clientManager *ClientManager) GetConn(UserName string) TcpClient {
	addr := userCache.GetUserLoginAddressMap()
	ipPort := addr[UserName]
	client := clientManager.clientMap[ipPort]
	return client
}

func (clientManager *ClientManager) SendToConn(ip string, pack []byte) {
	ClientManagerHandle.clientMap[ip].conn.Write(pack)
}

// UpdateConn 设置客户端连接以及其他信息
func (client *TcpClient) UpdateConn(conn net.Conn) {

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
			UserName := userCache.GetUserNameFromIpPort(conn.RemoteAddr().String())
			NoticeAllOnlineUserChangeStatus(UserName, UserCache.LogoffStatus)
			ClientManagerHandle.DelConn(conn.RemoteAddr().String())
			userCache.DelUserCache(UserName)
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

var MethodMap map[int]func(pack *PackManager.Pack, conn net.Conn) (request *PackManager.Pack)

func Init() {
	MethodMap = make(map[int]func(pack *PackManager.Pack, conn net.Conn) (request *PackManager.Pack))
	Register()
}

// Register 注册方法
func Register() {
	MethodMap[PackManager.Login] = SendLogin
	MethodMap[PackManager.Logout] = SendLogout
	MethodMap[PackManager.Msg] = SendMsg
	MethodMap[PackManager.StartSendFile] = SendStartFile
	MethodMap[PackManager.SendFileData] = SendFileData
	MethodMap[PackManager.SendFileCancel] = SendFileCancel
	MethodMap[PackManager.SendFileSuccess] = SendFileEnd
	MethodMap[PackManager.ContinueSend2File] = SendFileContinue

}

func MethodPerform(pack *PackManager.Pack, conn net.Conn) (request *PackManager.Pack) {
	return MethodMap[pack.Header.Method](pack, conn)
}

var newChannelCache = Utils.NewChannelCache()
var userCache = UserCache.NewUserCache()

func intToBytes(n int) []byte {
	x := int32(n)
	bytesBuffer := bytes.NewBuffer([]byte{})
	binary.Write(bytesBuffer, binary.BigEndian, x)
	return bytesBuffer.Bytes()
}

func createSendBuffer(pack PackManager.Pack) []byte {
	var buffer bytes.Buffer //Buffer是一个实现了读写方法的可变大小的字节缓冲
	buffer.Write(intToBytes(PackManager.HeaderSize + len(pack.Body)))
	buffer.Write(intToBytes(pack.Header.Method))
	buffer.Write(intToBytes(pack.Header.MethodType))
	buffer.Write(pack.Body)
	return buffer.Bytes()
}

func NoticeAllOnlineUserChangeStatus(UserName string, status int) {

	pack := PackManager.Pack{}
	onlineListBody := PackManager.OnlineListBody{}
	onlineListBody.UserName = UserName
	onlineListBody.Status = status
	b, _ := json.Marshal(onlineListBody)
	pack.Body = b
	pack.Header.Method = PackManager.UpdateOnlineUser
	pack.Header.MethodType = 0
	data := pack.CreateBuffer()
	userStatus := userCache.GetUserLoginStatusMap()
	userAddress := userCache.GetUserLoginAddressMap()
	for k, v := range userStatus {
		if v == 1 {
			ClientManagerHandle.SendToConn(userAddress[k], data)
		}
	}

}

func SendOnlineUserList(UserName string, conn net.Conn) (requestPack *PackManager.Pack) {
	status := userCache.GetUserLoginStatus(UserName)
	if status != UserCache.LoginStatus {
		return nil
	}
	onlineList := userCache.GetOnlineUsers()
	b := []byte(onlineList)
	resPack := PackManager.Pack{}
	resPack.Header.Method = PackManager.OnlineUserList
	resPack.Header.MethodType = 0
	resPack.Body = b
	data := createSendBuffer(resPack)
	conn.Write(data)
	return nil
}
