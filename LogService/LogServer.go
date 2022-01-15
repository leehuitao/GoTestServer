package LogService

import (
	"fmt"
	"log"
	"os"
)

type fileLog string

var debugChannel chan string
var errorChannel chan string

func StartLogService(fileName string) {
	debugChannel = make(chan string, 100)
	errorChannel = make(chan string, 100)
	go goLogDebug(fileName)
	go goLogError(fileName)
}

func LogDebug(log string) {
	debugChannel <- log
}

func LogError(log string) {
	errorChannel <- log
}

func goLogDebug(fileName string) {
	debugLogFile, err := os.OpenFile(fileName+"_debug.log", os.O_CREATE|os.O_WRONLY|os.O_APPEND, 0600)
	if err != nil {
		return
	}
	logger := log.New(debugLogFile, " debug ", log.LstdFlags)
	defer debugLogFile.Close()
	for {
		select {
		case data := <-debugChannel:
			logger.Println(data + "")
			fmt.Println(data)
		}
	}
}

func goLogError(fileName string) {
	errorLogFile, err := os.OpenFile(fileName+"_error.log", os.O_CREATE|os.O_WRONLY|os.O_APPEND, 0600)
	if err != nil {
		return
	}
	logger := log.New(errorLogFile, " error ", log.LstdFlags)
	defer errorLogFile.Close()
	for {
		select {
		case data := <-errorChannel:
			logger.Println(data)
			fmt.Println(data)
		}
	}
}
