package Methods

import (
	"bytes"
	"encoding/binary"
	"encoding/json"
	"fmt"
	"net"
	"testserver/PackManager"
	"testserver/Utils"
)

var newChannelCache = Utils.NewChannelCache()
var userCache = PackManager.NewUserCache()

func IntToBytes(n int) []byte {
	x := int32(n)
	bytesBuffer := bytes.NewBuffer([]byte{})
	binary.Write(bytesBuffer, binary.BigEndian, x)
	return bytesBuffer.Bytes()
}

// SendLogin 登录
func SendLogin(pack *PackManager.Pack, conn net.Conn) (requestPack *PackManager.Pack) {
	loginBody := PackManager.LoginBody{}
	if err := json.Unmarshal(pack.Body, &loginBody); err != nil {
		return nil
	}
	fmt.Println(loginBody)

	password, err := userCache.GetUserPassword(loginBody.UserName)

	resPack := PackManager.Pack{}
	resPack.Header.PackSize = 12
	resPack.Header.Method = PackManager.Login
	resPack.Header.MethodType = 0

	if err != nil {
		loginBody.LoginStatus = 0
	}
	if loginBody.PassWord == password {
		loginBody.LoginStatus = 1
		userCache.UpdateUserLoginStatus(loginBody.UserName, PackManager.LoginStatus)
	} else {
		loginBody.LoginStatus = 0
	}
	b, _ := json.Marshal(loginBody)
	resPack.Body = b
	var buffer bytes.Buffer //Buffer是一个实现了读写方法的可变大小的字节缓冲
	buffer.Write(IntToBytes(12 + len(b)))
	buffer.Write(IntToBytes(PackManager.Login))
	buffer.Write(IntToBytes(0))
	buffer.Write(b)

	println(buffer.Bytes())
	conn.Write(buffer.Bytes())
	return pack
}

// SendLogout 登出
func SendLogout(pack *PackManager.Pack, conn net.Conn) (requestPack *PackManager.Pack) {
	loginBody := PackManager.LoginBody{}
	if err := json.Unmarshal(pack.Body, &loginBody); err != nil {
		return nil
	}
	status := userCache.GetUserLoginStatus(loginBody.UserName)
	if status != PackManager.LoginStatus {
		return nil
	}
	userCache.UpdateUserLoginStatus(loginBody.UserName, PackManager.LogoffStatus)
	return pack
}

// SendMsg 发送文字消息
func SendMsg(pack *PackManager.Pack, conn net.Conn) (requestPack *PackManager.Pack) {
	msgBody := PackManager.MsgBody{}
	if err := json.Unmarshal(pack.Body, &msgBody); err != nil {
		return nil
	}
	status := userCache.GetUserLoginStatus(msgBody.UserName)
	if status != PackManager.LoginStatus {
		return nil
	}
	return pack
}

// SendStartFile 开始发送文件
func SendStartFile(pack *PackManager.Pack, conn net.Conn) (requestPack *PackManager.Pack) {
	fileBody := PackManager.FileBody{}
	if err := json.Unmarshal(pack.Body, &fileBody); err != nil {
		return nil
	}
	status := userCache.GetUserLoginStatus(fileBody.UserName)
	if status != PackManager.LoginStatus {
		return nil
	}
	fileChannel := make(chan PackManager.FileBody, 5)
	quitChannel := make(chan bool)
	go Utils.AliveFileWrite(fileChannel, quitChannel, fileBody.FileName, fileBody.FileMD5)
	newChannelCache.AddNewChannelCache(fileBody.FileMD5, fileChannel, quitChannel)

	return pack
}

// SendFileData  发送文件数据
func SendFileData(pack *PackManager.Pack, conn net.Conn) (requestPack *PackManager.Pack) {
	fileBody := PackManager.FileBody{}
	if err := json.Unmarshal(pack.Body, &fileBody); err != nil {
		return nil
	}
	status := userCache.GetUserLoginStatus(fileBody.UserName)
	if status != PackManager.LoginStatus {
		return nil
	}
	newChannelCache.FileChanMap[fileBody.FileMD5] <- fileBody

	return pack
}

// SendFileCancel 文件发送取消
func SendFileCancel(pack *PackManager.Pack, conn net.Conn) (requestPack *PackManager.Pack) {
	fileBody := PackManager.FileBody{}
	if err := json.Unmarshal(pack.Body, &fileBody); err != nil {
		return nil
	}
	status := userCache.GetUserLoginStatus(fileBody.UserName)
	if status != PackManager.LoginStatus {
		return nil
	}
	newChannelCache.FileStopChanMap[fileBody.FileMD5] <- true
	newChannelCache.ClearChannelCache(fileBody.FileMD5)

	return pack
}

// SendFileEnd 发送文件完成
func SendFileEnd(pack *PackManager.Pack, conn net.Conn) (requestPack *PackManager.Pack) {
	fileBody := PackManager.FileBody{}
	if err := json.Unmarshal(pack.Body, &fileBody); err != nil {
		return nil
	}
	status := userCache.GetUserLoginStatus(fileBody.UserName)
	if status != PackManager.LoginStatus {
		return nil
	}
	newChannelCache.FileChanMap[fileBody.FileMD5] <- fileBody
	newChannelCache.FileStopChanMap[fileBody.FileMD5] <- true
	newChannelCache.ClearChannelCache(fileBody.FileMD5)

	return pack
}
