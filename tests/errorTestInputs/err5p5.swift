// pouzitie nedefinovanej premennej v definicii funkcie

var b : Double

func function1(_ a : Double) -> Double {
    let a = b
    return a
}
