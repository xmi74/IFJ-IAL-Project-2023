#!/bin/bash

# V maine treba zavolat checkexpression() !

# Kontrola argumentov skriptu
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 input_file output_file"
    exit 1
fi

input_file=$1
output_file=$2

# Kontrola ci existuje vstupny subor
if [ ! -f "$input_file" ]; then
    echo "Error: Input file '$input_file' not found."
    exit 1
fi

# Nastavenie output suboru
> "$output_file"

# Iterovanie cez vsetky riadky vstupneho suboru
while IFS= read -r expression; do
    # Spustenie programu s danym vyrazom
    result=$(./project <<< "$expression")
    echo "Expression: $expression" >> "$output_file"
    echo "Result: $result" >> "$output_file"
    echo "------------------------" >> "$output_file"
done < "$input_file"

echo "Results logged to '$output_file'"
