#ifndef CUSTOM_STRING_STREAM_H
#define CUSTOM_STRING_STREAM_H
#include <sstream>
using namespace std;
class custom_istr_stream :public istringstream
{
public:
	static int column_index, row_index;// not used in the final version
	static bool erroneus_peek;
	int get();
	custom_istr_stream(string s);
};





#endif // !CUSTOM_STRING_STREAM_H

