package Network

import (
	"bytes"
	"encoding/binary"
)

// Header 协议头
type Header struct {
	//包大小
	packSize int
	//指令
	method int
	//指令类别
	methodType int
}

// Pack 协议体
type Pack struct {
	header Header
	//数据体
	body []byte
}

//整形转换成字节
func IntToBytes(n int) []byte {
	x := int32(n)
	bytesBuffer := bytes.NewBuffer([]byte{})
	binary.Write(bytesBuffer, binary.BigEndian, x)
	return bytesBuffer.Bytes()
}

//字节转换成整形
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

func Decode(b []byte) (proto *Pack) {
	var pack Pack
	size := b[:4]
	sizeInt := binary.BigEndian.Uint32(size)
	pack.header.packSize = int(sizeInt)
	cmd := b[4:8]
	cmdInt := binary.BigEndian.Uint32(cmd)
	pack.header.method = int(cmdInt)
	cmdType := b[8:12]
	cmdTypeInt := binary.BigEndian.Uint32(cmdType)
	pack.header.methodType = int(cmdTypeInt)
	pack.body = b[12:sizeInt]

	proto = &pack
	return proto
}
