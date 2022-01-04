package PackManager

import "errors"

type UserCache struct {
	userCacheMap    map[string]LoginBody
	loginNumber     int
	userLoginStatus map[string]int
}

const (
	LogoffStatus = 0
	LoginStatus  = 1
)

func NewUserCache() (cache *UserCache) {
	cache = &UserCache{
		make(map[string]LoginBody),
		0,
		make(map[string]int),
	}
	body := LoginBody{}
	body.UserName = "test"
	body.PassWord = "test"
	cache.userCacheMap["test"] = body
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

func (userCache *UserCache) GetUserPassword(userName string) (string, error) {
	body, exist := userCache.userCacheMap[userName]

	if exist {
		return body.PassWord, nil
	}

	return "", errors.New("user does not exist")
}

func (userCache *UserCache) AddUserCache(userId string, LoginCache LoginBody) {
	userCache.userCacheMap[userId] = LoginCache
}

func (userCache *UserCache) DelUserCache(userId string) {
	delete(userCache.userCacheMap, userId)
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
