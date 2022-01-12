package MysqlManager

import (
	"database/sql"
	_ "mysql"
	"testserver/LogService"
)

//存储系统数据 人员账号密码，系统注册信息，群组信息等

var dbPool *sql.DB

var MysqlChannel chan string

func StartMysqlService(ip string, port string, user string, password string, databaseName string) {

	connectStr := user + ":" + password + "@tcp(" + ip + ":" + port + ")/" + databaseName

	pool, error := sql.Open("mysql", connectStr)
	if error != nil && pool != nil {
		LogService.LogError("database init error = " + error.Error())
		return
	}
	LogService.LogDebug("Mysql服务启动")
	dbPool = pool
	dbPool.SetMaxOpenConns(10)
	dbPool.SetMaxIdleConns(10)
	databaseInit()
	MysqlChannel = make(chan string, 100) //带缓存的 channel
	go queryInsert()                      //插入采用异步执行，其他同步执行
}

func databaseInit() {

}

// Insert 外部调用函数
func Insert(sql string) {
	MysqlChannel <- sql
}

type UserData struct {
	id            int    `db:"id"`
	UserName      string `db:"UserName"`
	UserLoginName string `db:"UserLoginName"`
	PassWord      string `db:"PassWord"`
	Notice        int    `db:"Notice"`
	MacAddress    string `db:"MacAddress"`
	LoginTime     string `db:"LoginTime"`
}

// Select 查询
func Select(sql string) map[string]UserData {
	rows, _ := dbPool.Query(sql)
	user := make(map[string]UserData)
	for rows.Next() { // 测试写一下

		userData := UserData{}
		err := rows.Scan(&userData.id, &userData.UserName, &userData.UserLoginName, &userData.PassWord, &userData.Notice, &userData.MacAddress, &userData.LoginTime)
		user[userData.UserLoginName] = userData
		checkErr(err)
	}
	rows.Close() //释放连接
	return user
}

func queryInsert() {
	for {
		select {
		case sql := <-MysqlChannel:
			go insert(sql)
		}
	}
}

func insert(sql string) {
	rows, _ := dbPool.Query(sql)
	rows.Close() //释放连接
}

func checkErr(err error) {

	if err != nil {

		LogService.LogError(err.Error())
	}
}
