// Porovnavanie zlych typov vo vnorenom if

var i = 5
var retazec : String = "retazec"

if (i  > 0) {
    write ("Porovnavanie zlych typov")
    if (i == retazec) {
        write("Tu nastala chyba")
    }
    else {
        write("Priklad na chybu 7")
    }
}
else {
    write("Ukoncenie")
}