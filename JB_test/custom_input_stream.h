#ifndef CUSTOM_IS_H
#define CUSTOM_IS_H
#include <fstream>
using namespace std;
class custom_is : public fstream
{
	 static int prev_column_index;
public:
	static int column_index;
	static int row_index;
	static bool erroneous_peek;//used for the purposes of showing the right erroneous JSON indexes
	int get();
static	void adjust_indexes();
	custom_is(const char* s) :fstream(s)
	{

	}
	


};









#endif