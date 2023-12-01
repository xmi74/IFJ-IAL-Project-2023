// redefinovana funkcia
var x = 1

func multiply(_ a: Int, _ b: Int) -> Int {
    return a * b
}

func multiply(_ a: Double, _ b: Double) -> Double {
    return a * b // Chyba: Redefinovaná funkcia s iným návratovým typom
}

let result = multiply(3.0, 4.0)

