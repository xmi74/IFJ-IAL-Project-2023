// stolen from pepega007xd

let cycles = 50

func is_inside(_ real: Double, _ imag: Double) -> Int {
    var x = real
    var y = imag

    var cycle = 0

    while cycle < cycles {
        let tmp = x
        x = x * x - y * y + real
        y = 2 * tmp * y + imag

        if x * x + y * y > 4 {
            return 0
        } else {}

        cycle = cycle + 1
    }

    return 1
}

let size : Int? = 10
if let size {
    let size_d = Int2Double(size)
    var y = 0 - size
    while y < size {
        var x = 0 - 2 * size

        while x < size {
            var real = Int2Double(x)
            var imag = Int2Double(y)
            real = real / size_d
            imag = imag / size_d

            let is_inside = is_inside(real, imag)

            if is_inside == 1 { write() }
            else { write() }

            x = x + 1
        }

        write()
        y = y + 1
    }
} else {
    write("Cannot parse input")
}

