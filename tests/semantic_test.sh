#!/bin/bash

# TESTY PRE SEMANTICKU ANALYZU
# pouzitie : ./semantic_test.sh

orange_color=$(tput setaf 166)  # Oranžová
purple_color=$(tput setaf 5)   # Fialová
yellow_color=$(tput setaf 3)
green_color=$(tput setaf 2)
red_color=$(tput setaf 1)
reset_color=$(tput sgr0)



# Funkcia pre vykonanie testu
run_test() {
    input=$1
    expected_exit_code=$2

    exec_file="../parser"

    # Inkrementujeme test_number
    ((test_number++))
    ((total_tests++))

    echo -e "${yellow_color} ---------------------------------------------- ${reset_color}"

    $exec_file < ./$input 1>/dev/null    

    # navratovy kod
    actual_exit_code=$?

    # Formát výsledku
    if [ $actual_exit_code -eq $expected_exit_code ]; then
        echo -e "${green_color}Test $test_number Passed:${reset_color} -> ${input} ${orange_color}error code: ${actual_exit_code}${reset_color}"
    else
        echo -e "${red_color}Test $test_number Failed:${reset_color}"
        echo "  - Input: [ $input ]"
        echo "  - Expected Exit Code: $expected_exit_code"
        echo "  - Actual Exit Code: $actual_exit_code"
    fi
}

echo -e "\n${purple_color}============================================================${reset_color}"
echo -e "\n\t${purple_color}TESTOVANIE ERROR 3${reset_color}"
run_test errorTestInputs/err3p1.swift 3
run_test errorTestInputs/err3p2.swift 3
run_test errorTestInputs/err3p3.swift 3
run_test errorTestInputs/err3p4.swift 3
run_test errorTestInputs/err3p5.swift 3
run_test errorTestInputs/err3p6.swift 3
run_test errorTestInputs/err3p7.swift 3

echo -e "\n${purple_color}============================================================${reset_color}"
echo -e "\n\t${purple_color}TESTOVANIE ERROR 4${reset_color}"
run_test errorTestInputs/err4p1.swift 4
run_test errorTestInputs/err4p2.swift 4
run_test errorTestInputs/err4p3.swift 4
run_test errorTestInputs/err4p4.swift 4
run_test errorTestInputs/err4p5.swift 4
run_test errorTestInputs/err4p6.swift 4
run_test errorTestInputs/err4p7.swift 4

echo -e "\n${purple_color}============================================================${reset_color}"
echo -e "\n\t${purple_color}TESTOVANIE ERROR 5${reset_color}"
run_test errorTestInputs/err5p1.swift 5
run_test errorTestInputs/err5p2.swift 5
run_test errorTestInputs/err5p3.swift 5
run_test errorTestInputs/err5p4.swift 5
run_test errorTestInputs/err5p5.swift 5
run_test errorTestInputs/err5p6.swift 5
run_test errorTestInputs/err5p7.swift 5

echo -e "\n${purple_color}============================================================${reset_color}"
echo -e "\n\t${purple_color}TESTOVANIE ERROR 6${reset_color}"
run_test errorTestInputs/err6p1.swift 6
run_test errorTestInputs/err6p2.swift 6
run_test errorTestInputs/err6p3.swift 6
run_test errorTestInputs/err6p4.swift 6
run_test errorTestInputs/err6p5.swift 6
run_test errorTestInputs/err6p6.swift 6
run_test errorTestInputs/err6p7.swift 6

echo -e "\n${purple_color}============================================================${reset_color}"
echo -e "\n\t${purple_color}TESTOVANIE ERROR 7${reset_color}"
run_test errorTestInputs/err7p1.swift 7
run_test errorTestInputs/err7p2.swift 7
run_test errorTestInputs/err7p3.swift 7
run_test errorTestInputs/err7p4.swift 7
run_test errorTestInputs/err7p5.swift 7
run_test errorTestInputs/err7p6.swift 7
run_test errorTestInputs/err7p7.swift 7

echo -e "\n${purple_color}============================================================${reset_color}"
echo -e "\n\t${purple_color}TESTOVANIE ERROR 8${reset_color}"
run_test errorTestInputs/err8p1.swift 8
run_test errorTestInputs/err8p2.swift 8
run_test errorTestInputs/err8p3.swift 8
run_test errorTestInputs/err8p4.swift 8
run_test errorTestInputs/err8p5.swift 8
run_test errorTestInputs/err8p6.swift 8
run_test errorTestInputs/err8p7.swift 8