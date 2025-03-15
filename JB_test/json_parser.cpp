#include "node.h"
#include <vector>
#include <fstream>
#include "exception_classes.h"
/*
json_parser function could be thought of as finite-state machine.
It attempts to build a tree-like structure, that represents the JSON file specified.
The way it does so is by depth-first patter. The 'stack' variable is a consequence of that approach.
*/
Node * json_parser(custom_is& file_input)
{

	vector<Node*> stack;
	Node* temp_node;
	int parser_state;
	int c, skip;

	try
	{
		temp_node = new Node();//node which represents the JSON file specified
		temp_node->set_type(JSON);
		stack.push_back(temp_node);
		parser_state = 0;//the initial state
		while ((skip = file_input.peek()) == ' ' || skip == '\n' || skip == '\r' || skip == '\t')file_input.get();
		if ((c = file_input.get()) == EOF) return temp_node;
		else if (c != '{')throw bad_format("JSON file must be empty or start with '{'");


		/*
		In parser state 0, parser expects JSON field name. Since JSON element could be empty '}' character is accepted as well(or rather forwared to parser state 3).
		In parser state 1, parser expects ':' character.
		In parser state 2, parser expects a JSON field value or an array element. Since an array could be empty, ']' character is accepted as well(or rather forwared to parser state 3).
		In parser state 3, parser expects a character from the set {',' , '}' , ']'}.
		*/
		while (true)
		{
			while ((skip = file_input.peek()) == ' ' || skip == '\n' || skip == '\r' || skip == '\t') file_input.get();
			if (parser_state == 0)
			{
				if (file_input.peek() == '}')
				{
					parser_state = 3;
					continue;
				}
				temp_node = new Node();
				file_input >> *temp_node;
				if (temp_node->get_type() != STRING) throw bad_format("Field name must be enclosed inside quotation marks."); 
				temp_node->set_name(*(temp_node->get_string_value()));
				parser_state = 1;
			}
			else if (parser_state == 1)
			{
				if (file_input.get() != ':') throw bad_format("Character ':' must follow field name.");
				parser_state = 2;
			}
			else if (parser_state == 2)
			{
				if (file_input.peek() == ']')
				{
					parser_state = 3;
					continue;
				}
				file_input >> *temp_node;
				node_type temp_type = temp_node->get_type();
				if (temp_type == JSON)
				{
					stack.push_back(temp_node);
					parser_state = 0;
				}
				else if (temp_type == ARRAY)
				{
					stack.push_back(temp_node);
					temp_node = new Node();
					parser_state = 2;

				}
				else
				{
					Node* parent = stack.back();
					parent->add_child(temp_node);
					parser_state = 3;
				}
			}
			else if (parser_state == 3)
			{
				c = file_input.get();
				Node* parent = stack.back();
				if (c == ',')
				{
					if (parent->get_type() == JSON) parser_state = 0;
					else if (parent->get_type() == ARRAY)
					{
						temp_node = new Node();
						parser_state = 2;
					}
					else throw unreachable_logic();//code shouldn't ever execute this line, but you never know
				}
				else if (c == '}')
				{
					if (parent->get_type() != JSON) throw bad_format("Unrecognized pattern.");
					if (file_input.peek() == EOF)
					{
						if (stack.size() != 1) throw bad_format("Unrecognized pattern.");
						break;
					}
					temp_node = parent;
					stack.pop_back();
					parent = stack.back();
					parent->add_child(temp_node);
					parser_state = 3;
				}
				else if (c == ']')
				{
					if (parent->get_type() != ARRAY) throw bad_format("Unrecognized pattern.");
					temp_node = parent;
					stack.pop_back();
					parent = stack.back();
					parent->add_child(temp_node);
					parser_state = 3;
				}
			}



		}
	}
	catch (bad_format& e)
	{
		
		cout <<"at column: "<< file_input.get_column_index()<<", row: " << file_input.get_row_index() <<" - " << "JSON parsing error: " << e.what();
		exit(9);
	}
	catch (escaped_char& e)
	{
		
		cout << "at column: " << file_input.get_column_index() << ", row: " << file_input.get_row_index() << " - " << "JSON parsing error: " <<"cannot escape character " << e.get_escaped_character();
		exit(9);
	}
	catch (unreachable_logic& e)
	{;
		cout << "at column: " << file_input.get_column_index() << ", row: " << file_input.get_row_index() << " - " << "JSON parsing error: "<<e.what();
		exit(9);
	}
	catch (invalid_argument& )
	{
		
		cout << "at column: " << file_input.get_column_index() << ", row: " << file_input.get_row_index() << " - " << "JSON parsing error: value of string type field must be specified between two single quotation marks";
		exit(9);
	}
	catch (out_of_range& )
	{
		cout << "at column: " << file_input.get_column_index() << ", row: " << file_input.get_row_index() << " - " << "JSON parsing error: number field value out of range";
		exit(9);
	}
	catch (control_character& )
	{
		cout << "at column: " << file_input.get_column_index() << ", row: " << file_input.get_row_index() << " - " << "JSON parsing error: control character can't be part of a string value.";
		exit(9);
	}

	
	temp_node = stack.back();
	return temp_node;








}