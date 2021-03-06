package PackManager

import (
	"bytes"
	"encoding/binary"
	"strconv"
	"testserver/LogService"
)

const (
	Login             = 100
	Logout            = 101
	Msg               = 102
	StartSendFile     = 103
	SendFileData      = 104
	SendFileCancel    = 105
	SendFileSuccess   = 106
	ContinueSend2File = 107
	OnlineUserList    = 200
	UpdateOnlineUser  = 201
	GetOrg            = 300
	GetOnlineUser     = 301
	GetUserOrg        = 302
)

// Header -----------------一级协议包--------------------
const HeaderSize = 12

// Header 协议头
type Header struct {
	//包大小
	PackSize int
	//指令
	Method int
	//指令类别
	MethodType int
}

// Pack 协议体
type Pack struct {
	Header Header
	//数据体
	Body []byte
}

// MsgBody -----------------二级协议包--------------------
// MsgBody 普通消息的json类型
type MsgBody struct {
	UserName      string `json:"UserName"`
	UserLoginName string `json:"UserLoginName"`
	DstUser       string `json:"DstUser"`
	DstUserID     int    `json:"DstUserID"`
	MsgType       int    `json:"MsgType"`
	Msg           string `json:"Msg"`
}
type FileBody struct {
	UserName      string `json:"UserName"`
	UserLoginName string `json:"UserLoginName"`
	FileName      string `json:"FileName"`
	FileMD5       string `json:"FileMD5"`
	TotalSize     int    `json:"TotalSize"`
	CurrentSize   int    `json:"CurrentSize"`
	DstUserName   string `json:"DstUserName"`
	SendStatus    int    `json:"SendStatus"`
	FileData      []byte `json:"FileData"`
}

// LoginBody  登录包json
type LoginBody struct {
	UserName      string `json:"UserName"`
	UserLoginName string `json:"UserLoginName"`
	PassWord      string `json:"PassWord"`
	Notice        bool   `json:"Notice"`
	MacAddress    string `json:"MacAddress"`
	LoginTime     string `json:"LoginTime"`
	LoginStatus   int    `json:"LoginStatus"`
	ParentDeptID  int    `json:"ParentDeptID"`
}

// SystemBody  系统指令json
type SystemBody struct {
	UserLoginName string `json:"UserLoginName"`
	SystemCMD     string `json:"SystemCMD"`
}

// GroupBody  群组指令json
type GroupBody struct {
	UserLoginName string `json:"UserLoginName"`
	DstGroupId    string `json:"DstGroupId"`
	Type          string `json:"Type"`
	Msg           string `json:"Msg"`
}

// OnlineListBody  用户状态更新json
type OnlineListBody struct {
	UserLoginName string `json:"UserLoginName"`
	Status        int    `json:"Status"`
}

// IntToBytes 整形转换成字节
func IntToBytes(n int) []byte {
	x := int32(n)
	bytesBuffer := bytes.NewBuffer([]byte{})
	binary.Write(bytesBuffer, binary.BigEndian, x)
	return bytesBuffer.Bytes()
}

// BytesToInt 字节转换成整形
func BytesToInt(b []byte) int {
	bytesBuffer := bytes.NewBuffer(b)

	var x int32
	binary.Read(bytesBuffer, binary.BigEndian, &x)

	return int(x)
}

func (proto *Pack) CreateBuffer() (data []byte) {
	buf := new(bytes.Buffer)
	if err := binary.Write(buf, binary.LittleEndian, proto); err != nil {
		return nil
	}
	return buf.Bytes()
}

func Decode(start int, b []byte) (proto *Pack) {
	var pack Pack
	size := b[start : start+4]
	sizeInt := binary.BigEndian.Uint32(size)
	pack.Header.PackSize = int(sizeInt)
	cmd := b[start+4 : start+8]
	cmdInt := binary.BigEndian.Uint32(cmd)
	cmdLog := "rec cmd = " + strconv.Itoa(int(cmdInt))
	LogService.LogDebug(cmdLog)
	pack.Header.Method = int(cmdInt)
	cmdType := b[start+8 : start+12]
	cmdTypeInt := binary.BigEndian.Uint32(cmdType)
	pack.Header.MethodType = int(cmdTypeInt)
	pack.Body = b[start+12 : start+int(sizeInt)]

	proto = &pack
	return proto
}
