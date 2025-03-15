#include "node.h"
#include <vector>
#include <fstream>
#include "custom_input_stream.h"
#include "command_processor.h"



Node * json_parser(custom_is& file_input);
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
	Node *root_node = json_parser(file_input);
	

	Node * temp  = command_processor(root_node, argv[2]).execute_command();
	cout<< *temp;
}