package Methods

import (
	"encoding/binary"
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
	password, err := userCache.GetUserPassword(loginBody.UserLoginName)

	resPack := PackManager.Pack{}
	resPack.Header.Method = pack.Header.Method
	resPack.Header.MethodType = 0

	if err != nil {
		loginBody.LoginStatus = -1
	}
	if loginBody.PassWord == password {
		loginBody.LoginStatus = 1
		userName, err := userCache.GetUserName(loginBody.UserLoginName)
		if err != nil {
			return nil
		}
		loginBody.UserName = userName
		NoticeAllOnlineUserChangeStatus(loginBody.UserLoginName, UserCache.LoginStatus)
		userCache.UpdateUserLoginStatus(loginBody.UserLoginName, UserCache.LoginStatus)
		userCache.AddUserCache(loginBody.UserLoginName, loginBody, conn.RemoteAddr().String())
	} else {
		loginBody.LoginStatus = 0
	}
	b, _ := json.Marshal(loginBody)
	resPack.Body = b
	data := createSendBuffer(resPack)
	conn.Write(data)
	return pack
}

// SendLogout 登出
func SendLogout(pack *PackManager.Pack, conn net.Conn) (requestPack *PackManager.Pack) {
	loginBody := PackManager.LoginBody{}
	if err := json.Unmarshal(pack.Body, &loginBody); err != nil {
		return nil
	}
	status := userCache.GetUserLoginStatus(loginBody.UserLoginName)
	if status != UserCache.LoginStatus {
		return nil
	}
	userCache.DelUserCache(loginBody.UserLoginName)
	resPack := PackManager.Pack{}
	resPack.Header.Method = pack.Header.Method
	resPack.Header.MethodType = 0
	resPack.Body = []byte{}
	data := createSendBuffer(resPack)
	conn.Write(data)
	NoticeAllOnlineUserChangeStatus(loginBody.UserLoginName, UserCache.LogoffStatus)
	return pack
}

// GetDeptOrg 发送组织架构
func GetDeptOrg(pack *PackManager.Pack, conn net.Conn) (requestPack *PackManager.Pack) {
	sysBody := PackManager.SystemBody{}
	if err := json.Unmarshal(pack.Body, &sysBody); err != nil {
		return nil
	}
	status := userCache.GetUserLoginStatus(sysBody.UserLoginName)
	if status != UserCache.LoginStatus {
		return nil
	}

	pack.Body = UserCache.OrgCacheData.GetJson()
	data := createSendBuffer(*pack)
	conn.Write(data)
	return pack
}

// GetUserDeptOrg 发送组织架构
func GetUserDeptOrg(pack *PackManager.Pack, conn net.Conn) (requestPack *PackManager.Pack) {
	sysBody := PackManager.SystemBody{}
	if err := json.Unmarshal(pack.Body, &sysBody); err != nil {
		return nil
	}
	status := userCache.GetUserLoginStatus(sysBody.UserLoginName)
	if status != UserCache.LoginStatus {
		return nil
	}

	pack.Body = userCache.GetJson()
	data := createSendBuffer(*pack)
	conn.Write(data)
	return pack
}

// GetOnlineUsers 发送在线人员列表
func GetOnlineUsers(pack *PackManager.Pack, conn net.Conn) (requestPack *PackManager.Pack) {
	sysBody := PackManager.SystemBody{}
	if err := json.Unmarshal(pack.Body, &sysBody); err != nil {
		return nil
	}
	status := userCache.GetUserLoginStatus(sysBody.UserLoginName)
	if status != UserCache.LoginStatus {
		return nil
	}

	SendOnlineUserList(sysBody.UserLoginName, conn)

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
	fileChannel := make(chan []byte, 5)
	quitChannel := make(chan bool)
	go Utils.AliveFileWrite(fileChannel, quitChannel, fileBody.FileName, fileBody.FileMD5)
	newChannelCache.AddNewChannelCache(fileBody.FileMD5, fileChannel, quitChannel)
	pack.Header.Method = PackManager.SendFileData
	data := createSendBuffer(*pack)
	conn.Write(data)
	return pack
}

// SendFileData  发送文件数据
func SendFileData(pack *PackManager.Pack, conn net.Conn) (requestPack *PackManager.Pack) {
	nameSizeData := pack.Body[0:4]
	nameSize := binary.BigEndian.Uint32(nameSizeData)
	md5SizeData := pack.Body[4:8]
	md5Size := binary.BigEndian.Uint32(md5SizeData)
	nameData := pack.Body[8 : 8+nameSize]
	name := string(nameData)
	md5Data := pack.Body[8+nameSize : 8+nameSize+md5Size]
	md5 := string(md5Data)
	status := userCache.GetUserLoginStatus(name)
	if status != UserCache.LoginStatus {
		return nil
	}

	fileData := pack.Body[8+nameSize+md5Size:]
	newChannelCache.FileChanMap[md5] <- fileData
	pack.Header.Method = PackManager.SendFileData
	data := createSendBuffer(*pack)
	conn.Write(data)
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

// SendFileEnd 发送文件完成开始发送到对方客户端
func SendFileEnd(pack *PackManager.Pack, conn net.Conn) (requestPack *PackManager.Pack) {
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
	sendFileChannel := make(chan bool)
	sendQuitChannel := make(chan bool)
	getConn := ClientManagerHandle.GetConn(fileBody.DstUserName)
	if getConn.conn != nil {
		pack.Header.Method = PackManager.StartSendFile
		data := createSendBuffer(*pack)
		getConn.conn.Write(data)
		go Utils.AliveFileSend(sendFileChannel, sendQuitChannel, fileBody.FileName, fileBody.FileMD5, fileBody.UserName, getConn.conn)
		newChannelCache.AddNewSendChannelCache(fileBody.FileMD5, sendFileChannel, sendQuitChannel)
	}
	return pack
}

// SendFileContinue 文件发送继续
func SendFileContinue(pack *PackManager.Pack, conn net.Conn) (requestPack *PackManager.Pack) {
	fileBody := PackManager.FileBody{}
	if err := json.Unmarshal(pack.Body, &fileBody); err != nil {
		return nil
	}
	status := userCache.GetUserLoginStatus(fileBody.UserName)
	if status != UserCache.LoginStatus {
		return nil
	}
	newChannelCache.SendFileChanMap[fileBody.FileMD5] <- true
	return pack
}
