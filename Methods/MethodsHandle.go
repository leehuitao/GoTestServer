package Methods

import (
	"encoding/json"
	"testserver/PackManager"
)

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
	return pack
}

// SendFileData  发送文件数据
func SendFileData(pack *PackManager.Pack) (requestPack *PackManager.Pack) {
	fileBody := PackManager.FileBody{}
	if err := json.Unmarshal(pack.Body, &fileBody); err != nil {
		return nil
	}
	return pack
}

// SendFileCancel 文件发送取消
func SendFileCancel(pack *PackManager.Pack) (requestPack *PackManager.Pack) {
	fileBody := PackManager.FileBody{}
	if err := json.Unmarshal(pack.Body, &fileBody); err != nil {
		return nil
	}
	return pack
}

// SendFileEnd 发送文件完成
func SendFileEnd(pack *PackManager.Pack) (requestPack *PackManager.Pack) {
	fileBody := PackManager.FileBody{}
	if err := json.Unmarshal(pack.Body, &fileBody); err != nil {
		return nil
	}
	return pack
}
