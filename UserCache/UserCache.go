package UserCache

import (
	"encoding/json"
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

type OrgCacheItem struct {
	DeptName     string `json:"DeptName"`
	DeptID       int    `json:"DeptID"`
	ParentDeptID int    `json:"ParentDeptID"`
	Level        int    `json:"Level"`
}
type OrgCache struct {
	dept     []OrgCacheItem
	deptJson map[int]interface{}
}

const (
	LogoffStatus = 0
	LoginStatus  = 1
)

var OrgCacheData OrgCache

func (org *OrgCache) addCache(data OrgCacheItem) {
	org.dept = append(org.dept, data)
	b, _ := json.Marshal(data)
	org.deptJson[data.DeptID] = b
}

func (org *OrgCache) GetJson() []byte {
	marshal, err := json.Marshal(org.deptJson)
	if err != nil {
		return nil
	}
	return marshal
}

func NewOrgCache() (cache OrgCache) {
	orgCache := OrgCache{}

	orgCache.deptJson = make(map[int]interface{})

	return orgCache
}

func NewUserCache() (cache *UserCache) {
	cache = &UserCache{
		make(map[string]PackManager.LoginBody),
		0,
		make(map[string]int),
		make(map[string]string),
	}
	mysqlInit(cache)
	orgInit()
	return cache
}

func mysqlInit(cache *UserCache) {
	m := MysqlManager.SelectUserCache("select * from userinfo")
	for k, v := range m {
		body := PackManager.LoginBody{}
		body.UserName = v.UserName
		body.PassWord = v.PassWord
		body.MacAddress = v.MacAddress
		body.UserLoginName = v.UserLoginName
		cache.userCacheMap[k] = body
	}
}

func orgInit() {
	OrgCacheData = NewOrgCache()
	org := MysqlManager.SelectOrg("select * from dept_info")
	for _, v := range org {
		item := OrgCacheItem{}
		item.DeptName = v.DeptName
		item.DeptID = v.DeptID
		item.ParentDeptID = v.ParentDeptID
		item.Level = v.Level
		OrgCacheData.addCache(item)
	}
}
func (userCache *UserCache) GetJson() []byte {
	cache := make(map[string]interface{})
	for k, v := range userCache.userCacheMap {
		b, _ := json.Marshal(v)
		cache[k] = b
	}
	marshal, err := json.Marshal(cache)
	if err != nil {
		return nil
	}
	return marshal
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
