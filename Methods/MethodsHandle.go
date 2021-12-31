package Methods

import (
	"encoding/json"
	"testserver/PackManager"
	"testserver/Utils"
)

var newChannelCache = Utils.NewChannelCache()

// SendLogin 登录
func SendLogin(pack *PackManager.Pack) (requestPack *PackManager.Pack) {
	loginBody := PackManager.LoginBody{}
	if err := json.Unmarshal(pack.Body, &loginBody); err != nil {
		return nil
	}
	return pack
}

// SendLogout 登出
func SendLogout(pack *PackManager.Pack) (requestPack *PackManager.Pack) {
	loginBody := PackManager.LoginBody{}
	if err := json.Unmarshal(pack.Body, &loginBody); err != nil {
		return nil
	}
	return pack
}

// SendMsg 发送文字消息
func SendMsg(pack *PackManager.Pack) (requestPack *PackManager.Pack) {
	msgBody := PackManager.MsgBody{}
	if err := json.Unmarshal(pack.Body, &msgBody); err != nil {
		return nil
	}
	return pack
}

// SendStartFile 开始发送文件
func SendStartFile(pack *PackManager.Pack) (requestPack *PackManager.Pack) {
	fileBody := PackManager.FileBody{}
	if err := json.Unmarshal(pack.Body, &fileBody); err != nil {
		return nil
	}
	fileChannel := make(chan PackManager.FileBody, 5)
	quitChannel := make(chan bool)
	go Utils.AliveFileWrite(fileChannel, quitChannel, fileBody.FileName, fileBody.FileMD5)
	newChannelCache.AddNewChannelCache(fileBody.FileMD5, fileChannel, quitChannel)

	return pack
}

// SendFileData  发送文件数据
func SendFileData(pack *PackManager.Pack) (requestPack *PackManager.Pack) {
	fileBody := PackManager.FileBody{}
	if err := json.Unmarshal(pack.Body, &fileBody); err != nil {
		return nil
	}

	newChannelCache.FileChanMap[fileBody.FileMD5] <- fileBody

	return pack
}

// SendFileCancel 文件发送取消
func SendFileCancel(pack *PackManager.Pack) (requestPack *PackManager.Pack) {
	fileBody := PackManager.FileBody{}
	if err := json.Unmarshal(pack.Body, &fileBody); err != nil {
		return nil
	}

	newChannelCache.FileStopChanMap[fileBody.FileMD5] <- true
	newChannelCache.ClearChannelCache(fileBody.FileMD5)

	return pack
}

// SendFileEnd 发送文件完成
func SendFileEnd(pack *PackManager.Pack) (requestPack *PackManager.Pack) {
	fileBody := PackManager.FileBody{}
	if err := json.Unmarshal(pack.Body, &fileBody); err != nil {
		return nil
	}
	newChannelCache.FileChanMap[fileBody.FileMD5] <- fileBody
	newChannelCache.FileStopChanMap[fileBody.FileMD5] <- true
	newChannelCache.ClearChannelCache(fileBody.FileMD5)

	return pack
}
