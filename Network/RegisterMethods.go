package Network

var MethodsMap map[int]func(pack *Pack)

func init()  {
	MethodsMap = make(map[int]func(pack *Pack))
}

func test(pack *Pack)  {

}

func Registe()  {
	MethodsMap[1] = test
}

func MethodPerform(pack *Pack)  {
	
}
