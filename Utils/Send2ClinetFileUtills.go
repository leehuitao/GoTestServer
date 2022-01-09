package Utils

import (
	"bytes"
	"encoding/binary"
	"fmt"
	"io/ioutil"
	"net"
	"os"
	"testserver/PackManager"
	"time"
)

func intToBytes(n int) []byte {
	x := int32(n)
	bytesBuffer := bytes.NewBuffer([]byte{})
	binary.Write(bytesBuffer, binary.BigEndian, x)
	return bytesBuffer.Bytes()
}

// AliveFileSend 实现简单的channel  防止频繁开关文件
func AliveFileSend(send chan bool, quit chan bool, fileName string, fileMD5 string, sendUserName string, conn net.Conn) bool {
	//打开文件，新建文件
	content, readFileErr := ioutil.ReadFile("./" + fileMD5 + "_" + fileName)
	if readFileErr == nil {
		fmt.Println("file size is ", len(content))

	}
	f, err := os.OpenFile("./"+fileMD5+"_"+fileName, os.O_RDONLY, os.ModePerm)
	if err != nil {
		fmt.Println("err = ", err)
		return false
	}
	readSize := 1024 * 8
	currentSeek := 0

	defer f.Close()
	for {
		select {
		case <-send:
			buf := make([]byte, readSize)
			read, readErr := f.Read(buf)
			if readErr != nil {
				return false
			} else {
				currentSeek = currentSeek + read
				var bodyBuffer bytes.Buffer //Buffer是一个实现了读写方法的可变大小的字节缓冲
				//bodyBuffer.Write(intToBytes(len(fileNameBuffer)))
				//bodyBuffer.Write(fileNameBuffer)
				//bodyBuffer.Write(intToBytes(len(fileMD5Buffer)))
				//bodyBuffer.Write(fileMD5Buffer)
				//bodyBuffer.Write(intToBytes(len(sendUserNameBuffer)))
				//bodyBuffer.Write(sendUserNameBuffer)
				bodyBuffer.Write(buf[:read])
				var buffer bytes.Buffer //Buffer是一个实现了读写方法的可变大小的字节缓冲
				buffer.Write(intToBytes(PackManager.HeaderSize + len(bodyBuffer.Bytes())))
				if currentSeek != len(content) {
					buffer.Write(intToBytes(PackManager.ContinueSend2File))
				} else {
					buffer.Write(intToBytes(PackManager.SendFileSuccess))
				}
				buffer.Write(intToBytes(0))
				buffer.Write(bodyBuffer.Bytes())
				conn.Write(buffer.Bytes())
				fmt.Println("read data size ", read)
			}
		case <-time.After(time.Second * 3): //三秒内没反应退出协程
			fmt.Println(fileMD5 + fileName + " timeout ")
			return false
		case <-quit:
			fmt.Println(fileMD5 + fileName + "received complete channel quit")
			f.Close()
			return true
		}
	}

}
