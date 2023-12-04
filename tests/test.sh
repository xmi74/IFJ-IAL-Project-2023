#!/bin/bash

testNum=1
compilerPath="../parser"
passed=0
failed=0
wrc=0

# arguments:
# 1. name of test
# 2. input file
# 3. expected output file
# 4. expected return code
#!/bin/bash

# Initialize test number variable
testNum=1

# Define the execTest function
execTest () {
	echo -e "\e[33m--------------------------------\e[0m"
	touch stderr.txt
	output=$(bash -c "$compilerPath < $2" 2>"stderr.txt")
	returnCode=$?
	echo "$output" > tmp_output.txt
	touch tmp_output2.txt
	if [ "$returnCode" = "0" ]; then
		./ic23int tmp_output.txt > tmp_output2.txt
	fi

	printf "\n" >> tmp_output2.txt

	if [ "$returnCode" -ne "$4" ]; then
		printf "\e[1m\e[31mFailed\e[0m Test %02d: %s:\n" "$testNum" "$1"
		printf "\tWrong return code, expected %s, got %s\n" "$4" "$returnCode"
		printf "\tStderr output: %s\n" "$(cat "stderr.txt" | tr -d '\n')"
		wrc=$((wrc+1))
	elif [ -z "$(diff --ignore-trailing-space --ignore-blank-lines tmp_output2.txt "$3")" ]; then
		printf "\e[1m\e[32mPassed\e[0m Test %02d: %s\n" "$testNum" "$1"
		passed=$((passed+1))
	else
		printf "\e[1m\e[31mFailed\e[0m Test %02d: %s\n" "$testNum" "$1"
		failed=$((failed+1))
		diff --ignore-trailing-space --ignore-blank-lines tmp_output2.txt "$3"
	fi
	testNum=$((testNum+1))
	rm -f tmp_output.txt tmp_output2.txt
}

execTest "Empty program" "input/empty.swift" "output/empty.txt" 0
execTest "Unfinished multiline comment" "input/multiline_comment_unfinished.swift" "output/empty.txt" 1
execTest "Legal variable names" "input/variable_name.swift" "output/empty.txt" 0
execTest "Variable names starting with numbers" "input/variable_name_number.swift" "output/empty.txt" 1
execTest "Variable name as single underscore" "input/variable_name_underscore.swift" "output/empty.txt" 2
execTest "Variable name as keyword" "input/variable_name_keyword.swift" "output/empty.txt" 2
execTest "Legal nil initialization" "input/nil_init.swift" "output/empty.txt" 0
execTest "Decimal literals" "input/decimal_literals.swift" "output/empty.txt" 0
execTest "Decimal literals with empty whole part" "input/decimal_literal_empty_whole_part.swift" "output/empty.txt" 1
execTest "Decimal literals with empty decimal part" "input/decimal_literal_empty_decimal_part.swift" "output/empty.txt" 1
execTest "Decimal literals with empty exponent" "input/decimal_literal_empty_exponent.swift" "output/empty.txt" 1
execTest "Decimal literals with decimal exponent" "input/decimal_literal_decimal_exponent.swift" "output/empty.txt" 1
execTest "Decimal literals with other chars" "input/decimal_literal_other_char.swift" "output/empty.txt" 1
execTest "Illegal nil initialization" "input/nil_init_illegal.swift" "output/empty.txt" 7
execTest "Type deduction" "input/type_deduction.swift" "output/empty.txt" 0
execTest "Nil type deduction" "input/nil_type_deduction.swift" "output/empty.txt" 8
execTest "Comments" "input/comments.swift" "output/empty.txt" 0
execTest "Variable redefinition" "input/variable_redefinition.swift" "output/empty.txt" 3
execTest "Variable shadowing" "input/variable_shadowing.swift" "output/variable_shadowing.txt" 0
execTest "Variable out of scope" "input/variable_out_of_scope.swift" "output/empty.txt" 5
execTest "Undefined variable" "input/undefined_variable.swift" "output/empty.txt" 5
execTest "Default nil initialisation" "input/default_nil_init.swift" "output/default_nil_init.txt" 0
execTest "Uninitialised variable" "input/uninitialised_variable.swift" "output/empty.txt" 5
execTest "Uninitialised variable (init in deeper scope)" "input/uninitialised_variable_in_scope.swift" "output/empty.txt" 5
execTest "Variable initialised in scope" "input/initialised_variable_in_scope.swift" "output/empty.txt" 0
execTest "Function call" "input/function_call.swift" "output/function_call.txt" 0
execTest "Function call before definition" "input/function_call_before_def.swift" "output/function_call.txt" 0
execTest "Undefined function" "input/undefined_function.swift" "output/empty.txt" 3
execTest "Function redefinition" "input/function_redefinition.swift" "output/empty.txt" 3
execTest "Function call with arguments" "input/function_call_args.swift" "output/function_call_args.txt" 0
execTest "Function with wrong parameter id" "input/funct_param_wrong_id.swift" "output/empty.txt" 5
execTest "Function call with wrong parameter name" "input/func_param_wrong_name.swift" "output/empty.txt" 4
execTest "Function call with omitted name" "input/func_param_omit_name.swift" "output/function_call_args.txt" 0
execTest "Function call with illegaly omitted name" "input/func_param_omit_name_illegal.swift" "output/empty.txt" 9
execTest "Function call with wrong parameter type" "input/func_call_wrong_type.swift" "output/empty.txt" 4
execTest "Function call with wrong number of params" "input/func_call_wrong_param_count.swift" "output/empty.txt" 4
execTest "Modify function parameter" "input/func_call_modify_param.swift" "output/empty.txt" 9
execTest "Eearly return from procedure" "input/proc_return.swift" "output/empty.txt" 0
execTest "Try to return value from procedure" "input/proc_return_with_type.swift" "output/empty.txt" 2
execTest "Try to return without value from function" "input/func_return_without_value.swift" "output/empty.txt" 2
execTest "Return wrong type from function" "input/func_wrong_return_type.swift" "output/empty.txt" 4
execTest "Return statement in global scope" "input/return_in_global_scope.swift" "output/empty.txt" 2
execTest "Return statement in global scope" "input/return_in_global_scope_with_value.swift" "output/empty.txt" 2
execTest "Modify constant" "input/modify_const_variable.swift" "output/empty.txt" 9
execTest "Set constant after definition" "input/immutable_deferred_init.swift" "output/immutable_deferred_init.txt" 0
execTest "Incomplete variable declaration" "input/incomplete_variabe_decl.swift" "output/empty.txt" 2
execTest "Wrong variable init type" "input/wrong_init_type.swift" "output/empty.txt" 7
execTest "Omit variable value in deifinition" "input/variable_omit_value.swift" "output/empty.txt" 0
execTest "Assignment with wrong type" "input/assignment_wrong_type.swift" "output/empty.txt" 7
execTest "Assignment" "input/assignment.swift" "output/assignment.txt" 0
execTest "Assign to const variable" "input/assign_const.swift" "output/empty.txt" 9
execTest "If statement" "input/if_statement.swift" "output/if_statement.txt" 0
execTest "If statement with wrong condition type" "input/if_statement_no_bool.swift" "output/empty.txt" 7
execTest "If let statement" "input/if_let.swift" "output/if_let.txt" 0
execTest "If let statement with non-null variable" "input/if_let_nonull.swift" "output/empty.txt" 9
execTest "If let statement modify attempt" "input/if_let_modify.swift" "output/empty.txt" 9
execTest "While loop" "input/while.swift" "output/while.txt" 0
execTest "While loop with wrong condition type" "input/while_no_bool.swift" "output/empty.txt" 7
execTest "Discard function return value" "input/func_call_discard_return.swift" "output/empty.txt" 0
execTest "Call function, assign to wrong variable type" "input/func_call_wrong_assign.swift" "output/empty.txt" 7
execTest "Call procedure, try to assign" "input/proc_call_assign.swift" "output/empty.txt" 7
execTest "Call procedure, try to init" "input/proc_call_init.swift" "output/empty.txt" 7
execTest "Arithmetic" "input/arithmetic.swift" "output/arithmetic.txt" 0
execTest "Implicit conversion of literal" "input/implicit_convert_literal.swift" "output/implicit_convert_literal.txt" 0
execTest "Implicit conversion of non-literal" "input/implicit_convert_non_literal.swift" "output/empty.txt" 7
execTest "Wrong binary types" "input/binary_wrong_types.swift" "output/empty.txt" 7
execTest "Non null lhs in coalescence" "input/coal_lhs_non_null.swift" "output/empty.txt" 7
execTest "Non-matching types in nil coalescence" "input/coal_wrong_types.swift" "output/empty.txt" 7
execTest "Force unwrapping of non-nullable variable" "input/unwrap_non_nullable.swift" "output/empty.txt" 7
execTest "Comparison of different types" "input/rel_wrong_type.swift" "output/empty.txt" 7
execTest "Comparison of convertible types" "input/rel_implicit_convert.swift" "output/empty.txt" 0
execTest "Comparison of non-nil and nil types" "input/rel_wrong_type_nil.swift" "output/empty.txt" 0
execTest "Comparison of strings" "input/rel_string.swift" "output/rel_string.txt" 0
execTest "Relational operators" "input/rel.swift" "output/rel.txt" 0
execTest "Relational operator with illegal implicit conversion" "input/rel_wrong_convert.swift" "output/empty.txt" 7
execTest "Operator precedence" "input/precedence.swift" "output/precedence.txt" 0
execTest "Builtin conversion functions" "input/builtin_convert.swift" "output/builtin_convert.txt" 0
execTest "Int2Double with wrong type" "input/int2double_wrong.swift" "output/empty.txt" 4
execTest "Double2Int with wrong type" "input/double2int_wrong.swift" "output/empty.txt" 4
execTest "Builtin Length function" "input/builtin_length.swift" "output/builtin_length.txt" 0
execTest "Builtin Length function with wrong type" "input/builtin_length_wrong.swift" "output/empty.txt" 4
execTest "Substring" "input/substring.swift" "output/substring.txt" 0
execTest "Builtin ord function" "input/builtin_ord.swift" "output/builtin_ord.txt" 0
execTest "Builtin chr function" "input/builtin_chr.swift" "output/builtin_chr.txt" 0
execTest "Init variable in while loop" "input/while_init.swift" "output/empty.txt" 0
execTest "Init variable in while loop - nested" "input/while_init_nested.swift" "output/while_init_nested.txt" 0
execTest "Illegal token instead of data type" "input/illegal_typename.swift" "output/empty.txt" 2
execTest "String with escape sequences" "input/string_valid.swift" "output/string_valid.txt" 0
execTest "String with invalid escape sequence" "input/string_invalid_escape.swift" "output/empty.txt" 1
execTest "String with ascii escape sequence" "input/string_ascii.swift" "output/string_ascii.txt" 0
execTest "String with invalid ascii" "input/string_ascii_invalid.swift" "output/empty.txt" 1
execTest "String literal with newline character" "input/string_literal_newline.swift" "output/empty.txt" 1
execTest "Init Double variable with int literal" "input/double_int_init.swift" "output/empty.txt" 0
execTest "Basic multiline string" "input/multiline_string.swift" "output/multiline_string.txt" 0
execTest "Multiline string with indent" "input/multiline_string_indent.swift" "output/multiline_string_indent.txt" 0
execTest "Empty multiline string" "input/multiline_string_empty.swift" "output/multiline_string_empty.txt" 0
execTest "Multiline string with empty line" "input/multiline_string_empty_line.swift" "output/multiline_string_empty_line.txt" 0
execTest "Multiline string with indent and empty line" "input/multiline_string_indent_empty_line.swift" "output/multiline_string_indent_empty_line.txt" 0
execTest "Multiline string with bad indent" "input/multiline_string_indent_illegal.swift" "output/empty.txt" 1
execTest "Multiline string with closing quotes not on unique line" "input/multiline_string_wrong_close.swift" "output/empty.txt" 1
execTest "Multiline string with quotation marks" "input/multiline_string_quotation_mark.swift" "output/multiline_string_quotation_mark.txt" 0
execTest "Equality comparison with nil literal" "input/nil_eq.swift" "output/nil_eq.txt" 0
execTest "Example - concat" "input/example_concat.swift" "output/example_concat.txt" 0
execTest "Example - factorial (iterative)" "input/example_factorial_iter.swift" "output/example_factorial_iter.txt" 0
execTest "Example - factorial (recursive)" "input/example_factorial_rec.swift" "output/example_factorial_rec.txt" 0
execTest "Create a variable inside while loop" "input/init_inside_while.swift" "output/init_inside_while.txt" 0
execTest "Shadowing a variable inside if-let statement" "input/if_let_shadowing.swift" "output/if_let_shadowing.txt" 0
execTest "Implicit conversions in variable declarations" "input/var_init_implicit_convert.swift" "output/empty.txt" 0
execTest "Illegal implicit conversions in variable declarations" "input/var_init_implicit_convert_bad.swift" "output/empty.txt" 7
execTest "Implicit conversions in variable assignment" "input/assignment_implicit_convert.swift" "output/empty.txt" 0
execTest "Illegal implicit conversions in variable assignment" "input/assignment_implicit_convert_wrong.swift" "output/empty.txt" 7
execTest "Escaping characters in strings" "input/escaped_strings.swift" "output/escaped_strings.txt" 0
execTest "Example - redefinition inside while" "input/example_while_redef.swift" "output/example_while_redef.txt" 0
execTest "Coalescence with nil on lhs" "input/coal_nil.swift" "output/coal_nil.txt" 0
execTest "Example - ifj projekt je moc hard" "input/example_moc_hard.swift" "output/example_moc_hard.txt" 0
execTest "Example - cycles" "input/example_cycles.swift" "output/empty.txt" 0
execTest "Bogus input (func !)" "input/bogus_func.swift" "output/empty.txt" 2
execTest "Escape sequence + ord" "input/escape_ord.swift" "output/escape_ord.txt" 0
execTest "Ord function with empty string" "input/ord_empty.swift" "output/empty.txt" 0
execTest "Substring end out of bounds" "input/substring_oob.swift" "output/empty.txt" 0
execTest "Implicit cast in nil coalescence" "input/implicit_cast_coal.swift" "output/empty.txt" 0
execTest "Wrong argument name" "input/func_wrong_arg_name.swift" "output/empty.txt" 9
execTest "Unused param" "input/unused_param.swift" "output/empty.txt" 0
execTest "Function param with same name" "input/func_param_same_name.swift" "output/empty.txt" 9
execTest "Implicit conversion when assigning" "input/assign_convert.swift" "output/assign_convert.txt" 0

echo
printf "\e[1m\e[32mPassed tests:\e[0m %02d %s\n" "$passed" "$1"
printf "\e[1m\e[31mFailed tests:\e[0m %02d %s\n" "$failed" "$1"
printf "\e[1m\e[31mWrong return codes:\e[0m %02d %s\n" "$wrc" "$1"