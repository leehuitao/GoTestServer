package UserCache

import (
	"errors"
	"testserver/PackManager"
)

type UserCache struct {
	userCacheMap     map[string]PackManager.LoginBody
	loginNumber      int
	userLoginStatus  map[string]int
	userLoginAddress map[string]string
}

const (
	LogoffStatus = 0
	LoginStatus  = 1
)

func NewUserCache() (cache *UserCache) {
	cache = &UserCache{
		make(map[string]PackManager.LoginBody),
		0,
		make(map[string]int),
		make(map[string]string),
	}
	body := PackManager.LoginBody{}
	body.UserName = "test"
	body.PassWord = "test"
	cache.userCacheMap["test"] = body
	body.UserName = "test1"
	body.PassWord = "test1"
	cache.userCacheMap["test1"] = body
	return cache

}
func (userCache *UserCache) GetOnlineUsers() string {
	keys := ""
	for k, v := range userCache.userLoginStatus {
		if v == LoginStatus {
			keys = keys + "," + k
		}
	}
	return keys
}

func (userCache *UserCache) GetUserLoginStatusMap() map[string]int {
	return userCache.userLoginStatus
}

func (userCache *UserCache) GetUserLoginAddressMap() map[string]string {
	return userCache.userLoginAddress
}

func (userCache *UserCache) GetUserPassword(userName string) (string, error) {
	body, exist := userCache.userCacheMap[userName]

	if exist {
		return body.PassWord, nil
	}

	return "", errors.New("user does not exist")
}

func (userCache *UserCache) AddUserCache(userId string, LoginCache PackManager.LoginBody, Address string) {
	userCache.userCacheMap[userId] = LoginCache
	userCache.userLoginAddress[userId] = Address
}

func (userCache *UserCache) DelUserCache(userId string) {
	delete(userCache.userLoginStatus, userId)
	delete(userCache.userLoginAddress, userId)
}

func (userCache *UserCache) GetUserNameFromIpPort(ipPort string) string {
	var userName string
	for k, v := range userCache.userLoginAddress {
		if v == ipPort {
			userName = k
		}
	}
	return userName
}

func (userCache *UserCache) DelUserCacheForIpPort(ip string) {

	var userName string
	for k, v := range userCache.userLoginAddress {
		if v == ip {
			userName = k
		}
	}
	delete(userCache.userLoginStatus, userName)
	delete(userCache.userLoginAddress, userName)
}

func (userCache *UserCache) UpdateUserLoginStatus(userName string, status int) {
	userCache.userLoginStatus[userName] = status
}

func (userCache *UserCache) GetUserLoginStatus(userName string) int {
	status, exist := userCache.userLoginStatus[userName]

	if exist {
		return status
	}

	return LogoffStatus
}
