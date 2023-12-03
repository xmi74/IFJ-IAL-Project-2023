// Nespravny pocet parametrov pri volani funkcie vo WHILE
func subtract(_ a: Int, _ b: Int) -> Int {
    return a - b
}

let i = 5
while (i > 0) {
    var calc : Int 
    calc = subtract(10) // Chyba: Zlý počet parametrov
}
