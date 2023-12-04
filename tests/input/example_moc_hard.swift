// stolen from pepega007xd

let input = "ifj projekt je moc hard"
let mock = mock(input)
write(mock)

func is_even(_ i: Int) -> Int {
    let int = i / 2 * 2 // integer division

    let i_d = Int2Double(i)
    let d = i_d / 2.0 * 2.0
    let double = Double2Int(d)

    if int == double {
        return 1
    } else {
        return 0
    }
}
func mock_char(_ s: String) -> String {
    var c = ord(s)

    if c >= 65 { if c <= 90 { // big -> small
        c = c + 32
        let new = chr(c)
        return new
    } else {} } else {}

    if c >= 97 { if c <= 122 { // small -> big
        c = c - 32
        let new = chr(c)
        return new
    } else {} } else {}

    return s
}

// you know what it is
func mock(_ s: String) -> String {
    let length = length(s)
    var idx = 0
    var output = ""

    while idx < length {
        let end = idx + 1
        let char_opt = substring(of: s, startingAt: idx, endingBefore: end)
        let char = char_opt!

        let is_even = is_even(idx)

        if is_even == 1 {
            let mock_char = mock_char(char)
            output = output + mock_char
        } else {
            output = output + char
        }

        idx = idx + 1
    }

    return output
}
