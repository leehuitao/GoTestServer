package PackManager

const (
	// OrderLogin 登录
	OrderLogin = 100
	// OrderStartRcvData 客户机开始接收数据
	OrderStartRcvData = 200
	// OrderRcvData	服务器接收机器数据(mes通讯)
	OrderRcvData = 201
	// OrderStopRcvData 客户机停止接收数据
	OrderStopRcvData = 202
	// OrderSendData 服务器发送数据到客户机
	OrderSendData = 203
)

type Request struct {
	Order int    `json:"order"`
	Uuid  string `json:"uuid"`
	Key   string `json:"key"`
}
type Message struct {
	Request   Request `json:"header"`
	Body      []byte
	Recipient string
}
