package PackManager

type UserCache struct {
	userCacheMap map[string]LoginBody
	loginNumber  int
}

func NewUserCache() (cache *UserCache) {
	cache = &UserCache{
		make(map[string]LoginBody),
		0,
	}
	return cache
}

func (userCache *UserCache) AddUserCache(userId string, LoginCache LoginBody) {
	userCache.userCacheMap[userId] = LoginCache
}

func (userCache *UserCache) DelUserCache(userId string) {
	delete(userCache.userCacheMap, userId)
}
