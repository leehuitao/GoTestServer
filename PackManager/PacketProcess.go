package PackManager

import (
	"bytes"
	"encoding/binary"
)

// Header -----------------一级协议包--------------------
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
	DstUser   string `json:"DstUser"`
	DstUserID int    `json:"DstUserID"`
	MsgType   int    `json:"MsgType"`
	Msg       string `json:"Msg"`
}
type FileBody struct {
	FileName    string `json:"FileName"`
	FileMD5     string `json:"FileMD5"`
	TotalSize   int    `json:"TotalSize"`
	CurrentSize int    `json:"CurrentSize"`
	DstUserID   int    `json:"DstUserID"`
	SendStatus  int    `json:"SendStatus"`
	FileData    []byte `json:"FileData"`
}

// LoginBody  登录包json
type LoginBody struct {
	UserName string `json:"UserName"`
	PassWord string `json:"PassWord"`
	Notice   bool   `json:"Notice"`
}

// SystemBody  系统指令json
type SystemBody struct {
	SystemCMD string `json:"SystemCMD"`
}

// GroupBody  群组指令json
type GroupBody struct {
	DstGroupId string `json:"DstGroupId"`
	Type       string `json:"Type"`
	Msg        string `json:"Msg"`
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
	pack.Header.Method = int(cmdInt)
	cmdType := b[start+8 : start+12]
	cmdTypeInt := binary.BigEndian.Uint32(cmdType)
	pack.Header.MethodType = int(cmdTypeInt)
	pack.Body = b[start+12 : start+int(sizeInt)]

	proto = &pack
	return proto
}
