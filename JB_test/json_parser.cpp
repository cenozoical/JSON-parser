#include "node.h"
#include <vector>
#include <fstream>


Node * json_parser(custom_is& file_input)
{

	vector<Node*> stack;
	Node* temp_node;
	int parser_state = 0;
	int c, skip;

	char w[34];
	try
	{
		temp_node = new Node();
		temp_node->set_type(JSON);
		stack.push_back(temp_node);
		parser_state = 1;
		skip = file_input.peek();
		while (skip == ' ' || skip == '\n' || skip == '\r' || skip == '\t')
		{
			file_input.get();
			skip = file_input.peek();
		}
		c = file_input.get();
		if (c == EOF) return temp_node;
		else if (c != '{')throw bad_format("JSON file must be empty or start with '{'");



		while (true)
		{
			skip = file_input.peek();
			while (skip == ' ' || skip == '\n' || skip == '\r' || skip == '\t')
			{
				file_input.get();
				skip = file_input.peek();
			}
			//cout << parser_state << '\n';
			if (parser_state == 1)
			{
				if (file_input.peek() == '}')
				{
					parser_state = 4;
					continue;
				}
				temp_node = new Node();
				file_input >> *temp_node;
				if (temp_node->get_type() != STRING) throw bad_format("Field name must be enclosed inside quotation marks.");
				temp_node->set_name(*(temp_node->get_string_value()));
				parser_state = 2;
			}
			else if (parser_state == 2)
			{
				if (file_input.get() != ':') throw bad_format("Character ':' must follow field name.");
				parser_state = 3;
			}
			else if (parser_state == 3)
			{
				if (file_input.peek() == ']')
				{
					parser_state = 4;
					continue;
				}
				file_input >> *temp_node;
				node_type temp_type = temp_node->get_type();
				if (temp_type == JSON)
				{
					stack.push_back(temp_node);
					parser_state = 1;
				}
				else if (temp_type == ARRAY)
				{
					stack.push_back(temp_node);
					temp_node = new Node();
					parser_state = 3;

				}
				else
				{
					Node* parent = stack.back();
					parent->add_child(temp_node);
					parser_state = 4;
				}
			}
			else if (parser_state == 4)
			{
				c = file_input.get();
				Node* parent = stack.back();
				if (c == ',')
				{
					if (parent->get_type() == JSON) parser_state = 1;
					else if (parent->get_type() == ARRAY)
					{
						temp_node = new Node();
						parser_state = 3;
					}
					else throw unreachable_logic();
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
					parser_state = 4;
				}
				else if (c == ']')
				{
					if (parent->get_type() != ARRAY) throw bad_format("Unrecognized pattern.");
					temp_node = parent;
					stack.pop_back();
					parent = stack.back();
					parent->add_child(temp_node);
					parser_state = 4;
				}
			}



		}
	}
	catch (bad_format& e)
	{
		custom_is::adjust_indexes();
		cout <<"at column: "<< custom_is::column_index<<", row: " << custom_is::row_index <<" - " << "JSON parsing error: " << e.what();
		exit(9);
	}
	catch (escaped_char& e)
	{
		custom_is::adjust_indexes();
		cout << "at column: " << custom_is::column_index << ", row: " << custom_is::row_index << " - " << "JSON parsing error: " <<"cannot escape character " << e.get_escaped_character();
		exit(9);
	}
	catch (unreachable_logic& e)
	{
		custom_is::adjust_indexes();
		cout << "at column: " << custom_is::column_index << ", row: " << custom_is::row_index << " - " << "JSON parsing error: "<<e.what();
		exit(9);
	}
	catch (invalid_argument& e)
	{
		custom_is::adjust_indexes();
		cout << "at column: " << custom_is::column_index << ", row: " << custom_is::row_index << " - " << "JSON parsing error: value of string type field must be specified between two single quotation marks";
		exit(9);
	}
	catch (out_of_range& e)
	{
		custom_is::adjust_indexes();
		cout << "at column: " << custom_is::column_index << ", row: " << custom_is::row_index << " - " << "JSON parsing error: number field value out of range";
		exit(9);
	}
	catch (control_character& e)
	{
		custom_is::adjust_indexes();
		cout << "at column: " << custom_is::column_index << ", row: " << custom_is::row_index << " - " << "JSON parsing error: control character can't be part of a string value.";
		exit(9);
	}

	
	temp_node = stack.back();
	return temp_node;








}