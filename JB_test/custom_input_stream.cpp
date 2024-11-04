#include "custom_input_stream.h"

int custom_is::get()
{
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
int custom_is::column_index = 1;
int custom_is::prev_column_index = 1;
int custom_is::row_index = 1;
bool custom_is::erroneous_peek = false;
void custom_is::adjust_indexes()
{
	if (!erroneous_peek)
	{
		if (column_index == 1)
		{
			row_index--;
			column_index = prev_column_index;
		}
		else column_index--;
	}
}