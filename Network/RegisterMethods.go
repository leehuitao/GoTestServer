package Network

var MethodsMap map[int]func(pack *Pack)

func Init() {
	MethodsMap = make(map[int]func(pack *Pack))
	Registe()
}

func test(pack *Pack) {

}

func Registe() {
	MethodsMap[1] = test
}

func MethodPerform(pack *Pack) {

}
