package Utils

import (
	"fmt"
	"os"
	"testserver/PackManager"
	"time"
)

type ChannelCache struct {
	FileChanMap      map[string]chan PackManager.FileBody
	FileStopChanMap  map[string]chan bool
	conCurrentNumber int
}

func NewChannelCache() (Cache *ChannelCache) {
	Cache = &ChannelCache{
		make(map[string]chan PackManager.FileBody),
		make(map[string]chan bool),
		0,
	}
	return Cache
}

func (channelCache *ChannelCache) AddNewChannelCache(fileMd5 string, write chan PackManager.FileBody, quit chan bool) {
	channelCache.FileChanMap[fileMd5] = write
	channelCache.FileStopChanMap[fileMd5] = quit
	channelCache.conCurrentNumber = channelCache.conCurrentNumber + 1
}

func (channelCache *ChannelCache) ClearChannelCache(fileMd5 string) {
	delete(channelCache.FileChanMap, fileMd5)
	delete(channelCache.FileStopChanMap, fileMd5)
	channelCache.conCurrentNumber = channelCache.conCurrentNumber - 1
}

// AliveFileWrite 实现简单的channel  防止频繁开关文件
func AliveFileWrite(pack chan PackManager.FileBody, quit chan bool, fileName string, fileMD5 string) bool {
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
			if &filePack != nil {
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
