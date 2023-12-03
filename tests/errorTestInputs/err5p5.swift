// pouzitie nedefinovanej premennej v definicii funkcie

var b : Double

func function1(_ a : Double) -> Double {
    a = a + b
    return a
}