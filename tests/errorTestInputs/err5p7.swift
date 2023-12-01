// pouzitie nedefinovanej premennej v ZANORENOM WHILE - IF

var b : Double
var i : Int = 3
var j : Int = 5
while (i > 0)
{
    while (j > 0) {
        if (b > 3) {
            i = i + 1
        }
        else
        {
            i = i - 1
            j = j - 1
        }
    }
}