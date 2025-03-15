#ifndef COMMAND_PROCESSOR_H
#define COMMAND_PROCESSOR_H
#include "node.h"
#include <vector>
#include <sstream>
#include "custom_input_string_stream.h"
#include "command_parsing_enumerations.h"
class command_processor
{
	Node* root_node;//root node of a tree structure that represents the target JSON file
	custom_istr_stream ss;
	vector <vector<Node*>*> scopes;//a stack of "scopes", where each scope is opened with brackets or braces in command string
	vector <intrinsic_function_type> instrinsic_scopes;//keeps information as to whether the scopes are opened with an intinsic function or without e.g. max(a,b) pushes MAX
	vector <num_op> op_vect;//stack of binary operators that provides support for operation priorities e.g. in "a + b*c", '*' operation
	//should be performed before +
	vector<int> op_pos_vect;//keeps the information about column index of arithmetic operators for purposes of precise error reporting
	vector<enclosure_type>enc_vector;//keeps information about last enclosure type, so as to prevent braces from closing brackets

	string* read_name();
	Node* execute_command_inner();
	void execute_intrinsic_function();
	void calculate();

public:
	command_processor(Node* root_node,const char* command);
	Node* execute_command();

};
#endif

