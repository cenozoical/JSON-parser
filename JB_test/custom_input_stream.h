#ifndef CUSTOM_IS_H
#define CUSTOM_IS_H
#include <fstream>
using namespace std;
/*
* This class is used for reading characters from JSON file.
* It builds upon fstream class by providing additional information about the current position in JSON file,
* which is used for precise error reporting.
*/
class custom_is : public fstream
{
	  int prev_column_index;
	  int column_index;
	  int row_index;

	  bool last_operation_is_peek;
public:
	void subtract_column_index(int subtrahend);
	int get_column_index();
	int get_row_index();
	int peek();

	int get();
	
	custom_is(const char* s);
	


};









#endif