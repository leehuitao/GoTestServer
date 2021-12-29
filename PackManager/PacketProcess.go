package PackManager

import (
	"bytes"
	"encoding/binary"
)

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
