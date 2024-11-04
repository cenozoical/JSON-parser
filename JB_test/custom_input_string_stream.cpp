#include "custom_input_string_stream.h"
#include <iostream>
#include "node.h"

custom_istr_stream::custom_istr_stream(string s) :istringstream(s)
{

}
int custom_istr_stream::get()
{
	
	int c = this->istringstream::get();
	
	if (c == '\n')throw bad_format("Command strings cannot have line breaks.");
		
	 column_index++;
	return c;

}
int custom_istr_stream::column_index = 1;
int custom_istr_stream::row_index = 1;
bool custom_istr_stream::erroneus_peek = false;