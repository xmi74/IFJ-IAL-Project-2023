// Nespravny pocet parametrov pri volani funkcie v IF
func subtract(_ a: Int, _ b: Int) -> Int {
    return a - b
}

let i = 5
if (i > 0) {
    let calc : Int 
    calc = subtract(10) // Chyba: Zlý počet parametrov pre metódu triedy
}
else {
    write("Ukoncenie")
}
