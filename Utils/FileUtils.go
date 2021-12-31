package Utils

import (
	"fmt"
	"os"
	"testserver/PackManager"
	"time"
)

var FileChanMap map[string]chan *PackManager.FileBody
var FileStopChanMap map[string]chan bool

// AliveFileWrite 实现简单的channel  防止频繁开关文件
func AliveFileWrite(pack chan *PackManager.FileBody, quit chan bool, fileName string, fileMD5 string) bool {
	//打开文件，新建文件
	f, err := os.Create("./" + fileMD5 + "_" + fileName) //传递文件路径
	if err != nil {
		fmt.Println("err = ", err)
		return false
	}
	defer f.Close()
	for {
		select {
		case filePack := <-pack:
			if filePack != nil {
				seek, err := f.Seek(int64(filePack.CurrentSize), 0)
				if err != nil || seek != int64(filePack.CurrentSize) {
					return false
				}
				f.Write(filePack.FileData)
				fmt.Println(filePack.FileName)
			} else {
				fmt.Println("file data error")
				return true
			}
		case <-time.After(time.Second * 3): //三秒内没反应退出协程
			fmt.Println(fileMD5 + fileName + " timeout ")
			return false
		case <-quit:
			fmt.Println(fileMD5 + fileName + " quit")
			return true
		}
	}

}
