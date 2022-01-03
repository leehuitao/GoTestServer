package PackManager

import "errors"

type UserCache struct {
	userCacheMap map[string]LoginBody
	loginNumber  int
}

func NewUserCache() (cache *UserCache) {
	cache = &UserCache{
		make(map[string]LoginBody),
		0,
	}
	body := LoginBody{}
	body.UserName = "test"
	body.PassWord = "test"
	cache.userCacheMap["test"] = body
	return cache

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
