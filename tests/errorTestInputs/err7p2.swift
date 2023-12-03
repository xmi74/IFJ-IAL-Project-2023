// zla typova kompatibilita aritmetickych vyrazov vo vnorenom WHILE

var x : String = "STRING"
var y : Double = 3.14
var i = 5
var j = 5

while i > 0
{
    while (j > 0)
    {
        var z = x + y
        j = j - 1
    }
    i = i - 1
}