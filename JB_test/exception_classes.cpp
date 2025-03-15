#include "exception_classes.h"
#include "command_parsing_enumerations.h"



bad_operands::bad_operands(num_op op, char lr) :exception(), left_only(false), right_only(false), both(false)
{
	char ops[] = { '+','-','*','/' };
	operation = ops[op];
	if (lr == 3) both = true;
	else if (lr == 2) right_only = true;
	else left_only = true;
}
char bad_operands::get_op()
{
	return operation;
}
char bad_operands::get_lo()
{
	return left_only;
}
char bad_operands::get_ro()
{
	return right_only;
}
char bad_operands::get_both()
{
	return both;
}