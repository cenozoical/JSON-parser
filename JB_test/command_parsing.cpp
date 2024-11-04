#include "node.h"
#include <vector>
#include <sstream>
#include <regex>
#include <limits>
#include "custom_input_string_stream.h"
#include <cmath>
#include "repetetive_code.h"
using namespace std;


enum command_type
{
	MAX,
	MIN,
	SIZE,
	FIND,//default value
	SUM
};
enum num_op
{
	ADD = 0,
	SUB =  1,
	MUL = 2,
	DIV = 3

};
enum enclosure_type
{
	BRACES,
	BRACKETS

};

extern Node* root_node;
vector <Node*> list;

static string* read_name(custom_istr_stream &ss)
{
	string* sp = new string();
	int c = ss.peek();
	if (c == '+' || c == '-') *sp += ss.get();
	c = ss.peek();
	while (c != EOF && c != '.' && c != '[' && c != ']' && c!=',' && c!=' ' && c!= '\t' && c!= '+' && c != '-'
		&& c != '*' && c != '/' && c != '(' && c != ')')
	{
		ss.get();
		if (c != '\\') *sp += c;
		else
		{
			c = ss.get();
			if (c == 'b') *sp += '\b';
			else if (c == 'n')*sp += '\n';
			else if (c == ',')*sp += ',';
			else if (c == 'f')*sp += '\f';
			else if (c == 'r') *sp += '\r';
			else if (c == 't') *sp += '\t';
			else if (c == '.') *sp += '.';
			else if (c == '[') *sp += '[';
			else if (c == ']') *sp += ']';
			else if (c == '(') *sp += '(';
			else if (c == ')') *sp += ')';
			else if (c == '\\')*sp += '\\';
			else if (c == '/')*sp += '/';
			else if (c == '*')*sp += '*';
			else if (c == '+')*sp += '+';
			else if (c == '-')*sp += '-';
			else throw escaped_char(c);
		}
		c = ss.peek();
	}
	//cout << *sp ;
	return sp;
}


class semantic_error :public exception
{
public:

	semantic_error(const char* s) : exception(s)
	{

	}
};
class zero_division :public exception
{
};
class bad_operands :public exception
{
	char operation;
	bool  left_only, right_only, both;// +1 if left, +2 if right
public:
	bad_operands(num_op op, char lr):exception(),left_only(false),right_only(false),both(false)
	{
		char ops[] = { '+','-','*','/' };
		operation = ops[op];
		if (lr == 3) both = true;
		else if (lr == 2) right_only = true;
		else left_only = true;
	}
	char get_op()
	{
		return operation;
	}
	char get_lo()
	{
		return left_only;
	}
	char get_ro()
	{
		return right_only;
	}
	char get_both()
	{
		return both;
	}
};
class no_index :public exception
{
public:

	no_index(const char* s) : exception(s)
	{

	}
};
class bad_args :public exception
{
	int args_no;
public:

	bad_args(const char* s, int index) : exception(s)
	{
		args_no = index;
	}
	int get_index()
	{
		return args_no;
	}
};
class field_not_found :public exception
{
	string* s;
public:
	
	field_not_found(string *s) : exception()
	{
		this->s = s;
	}
	string* getString()
	{
		return s;
	}
};


static void parse_command_arguments(custom_istr_stream& ss)
{
	
	vector <vector<Node*>*> stack;
	vector <Node*>* temp_vect;
	vector <num_op> op_vect;
	vector<int> op_pos_vect;//for precise algebric error reporting
	vector<enclosure_type>enc_vector;
	Node* temp_node;


	int parser_state = 0;
	temp_vect = new vector<Node*>();
	temp_vect->push_back(root_node);
	stack.push_back(temp_vect);
	int skip;

	while (true)
	{
		skip = ss.peek();
		while (skip == '\t' || skip == ' ')
		{
			ss.get();
			skip = ss.peek();
		}
		if (parser_state == 0)
		{
			if (skip == '(')
			{
				ss.get();
				enc_vector.push_back(BRACES);

				if (stack.back()->back() != root_node)throw bad_format("Unexpected character");
				stack.back()->pop_back();
				temp_vect = new vector<Node*>();
				temp_vect->push_back(root_node);
				stack.push_back(temp_vect);
				continue;

			}
			string* s = read_name(ss);
			if (s->length() == 0 && (stack.back()->back() != root_node))
			{
				custom_istr_stream::erroneus_peek = true;
				custom_istr_stream::column_index -= s->length();
				throw bad_format("expected a field name.");
			}
			else if (s->length() == 0 && (stack.back()->back() == root_node))
			{
				custom_istr_stream::erroneus_peek = true;
				custom_istr_stream::column_index -= s->length();
				throw bad_format("expected a field name or a number literal.");
			}
			node_type temp_type = stack.back()->back()->get_type();
			if (temp_type != JSON)throw redundant_check_succeeded();
			
			temp_node = stack.back()->back()->get_child(s);
			if (temp_node != nullptr)
			{
				
					stack.back()->pop_back();
					stack.back()->push_back(temp_node);
					parser_state = 1;
					//continue;
			}
			
			//**name not found - can only be a number literal**
			
			else if (stack.back()->back() != root_node)
			{
				custom_istr_stream::erroneus_peek = true;
				custom_istr_stream::column_index -= s->length();
				throw field_not_found(s);
			}

			else 
			{
				try
				{

					float number = stof(*s, nullptr);//throws invalid_argument or out_of_range
					temp_node = new Node();
					temp_node->set_string_value(*s);
					temp_node->set_type(NUMBER);
					temp_node->set_number_value(number);
					stack.back()->pop_back();
					stack.back()->push_back(temp_node);
					parser_state = 2;
					
				}
				catch (invalid_argument& e)
				{
					custom_istr_stream::erroneus_peek = true;
					custom_istr_stream::column_index -= s->length();
					throw field_not_found(s);
				}
				catch (out_of_range& e)
				{
					custom_istr_stream::column_index -= s->length();
					custom_istr_stream::erroneus_peek = true;
					throw out_of_range(nullptr);
				}


			}
			calculate
			continue;

		}
		else if (parser_state == 1)
		{
			int c = ss.get();
			if (c == '[')
			{
				enc_vector.push_back(BRACKETS);
				if (stack.back()->back()->get_type() != ARRAY) throw semantic_error("only arrays can be indexed.");
				stack.push_back(new vector<Node*>());
				stack.back()->push_back(root_node);
				parser_state = 0;
				continue;
			}
			else if (c == ']')
			{
				int index;
				if(stack.size() < 2) throw bad_format("']' must follow '[', as well as an index argument, in a command argument.");
				if(enc_vector.back()!= BRACKETS) throw bad_format("']' cannot enclose ')'.");
				enc_vector.pop_back();
				Node* num_node = stack.back()->back();//it should be alone in the  top vector
				stack.pop_back();
				Node* arr_node = stack.back()->back();
				stack.back()->pop_back();

				if(num_node->get_type() != NUMBER)throw semantic_error("array index must be of type 'number'.");
				//if(num_node->get_number_value() != roundf(num_node->get_number_value()))throw semantic_error("array index must be an integer'.");
				if (arr_node->get_type() != ARRAY)throw redundant_check_succeeded();
				index = num_node->get_number_value();
				if(index >= arr_node->get_size())throw semantic_error("array index out of bounds.");

				temp_node = arr_node->get_child(index);
				stack.back()->push_back(temp_node);
				parser_state = 1;
				
				calculate
				continue;


			}
			else if (c == '.')
			{
				if(stack.back()->back()->get_type() != JSON)throw semantic_error("only JSON objects have named fields.");
				parser_state = 0;
				continue;
			}
			else if (c == ',')
			{
				if (stack.size() != 1)throw bad_format("opening bracket must be closed.");
				temp_node = stack.back()->back();
				stack.back()->pop_back();
				list.push_back(temp_node);
				stack.back()->push_back(root_node);
				parser_state = 0;
				continue;

			}
			else if (c == EOF)
			{
				if (stack.size() != 1)throw bad_format("opening bracket must be closed.");
				temp_node = stack.back()->back();
				stack.back()->pop_back();
				list.push_back(temp_node);
				break;
			}
			else if (c == '+')
			{
				
				stack.back()->push_back(root_node);
				op_vect.push_back(ADD);
				op_pos_vect.push_back(custom_istr_stream::column_index - 1);
				if (ss.peek() == '+' || ss.peek() == '-') throw bad_format("double numeric opeation is not allowed.");
				parser_state = 0;
				continue;
			}
			else if (c == '-')
			{
				stack.back()->push_back(root_node);
				op_vect.push_back(SUB);
				op_pos_vect.push_back(custom_istr_stream::column_index - 1);
				if (ss.peek() == '+' || ss.peek() == '-') throw bad_format("double numeric opeation is not allowed.");
				parser_state = 0;
				continue;
			}
			else if (c == '*')
			{
				stack.back()->push_back(root_node);
				op_vect.push_back(MUL);
				op_pos_vect.push_back(custom_istr_stream::column_index - 1);
				if (ss.peek() == '+' || ss.peek() == '-') throw bad_format("double numeric opeation is not allowed.");
				parser_state = 0;
				continue;
			}
			else if (c == '/')
			{
				stack.back()->push_back(root_node);
				op_vect.push_back(DIV);
				op_pos_vect.push_back(custom_istr_stream::column_index - 1);
				if (ss.peek() == '+' || ss.peek() == '-') throw bad_format("double numeric opeation is not allowed.");
				parser_state = 0;
				continue;
			}
			else if (c == ')')
			{
				
				if (stack.size() < 2) throw bad_format("')' must follow '('  in a command string.");
				if (enc_vector.back() != BRACES) throw bad_format("')' cannot enclose ']'.");
				enc_vector.pop_back();
				Node* num_node = stack.back()->back();//it should be alone in the  top vector
				stack.pop_back();

				if (num_node->get_type() != NUMBER)throw semantic_error("expression inside baces must be of type 'number'.");

				stack.back()->push_back(num_node);
				
				calculate

					parser_state = 2;
				continue;


			}
			else
			{
				throw bad_format("Unexpected character");
			}

		}
		else if (parser_state == 2)//node representing flaotaing point literal must be on top of the stack
		{
			int c = ss.get();
			if(c == ']')
			{
				int index;
				if (stack.size() < 2) throw bad_format("']' must follow '[', as well as an index argument, in a command argument.");
				if (enc_vector.back() != BRACKETS) throw bad_format("']' cannot enclose ')'.");
				enc_vector.pop_back();
				Node* num_node = stack.back()->back();
				stack.pop_back();
				Node* arr_node = stack.back()->back();
				stack.back()->pop_back();

				if (num_node->get_type() != NUMBER)throw semantic_error("array index must be of type 'number'.");
				//if (num_node->get_number_value() != roundf(num_node->get_number_value()))throw semantic_error("array index must be an integer'.");
				if (arr_node->get_type() != ARRAY)throw redundant_check_succeeded();
				index = num_node->get_number_value();
				if (index >= arr_node->get_size())throw semantic_error("array index out of bounds.");

				temp_node = arr_node->get_child(index);
				stack.back()->push_back(temp_node);
				parser_state = 1;



				calculate
				continue;


			}
			else if(c == ',')
			{
				if (stack.size() != 1)throw bad_format("opening bracket must be closed.");
				temp_node = stack.back()->back();
				stack.back()->pop_back();
				list.push_back(temp_node);
				stack.back()->push_back(root_node);
				parser_state = 0;
				continue;

			}
			else if (c == EOF)
			{
				if (stack.size() != 1)throw bad_format("opening bracket must be closed.");
				temp_node = stack.back()->back();
				stack.back()->pop_back();
				list.push_back(temp_node);
				break;
			}
			else if (c == '+')
			{

				stack.back()->push_back(root_node);
				op_vect.push_back(ADD);
				op_pos_vect.push_back(custom_istr_stream::column_index - 1);
				if (ss.peek() == '+' || ss.peek() == '-') throw bad_format("double numeric opeation is not allowed.");
				parser_state = 0;
				continue;
			}
			else if (c == '-')
			{
				stack.back()->push_back(root_node);
				op_vect.push_back(SUB);
				op_pos_vect.push_back(custom_istr_stream::column_index - 1);
				if (ss.peek() == '+' || ss.peek() == '-') throw bad_format("double numeric opeation is not allowed.");
				parser_state = 0;
				continue;
			}
			else if (c == '*')
			{
				stack.back()->push_back(root_node);
				op_vect.push_back(MUL);
				op_pos_vect.push_back(custom_istr_stream::column_index - 1);
				if (ss.peek() == '+' || ss.peek() == '-') throw bad_format("double numeric opeation is not allowed.");
				parser_state = 0;
				continue;
			}
			else if (c == '/')
			{
				stack.back()->push_back(root_node);
				op_vect.push_back(DIV);
				op_pos_vect.push_back(custom_istr_stream::column_index - 1);
				if (ss.peek() == '+' || ss.peek() == '-') throw bad_format("double numeric opeation is not allowed.");
				parser_state = 0;
				continue;
			}
			else if (c == ')')
			{

				if (stack.size() < 2) throw bad_format("')' must follow '('  in a command string.");
				Node* num_node = stack.back()->back();//it should be alone in the  top vector
				stack.pop_back();

				if (num_node->get_type() != NUMBER)throw semantic_error("expression inside baces must be of type 'number'.");

				stack.back()->push_back(num_node);

				calculate

					parser_state = 1;
				continue;


			}
			else
			{
				throw bad_format("Unexpected character");
			}

		}

	}

}



void execute_command(const char* command)
{
	command_type temp_cmd_type = FIND;
	string s = string(command);
	if (s.length() > 5 && s.at(s.length() - 1) == ')')
	{
		if (s.at(0) == 'm' && s.at(1) == 'a' && s.at(2) == 'x' && s.at(3) == '(')
		{
			temp_cmd_type = MAX;
			s.erase(s.length() - 1);
			s.erase(0, 4);
			custom_istr_stream::column_index = 5;
			//cout << s << endl;
		}
		else if (s.at(0) == 'm' && s.at(1) == 'i' && s.at(2) == 'n' && s.at(3) == '(')
		{
			temp_cmd_type = MIN;
			s.erase(s.length() - 1);
			s.erase(0, 4);
			custom_istr_stream::column_index = 5;
			//cout << s << endl;
		}
		else if (s.at(0) == 's' && s.at(1) == 'u' && s.at(2) == 'm' && s.at(3) == '(')
		{
			temp_cmd_type = SUM;
			s.erase(s.length() - 1);
			s.erase(0, 4);
			custom_istr_stream::column_index = 5;
			//cout << s << endl;
		}
		else if (s.length() > 6 && s.at(0) == 's' && s.at(1) == 'i' && s.at(2) == 'z' && s.at(3) == 'e' && s.at(4) == '(')
		{
			temp_cmd_type = SIZE;
			s.erase(s.length() - 1);
			s.erase(0, 5);
			custom_istr_stream::column_index = 6;
			//cout << s << endl;
		}
	}

	custom_istr_stream ss(s);
	try
	{
		parse_command_arguments(ss);

		if (temp_cmd_type == FIND)
		{
			if (list.size() != 1) throw no_index("multiple command arguments may only be passed inside max, min or sum directives.");
			cout << *list.back();
		}
		else if (temp_cmd_type == SIZE)
		{
			if (list.size() != 1) throw no_index("multiple command arguments may only be passed inside max, min or sum directives.");
			Node* temp_node = list.back();
			node_type temp_type = temp_node->get_type();
			if (temp_type == STRING)
			{
				cout << temp_node->get_string_value()->length();
			}
			else if (temp_type == ARRAY || temp_type == JSON)
			{
				cout << temp_node->get_size();
			}
			else throw no_index("argument of 'size' directive may only be of STRING, JSON or ARRAY type");
		}
		else if (temp_cmd_type == MAX)
		{
			float max_float = numeric_limits<float>::min();
			Node* temp_node, * max_node;
			max_node = nullptr;//compiler error supression
			if (list.back()->get_type() == ARRAY)
			{
				if (list.size() != 1) throw bad_args("ARRAY type argument of 'max' directive must be a single argument.", list.size() );
				for (int i = 0; i < list.back()->get_size(); i++)
				{
					temp_node = list.back()->get_child(i);
					if (temp_node->get_type() != NUMBER) throw no_index("elements of array passed to 'max' directive can only be of type NUMBER.");
					if (temp_node->get_number_value() >= max_float)
					{
						max_float = temp_node->get_number_value();
						max_node = temp_node;
					}

				}
			}
			else
			{
				for (int i = 0; i < list.size(); i++)
				{
					temp_node = list.at(i);
					if (temp_node->get_type() == ARRAY) throw bad_args("ARRAY type argument of 'max' directive must be a single argument.", i +1);
					if (temp_node->get_type() != NUMBER) throw bad_args("arguments of 'max' directive may only be of NUMBER or ARRAY type.", i+1 );
					if (temp_node->get_number_value() >= max_float)
					{
						max_float = temp_node->get_number_value();
						max_node = temp_node;
					}
				}
			}
			cout << *max_node;

		}
		else if (temp_cmd_type == MIN)
		{
			float min_float = numeric_limits<float>::max();
			Node* temp_node, * min_node;
			min_node = nullptr;//compiler error supression
			if (list.back()->get_type() == ARRAY)
			{
				if (list.size() != 1) throw bad_args("ARRAY type argument of 'min' directive must be a single argument.",list.size() );
				for (int i = 0; i < list.back()->get_size(); i++)
				{
					temp_node = list.back()->get_child(i);
					if (temp_node->get_type() != NUMBER) throw no_index("elements of array passed to 'min' directive can only be of type NUMBER.");
					if (temp_node->get_number_value() <= min_float)
					{
						min_float = temp_node->get_number_value();
						min_node = temp_node;
					}

				}
			}
			else
			{
				for (int i = 0; i < list.size(); i++)
				{
					temp_node = list.at(i);
					if (temp_node->get_type() == ARRAY) throw bad_args("ARRAY type argument of 'min' directive must be a single argument.", i +1);
					if (temp_node->get_type() != NUMBER) throw bad_args("arguments of 'min' directive may only be of NUMBER or ARRAY type.", i +1);
					if (temp_node->get_number_value() <= min_float)
					{
						min_float = temp_node->get_number_value();
						min_node = temp_node;
					}
				}
			}
			cout << *min_node;
		}
		else if (temp_cmd_type == SUM)
		{
			float sum = 0;
			Node* temp_node, * sum_node;
			sum_node = new Node();//compiler error supression
			sum_node->set_type(NUMBER);
			if (list.back()->get_type() == ARRAY)
			{
				if (list.size() != 1) throw bad_args("ARRAY type argument of 'sum' directive must be a single argument.", list.size());
				for (int i = 0; i < list.back()->get_size(); i++)
				{
					temp_node = list.back()->get_child(i);
					if (temp_node->get_type() != NUMBER) throw no_index("elements of array passed to 'sum' directive can only be of type NUMBER.");
					sum += temp_node->get_number_value();

				}
			}
			else
			{
				for (int i = 0; i < list.size(); i++)
				{
					temp_node = list.at(i);
					if (temp_node->get_type() == ARRAY) throw bad_args("ARRAY type argument of 'sum' directive must be a single argument.", i +1);
					if (temp_node->get_type() != NUMBER) throw bad_args("arguments of 'sum' directive may only be of NUMBER or ARRAY type.", i +1);
					sum += temp_node->get_number_value();
				}
			}
			sum_node->set_string_value(*(new string(to_string(sum))));
			cout << *sum_node;
		}
	}
	catch (bad_format& e)
	{
		if (!custom_istr_stream::erroneus_peek) custom_istr_stream::column_index--;
		cout << "at position " << custom_istr_stream::column_index << ": " << "Command parsing error: " << e.what();
		exit(9);
	}
	catch (semantic_error& e)
	{
		if (!custom_istr_stream::erroneus_peek) custom_istr_stream::column_index--;
		cout << "at position " << custom_istr_stream::column_index << ": " << "Command parsing error: " << e.what();
		exit(9);
	}
	catch (escaped_char& e)
	{
		if (!custom_istr_stream::erroneus_peek) custom_istr_stream::column_index--;
		cout << "at position " << custom_istr_stream::column_index << ": " << "Command parsing error: cannot escape character '" << e.get_escaped_character() << "'.";
		exit(9);
	}
	catch (out_of_range& e)
	{
		if (!custom_istr_stream::erroneus_peek) custom_istr_stream::column_index--;
		cout << "at position " << custom_istr_stream::column_index << ": " << "Command parsing error: number specified is out of floating point range.";
		exit(9);
	}
	catch (invalid_argument & e)
	{
		if (!custom_istr_stream::erroneus_peek) custom_istr_stream::column_index--;
		cout << "at position " << custom_istr_stream::column_index << ": " << "Command parsing error: field name could not be found.";
		exit(9);
	}
	catch (field_not_found& e)
	{
		if (!custom_istr_stream::erroneus_peek) custom_istr_stream::column_index--;
		cout << "at position " << custom_istr_stream::column_index << ": " << "Command parsing error: field " << *e.getString() << " could not be found.";
		exit(9);
	}
	catch (zero_division& e)
	{
		cout << "at position " << custom_istr_stream::column_index<<": "<< "Command parsing error: division by zero is not allowed.";
		exit(9);
	}
	catch (bad_operands& e)
	{
		if (e.get_lo()) cout << "Command parsing error: operations ('" << e.get_op() << "', at position " << custom_istr_stream::column_index
			<< ") left operand is not of NUMBER type.";
		else if(e.get_ro()) cout << "Command parsing error: operations ('" << e.get_op() << "', at position " << custom_istr_stream::column_index
			<< ") right operand is not of NUMBER type.";
		else cout << "Command parsing error: operations ('" << e.get_op() << "', at position " << custom_istr_stream::column_index
			<< ") operands are not of NUMBER type.";
		exit(9);
	}
	catch (redundant_check_succeeded & e)//should never be caught
	{
		cout << "Panic: redundant check succeeded.";
		exit(9);
	}
	catch (bad_args& e)
	{
		cout << "Command parsing error: (argument number " << e.get_index() << ") " << e.what();
		exit(9);
	}
	catch (no_index& e)
	{
		
		cout << "Command parsing error: " << e.what();
		exit(9);
	
	}

	//cout << *(list.back());
	//if (temp_cmd_type == FIND)
	//{
	//	if (list.size() != 1) throw bad_format("multiple command arguments may only be passed inside max or min directives.");
	//	cout << list.back();
	//}
	//else if (temp_cmd_type == SIZE)
	//{
	//	if (list.size() != 1) throw bad_format("multiple command arguments may only be passed inside max or min directives.");
	//	Node* temp_node = list.back();
	//	node_type temp_type = temp_node->get_type();
	//	if (temp_type == STRING)
	//	{
	//		cout << temp_node->get_string_value()->length();
	//	}
	//	else if (temp_type == ARRAY || temp_type == JSON)
	//	{
	//		cout << temp_node->get_size();
	//	}
	//	else throw semantic_error("argument of 'size' directive may only be of STRING, JSON or ARRAY type");
	//}
	//else if (temp_cmd_type == MAX)
	//{
	//	float max_float = numeric_limits<float>::min();
	//	Node* temp_node, * max_node;
	//	max_node = nullptr;//compiler error supression
	//	for (int i = 0; i < list.size(); i++)
	//	{
	//		temp_node = list.at(i);
	//		if(temp_node->get_type()!= NUMBER) throw semantic_error("argument of 'max' directive may only be of NUMBER type.");
	//		if (temp_node->get_number_value() >= max_float)
	//		{
	//			max_float = temp_node->get_number_value();
	//			max_node = temp_node;
	//		}
	//	}
	//	cout << * max_node;

	//}
	//else if (temp_cmd_type == MIN)
	//{
	//	float min_float = numeric_limits<float>::max();
	//	Node* temp_node, * min_node;
	//	min_node = nullptr;//compiler error supression
	//	for (int i = 0; i < list.size(); i++)
	//	{
	//		temp_node = list.at(i);
	//		if (temp_node->get_type() != NUMBER) throw semantic_error("argument of 'max' directive may only be of NUMBER type.");
	//		if (temp_node->get_number_value() <= min_float)
	//		{
	//			min_float = temp_node->get_number_value();
	//			min_node = temp_node;
	//		}
	//	}
	//	cout << *min_node;
	//}

}
