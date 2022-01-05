package Methods

import (
	"encoding/json"
	"net"
	"testserver/PackManager"
	"testserver/UserCache"
	"testserver/Utils"
)

// SendLogin 登录
func SendLogin(pack *PackManager.Pack, conn net.Conn) (requestPack *PackManager.Pack) {
	loginBody := PackManager.LoginBody{}
	if err := json.Unmarshal(pack.Body, &loginBody); err != nil {
		return nil
	}
	password, err := userCache.GetUserPassword(loginBody.UserName)

	resPack := PackManager.Pack{}
	resPack.Header.Method = pack.Header.Method
	resPack.Header.MethodType = 0

	if err != nil {
		loginBody.LoginStatus = -1
	}
	if loginBody.PassWord == password {
		loginBody.LoginStatus = 1
		NoticeAllOnlineUserChangeStatus(loginBody.UserName, UserCache.LoginStatus)
		userCache.UpdateUserLoginStatus(loginBody.UserName, UserCache.LoginStatus)
		userCache.AddUserCache(loginBody.UserName, loginBody, conn.RemoteAddr().String())
	} else {
		loginBody.LoginStatus = 0
	}
	b, _ := json.Marshal(loginBody)
	resPack.Body = b
	data := createSendBuffer(resPack)
	conn.Write(data)
	SendOnlineUserList(loginBody.UserName, conn)
	return pack
}

// SendLogout 登出
func SendLogout(pack *PackManager.Pack, conn net.Conn) (requestPack *PackManager.Pack) {
	loginBody := PackManager.LoginBody{}
	if err := json.Unmarshal(pack.Body, &loginBody); err != nil {
		return nil
	}
	status := userCache.GetUserLoginStatus(loginBody.UserName)
	if status != UserCache.LoginStatus {
		return nil
	}
	userCache.DelUserCache(loginBody.UserName)
	resPack := PackManager.Pack{}
	resPack.Header.Method = pack.Header.Method
	resPack.Header.MethodType = 0
	resPack.Body = []byte{}
	data := createSendBuffer(resPack)
	conn.Write(data)
	NoticeAllOnlineUserChangeStatus(loginBody.UserName, UserCache.LogoffStatus)
	return pack
}

// SendMsg 发送文字消息
func SendMsg(pack *PackManager.Pack, conn net.Conn) (requestPack *PackManager.Pack) {
	msgBody := PackManager.MsgBody{}
	if err := json.Unmarshal(pack.Body, &msgBody); err != nil {
		return nil
	}
	status := userCache.GetUserLoginStatus(msgBody.UserName)
	if status != UserCache.LoginStatus {
		return nil
	}

	getConn := ClientManagerHandle.GetConn(msgBody.DstUser)
	data := createSendBuffer(*pack)
	if getConn.conn != nil {
		getConn.conn.Write(data)
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
	if status != UserCache.LoginStatus {
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
	if status != UserCache.LoginStatus {
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
	if status != UserCache.LoginStatus {
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
	if status != UserCache.LoginStatus {
		return nil
	}
	newChannelCache.FileChanMap[fileBody.FileMD5] <- fileBody
	newChannelCache.FileStopChanMap[fileBody.FileMD5] <- true
	newChannelCache.ClearChannelCache(fileBody.FileMD5)

	return pack
}
