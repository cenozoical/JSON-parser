#ifndef CUSTOM_STRING_STREAM_H
#define CUSTOM_STRING_STREAM_H
#include <sstream>
using namespace std;
class custom_istr_stream :public istringstream
{
	int column_index;// not used in the final version
	bool last_operation_was_peek;
public:
	int peek();
	int get();
	int get_column_index();
	void set_column_index(int new_value);
	void subtract_column_index(int subtrahend);
	custom_istr_stream(string s);
};





#endif // !CUSTOM_STRING_STREAM_H

