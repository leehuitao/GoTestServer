package UserCache

import (
	"errors"
	"testserver/DBIO/MysqlManager"
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
	m := MysqlManager.Select("select * from userinfo")
	for k, v := range m {
		body := PackManager.LoginBody{}
		body.UserName = v.UserName
		body.PassWord = v.PassWord
		body.MacAddress = v.MacAddress
		body.UserLoginName = v.UserLoginName
		cache.userCacheMap[k] = body
	}
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

func (userCache *UserCache) GetUserPassword(userLoginName string) (string, error) {
	body, exist := userCache.userCacheMap[userLoginName]

	if exist {
		return body.PassWord, nil
	}

	return "", errors.New("user does not exist")
}

func (userCache *UserCache) GetUserName(userLoginName string) (string, error) {
	body, exist := userCache.userCacheMap[userLoginName]

	if exist {
		return body.UserName, nil
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

func (userCache *UserCache) UpdateUserLoginStatus(userLoginName string, status int) {
	userCache.userLoginStatus[userLoginName] = status
}

func (userCache *UserCache) GetUserLoginStatus(userLoginName string) int {
	status, exist := userCache.userLoginStatus[userLoginName]

	if exist {
		return status
	}

	return LogoffStatus
}
