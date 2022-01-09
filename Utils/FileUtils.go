package Utils

import (
	"fmt"
	"os"
	"time"
)

type ChannelCache struct {
	FileChanMap         map[string]chan []byte
	FileStopChanMap     map[string]chan bool
	SendFileChanMap     map[string]chan bool
	SendFileStopChanMap map[string]chan bool
	conCurrentNumber    int
}

func NewChannelCache() (Cache *ChannelCache) {
	Cache = &ChannelCache{
		make(map[string]chan []byte),
		make(map[string]chan bool),
		make(map[string]chan bool),
		make(map[string]chan bool),
		0,
	}
	return Cache
}

func (channelCache *ChannelCache) AddNewChannelCache(fileMd5 string, write chan []byte, quit chan bool) {
	channelCache.FileChanMap[fileMd5] = write
	channelCache.FileStopChanMap[fileMd5] = quit
	channelCache.conCurrentNumber = channelCache.conCurrentNumber + 1
}

func (channelCache *ChannelCache) ClearChannelCache(fileMd5 string) {
	delete(channelCache.FileChanMap, fileMd5)
	delete(channelCache.FileStopChanMap, fileMd5)
	channelCache.conCurrentNumber = channelCache.conCurrentNumber - 1
}

func (channelCache *ChannelCache) AddNewSendChannelCache(fileMd5 string, send chan bool, quit chan bool) {
	channelCache.SendFileChanMap[fileMd5] = send
	channelCache.SendFileStopChanMap[fileMd5] = quit
	channelCache.conCurrentNumber = channelCache.conCurrentNumber + 1
}

func (channelCache *ChannelCache) ClearSendChannelCache(fileMd5 string) {
	delete(channelCache.SendFileChanMap, fileMd5)
	delete(channelCache.SendFileStopChanMap, fileMd5)
	channelCache.conCurrentNumber = channelCache.conCurrentNumber - 1
}

// AliveFileWrite 实现简单的channel  防止频繁开关文件
func AliveFileWrite(pack chan []byte, quit chan bool, fileName string, fileMD5 string) bool {
	//打开文件，新建文件
	os.Truncate("./"+fileMD5+"_"+fileName, 0)
	f, err := os.OpenFile("./"+fileMD5+"_"+fileName, os.O_WRONLY|os.O_APPEND|os.O_CREATE, os.ModePerm)
	if err != nil {
		fmt.Println("err = ", err)
		return false
	}
	currentSeek := 0
	defer f.Close()
	for {
		select {
		case filePack := <-pack:
			if &filePack != nil {
				seek, _ := f.Seek(int64(currentSeek), 0)
				write, err := f.Write(filePack)
				if err != nil {
					return false
				} else {
					currentSeek = currentSeek + int(seek)
					fmt.Println("write data size ", write)
				}
			} else {
				fmt.Println("file data error")
				return true
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
