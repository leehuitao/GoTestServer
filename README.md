# GoTestServer

#### 介绍
golang测试并发服务器

#### 软件架构
软件架构说明

#### 开发时间

1.  2021.12.28  立项
2.  2021.12.29  做了服务器监听，完成了简单的协议处理以及tcp黏包处理，做了一些方法映射，以及客户端管理
3.  2021.12.30	添加普通消息的json转换
4.  2022.1.1    新增写文件的channel协程，封装了协程缓存，增加日志系统存储文件
5.  2022.1.2    新增mysql 连接池，redis MongoDB 暂时没写存个架构方便未来分布式，添加测试使用的QT客户端


#### 使用说明

1.    需要安装模块
    1.go  get  -u github.com/go-sql-driver/mysql，如果无法使用可以直接进入gopath 下的src目录  clone下来


#### 参与贡献

1.  牛顶顶

#### 特技

