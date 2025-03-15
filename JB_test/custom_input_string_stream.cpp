#include "custom_input_string_stream.h"
#include <iostream>
#include "node.h"
#include "exception_classes.h"

custom_istr_stream::custom_istr_stream(string s) :istringstream(s)
{
	column_index = 1;
	last_operation_was_peek = true;
}
int custom_istr_stream::get()
{
	last_operation_was_peek = false;
	int c = this->istringstream::get();
	
	if (c == '\n')throw bad_format("Command strings cannot have line breaks.");
		
	column_index++;
	return c;

}
int custom_istr_stream::peek()
{
	last_operation_was_peek = true;
	return this->istringstream::peek();
}
int custom_istr_stream::get_column_index()
{
	if (last_operation_was_peek) return column_index;
	else return column_index - 1;
}
void custom_istr_stream::subtract_column_index(int subtrahend)
{
	column_index -= subtrahend;
}
void custom_istr_stream::set_column_index(int new_value)
{
	column_index = new_value;
}


