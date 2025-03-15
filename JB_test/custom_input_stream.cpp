#include "custom_input_stream.h"

custom_is::custom_is(const char* s) :fstream(s)
{
	column_index = 1;
	prev_column_index = 1;
	row_index = 1;
	last_operation_is_peek = false;
}
int custom_is::get()
{
	last_operation_is_peek = false;
	int c = this->istream::get();
	if (c == '\n')
	{
		this->prev_column_index = custom_is::column_index;
		custom_is::column_index = 1;
		custom_is::row_index++;

	}
	else column_index++;
	return c;
}
int custom_is::peek()
{
	last_operation_is_peek = true;
	return this->istream::peek();
	
}

int custom_is::get_column_index()
{
	if (last_operation_is_peek) return column_index;
	else if (column_index == 1) return prev_column_index;
	else return column_index - 1;
}
int custom_is::get_row_index()
{
	if (last_operation_is_peek) return row_index;
	else if (column_index == 1) return row_index - 1;
	else return row_index;
}
void custom_is::subtract_column_index(int subtrahend)
{
	column_index -= subtrahend;
}
//void custom_is::adjust_indexes()
//{
//	if (!erroneous_peek)
//	{
//		if (column_index == 1)
//		{
//			row_index--;
//			column_index = prev_column_index;
//		}
//		else column_index--;
//	}
//}