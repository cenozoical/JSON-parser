#include "node.h"
#include <vector>
#include <fstream>
#include "custom_input_stream.h"



Node * json_parser(custom_is& file_input);
void execute_command(const char* command);
Node* root_node;
int main(int argc,  char const * argv[])
{
	if (argc != 3)
	{
		cout << "Program expects 2 arguments (json file path and command string, respectively).";
		exit(9);
	}
	custom_is file_input (argv[1]);
	if (file_input.rdstate() && ios_base::failbit)
	{
		cout << "JSON file could not be opened.";
		exit(9);
	}
	root_node = json_parser(file_input);
	

	//cout << *root_node<<'\n';
	execute_command(argv[2]);
}