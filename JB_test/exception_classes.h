#ifndef EXCEPTION_CLASSES
#define EXCEPTION_CLASSES
using namespace std;
#include <exception>
#include "command_parsing_enumerations.h"
#include <string>
class bad_format :public exception
{
public:

	bad_format(const char* s) : exception(s)
	{

	}
};
class escaped_char :public exception
{
	char c;
public:
	escaped_char(char c)
	{
		this->c = c;
	}
	char get_escaped_character()
	{
		return c;
	}
};
class unreachable_logic :public exception
{
public:
	unreachable_logic() : exception("This part of program shouldn't be executing.")
	{

	}
};
class control_character :public exception
{

};
class redundant_check_succeeded :public exception
{

};

//next group of classes is only used when parsing command string

class semantic_error :public exception
{
public:

	semantic_error(const char* s) : exception(s)
	{

	}
};
class zero_division :public exception
{
};
class bad_operands :public exception
{
	char operation;
	bool  left_only, right_only, both;// +1 if left, +2 if right
public:
	bad_operands(num_op op, char lr);
	char get_op();
	char get_lo();
	char get_ro();
	char get_both();
};
class no_index :public exception
{
public:

	no_index(const char* s) : exception(s)
	{

	}
};
class bad_args :public exception
{
	int args_no;
public:

	bad_args(const char* s, int index) : exception(s)
	{
		args_no = index;
	}
	int get_index()
	{
		return args_no;
	}
};
class field_not_found :public exception
{
	string* s;
public:

	field_not_found(string* s) : exception()
	{
		this->s = s;
	}
	string* getString()
	{
		return s;
	}
};

#endif // !EXCEPTION_CLASSES

