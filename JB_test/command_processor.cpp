#include "command_processor.h"
#include "exception_classes.h"

command_processor::command_processor(Node* root_node, const char* command) :ss(command), root_node(root_node) {};

string* command_processor::read_name()
{
	string* sp = new string();
	int c;
	if ((c = ss.peek()) == '+' || c == '-') *sp += ss.get();

	while ((c = ss.peek()) != EOF && c != '.' && c != '[' && c != ']' && c != ',' && c != ' ' && c != '\t' && c != '+' && c != '-'
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
	}
	return sp;
}

//checks to see whether any arithmetic operations are ready to be executed
void command_processor::calculate()
{
	int c;
	while ((c = ss.peek()) == ' ' || c == '\t')ss.get();
	num_op temp_op = ADD;
	if ((c == '*') || (c == '/'))temp_op = MUL;
	if (c == ']' || c == ',' || c == EOF || c == '+' || c == '-' || c == '*' || c == '/' || c == ')')
		while (scopes.back()->size() > 1 && op_vect.size() > 0)
		{
			if (op_vect.back() < temp_op)break;
			Node* op1, * op2;
			op1 = scopes.back()->back();
			scopes.back()->pop_back();
			op2 = scopes.back()->back();
			scopes.back()->pop_back();
			if (op1->get_type() != NUMBER || op2->get_type() != NUMBER)
			{
				char lr = 0;// a code that tells exception handler what operands are illegal
				if (op1->get_type() != NUMBER) lr += 2;
				if (op2->get_type() != NUMBER)lr += 1;
				ss.set_column_index(op_pos_vect.back());
				throw bad_operands(op_vect.back(), lr);
			}
			Node* temp_node = new Node();
			float value;
			if (op_vect.back() == ADD)
			{
				value = op1->get_number_value() + op2->get_number_value();
			}
			else if (op_vect.back() == SUB)
			{
				value = op2->get_number_value() - op1->get_number_value();
			}
			else if (op_vect.back() == MUL)
			{
				value = op1->get_number_value() * op2->get_number_value();
			}
			else
			{

				if (op1->get_number_value() == 0)
				{
					ss.set_column_index(op_pos_vect.back());
					throw zero_division();
				}
				value = op2->get_number_value() / op1->get_number_value(); \
			}
			temp_node->set_number_value(value);
			temp_node->set_type(NUMBER);
			string* temp_str = new string(to_string(value));
			temp_node->set_string_value(*temp_str);
			op_vect.pop_back();
			op_pos_vect.pop_back();
			scopes.back()->push_back(temp_node);

		}
}
//executes last intrinsic function to be invoked
void command_processor::execute_intrinsic_function()
{

	intrinsic_function_type intr_type = instrinsic_scopes.back();
	instrinsic_scopes.pop_back();
	auto scope = scopes.back();
	scopes.pop_back();


	Node* value = new Node();
	value->set_type(NUMBER);

	if (intr_type == NONE)
	{
		Node* num_node = scope->back();//it should be alone in the  top vector

		if (num_node->get_type() != NUMBER)throw semantic_error("expression inside braces must be of type 'number'.");

		delete value;
		value = num_node;
	}
	else if (intr_type == SIZE)
	{
		if (scope->size() != 1) throw no_index("multiple command arguments may only be passed inside max, min or sum directives.");
		Node* temp_node = scope->back();



		node_type temp_type = temp_node->get_type();

		if (temp_type == STRING) value->set_number_value(temp_node->get_string_value()->length());
		else if (temp_type == ARRAY || temp_type == JSON) value->set_number_value(temp_node->get_size());
		else throw no_index("argument of 'size' directive may only be of STRING, JSON or ARRAY type");


	}
	else if (intr_type == MAX)
	{
		float max_float = numeric_limits<float>::min();
		Node* temp_node;
		if (scope->back()->get_type() == ARRAY)
		{
			if (scope->size() > 1) throw bad_format("ARRAY type argument of 'max' intrinsic function must be a single argument.");
			Node* arr = scope->back();
			for (int i = 0; i < arr->get_size(); i++)
			{
				temp_node = arr->get_child(i);
				if (temp_node->get_type() != NUMBER) throw no_index("elements of array passed to 'max' directive can only be of type NUMBER.");
				if (temp_node->get_number_value() > max_float) max_float = temp_node->get_number_value();
			}
		}
		else
		{
			for (int i = 0; i < scope->size(); i++)
			{
				temp_node = scope->at(i);
				if (temp_node->get_type() == ARRAY) throw bad_args("ARRAY type argument of 'max' directive must be a single argument.", i + 1);
				if (temp_node->get_type() != NUMBER) throw bad_args("arguments of 'max' directive may only be of NUMBER or ARRAY type.", i + 1);
				if (temp_node->get_number_value() > max_float)max_float = temp_node->get_number_value();
			}
		}
		value->set_number_value(max_float);
	}
	else if (intr_type == MIN)
	{
		float min_float = numeric_limits<float>::max();
		Node* temp_node;
		if (scope->back()->get_type() == ARRAY)
		{
			if (scope->size() > 1) throw bad_format("ARRAY type argument of 'max' intrinsic function must be a single argument.");
			Node* arr = scope->back();
			for (int i = 0; i < arr->get_size(); i++)
			{
				temp_node = arr->get_child(i);
				if (temp_node->get_type() != NUMBER) throw no_index("elements of array passed to 'min' directive can only be of type NUMBER.");
				if (temp_node->get_number_value() < min_float) min_float = temp_node->get_number_value();
			}
		}
		else
		{
			for (int i = 0; i < scope->size(); i++)
			{
				temp_node = scope->at(i);
				if (temp_node->get_type() == ARRAY) throw bad_args("ARRAY type argument of 'min' directive must be a single argument.", i + 1);
				if (temp_node->get_type() != NUMBER) throw bad_args("arguments of 'min' directive may only be of NUMBER or ARRAY type.", i + 1);
				if (temp_node->get_number_value() < min_float)min_float = temp_node->get_number_value();
			}
		}
		value->set_number_value(min_float);

	}
	else if (intr_type == SUM)
	{
		float sum = 0;
		Node* temp_node;
		if (scope->back()->get_type() == ARRAY)
		{
			if (scope->size() != 1) throw bad_args("ARRAY type argument of 'sum' directive must be a single argument.", scope->size());
			for (int i = 0; i < scope->back()->get_size(); i++)
			{
				temp_node = scope->back()->get_child(i);
				if (temp_node->get_type() != NUMBER) throw no_index("elements of array passed to 'sum' directive can only be of type NUMBER.");
				sum += temp_node->get_number_value();

			}
		}
		else
		{
			for (int i = 0; i < scope->size(); i++)
			{
				temp_node = scope->at(i);
				if (temp_node->get_type() == ARRAY) throw bad_args("ARRAY type argument of 'sum' directive must be a single argument.", i + 1);
				if (temp_node->get_type() != NUMBER) throw bad_args("arguments of 'sum' directive may only be of NUMBER or ARRAY type.", i + 1);
				sum += temp_node->get_number_value();
			}
		}
		//value->set_string_value(*(new string(to_string(sum))));
		value->set_number_value(sum);
	}
	string* temp_str = new string(to_string(value->get_number_value()));
	value->set_string_value(*temp_str);
	scopes.back()->push_back(value);
}
//tries to execute a command specified
Node* command_processor::execute_command_inner()
{
	vector <Node*>* temp_scope;
	Node* temp_node;

	/*
	While parser is in state 0 it expects a field name, a number literal or an intrinsic function.
	If the parser in state 0 reads field name it moves to state 1. If it reads a number literal it moves to state 2.

	The only difference between states 1 and 2 is that the state 1 can read characters '.' and '['.
	It wouldn't make sense to access the field of a number literal, unless such a literal is a field name, in which case
	parser should recognize it as such.
	*/
	int parser_state = 0;
	temp_scope = new vector<Node*>();
	temp_scope->push_back(root_node);
	scopes.push_back(temp_scope);
	instrinsic_scopes.push_back(NONE);

	while (true)
	{

		while (ss.peek() == '\t' || ss.peek() == ' ') ss.get();
		if (parser_state == 0)
		{
			if (ss.peek() == '(')
			{
				ss.get();
				enc_vector.push_back(BRACES);
				instrinsic_scopes.push_back(NONE);

				if (scopes.back()->back() != root_node)throw bad_format("Unexpected character");
				scopes.back()->pop_back();
				temp_scope = new vector<Node*>();
				temp_scope->push_back(root_node);
				scopes.push_back(temp_scope);
				continue;

			}
			string* s = read_name();
			bool clean_slate = scopes.back()->back() == root_node;
			if (clean_slate && *s == "size" && ss.peek() == '(')
			{
				ss.get();

				enc_vector.push_back(BRACES);

				instrinsic_scopes.push_back(SIZE);

				scopes.back()->pop_back();
				temp_scope = new vector<Node*>();
				temp_scope->push_back(root_node);
				scopes.push_back(temp_scope);
				continue;
			}
			else if (clean_slate && *s == "max" && ss.peek() == '(')
			{
				ss.get();

				enc_vector.push_back(BRACES);

				instrinsic_scopes.push_back(MAX);

				scopes.back()->pop_back();
				temp_scope = new vector<Node*>();
				temp_scope->push_back(root_node);
				scopes.push_back(temp_scope);
				continue;
			}
			else if (clean_slate && *s == "min" && ss.peek() == '(')
			{
				ss.get();

				enc_vector.push_back(BRACES);

				instrinsic_scopes.push_back(MIN);

				scopes.back()->pop_back();
				temp_scope = new vector<Node*>();
				temp_scope->push_back(root_node);
				scopes.push_back(temp_scope);
				continue;
			}
			else if (clean_slate && *s == "sum" && ss.peek() == '(')
			{
				ss.get();

				enc_vector.push_back(BRACES);

				instrinsic_scopes.push_back(SUM);

				scopes.back()->pop_back();
				temp_scope = new vector<Node*>();
				temp_scope->push_back(root_node);
				scopes.push_back(temp_scope);
				continue;
			}
			else if (s->length() == 0 && (scopes.back()->back() != root_node))
			{
				ss.peek();
				ss.subtract_column_index(s->length());
				throw bad_format("expected a field name.");
			}
			else if (s->length() == 0 && (scopes.back()->back() == root_node))
			{
				ss.peek();
				ss.subtract_column_index(s->length());
				throw bad_format("expected a field name or a number literal.");
			}
			node_type temp_type = scopes.back()->back()->get_type();
			if (temp_type != JSON)throw redundant_check_succeeded();//program should never throw here

			temp_node = scopes.back()->back()->get_child(s);
			if (temp_node != nullptr)
			{

				scopes.back()->pop_back();
				scopes.back()->push_back(temp_node);

				parser_state = 1;
			}

			//**name not found - can only be a number literal**

			else if (scopes.back()->back() != root_node)
			{
				ss.peek();
				ss.subtract_column_index(s->length());
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
					scopes.back()->pop_back();
					scopes.back()->push_back(temp_node);
					parser_state = 2;

				}
				catch (invalid_argument& e)
				{
					ss.peek();
					ss.subtract_column_index(s->length());
					throw field_not_found(s);
				}
				catch (out_of_range& e)
				{
					ss.peek();
					ss.subtract_column_index(s->length());
					throw out_of_range(nullptr);
				}


			}
			calculate();
			continue;

		}
		else
		{
			int c = ss.get();
			if (c == ']')
			{
				int index;
				if (scopes.size() < 2) throw bad_format("']' must follow '[', as well as an index argument, in a command argument.");
				if (enc_vector.back() != BRACKETS) throw bad_format("']' cannot enclose '('.");
				enc_vector.pop_back();
				instrinsic_scopes.pop_back();
				Node* num_node = scopes.back()->back();//it should be alone in the  top vector
				scopes.pop_back();
				Node* arr_node = scopes.back()->back();
				scopes.back()->pop_back();

				if (num_node->get_type() != NUMBER)
				{
					ss.subtract_column_index(1);//so that error reporting index doesn't point at the comma.
					throw semantic_error("array index must be of type 'number'.");

				}
				if (arr_node->get_type() != ARRAY)throw redundant_check_succeeded();
				index = num_node->get_number_value();
				if (index >= arr_node->get_size())
				{
					ss.subtract_column_index(1);//so that error reporting index doesn't point at the comma.
					throw semantic_error("array index out of bounds.");
				}
				temp_node = arr_node->get_child(index);
				scopes.back()->push_back(temp_node);
				parser_state = 1;

				calculate();
				continue;


			}
			else if (c == ',')
			{

				if (instrinsic_scopes.back() == NONE) throw bad_format("unexpected character.");
				else if (scopes.back()->back()->get_type() != NUMBER)
				{
					if (scopes.back()->back()->get_type() == ARRAY) throw bad_args("ARRAY type argument of intrinsic function must be a single argument.",scopes.back()->size());
					else throw bad_args("elements passed to intrinsic functions may be of type ARRAY or NUMBER", scopes.back()->size());
				}
				scopes.back()->push_back(root_node);
				parser_state = 0;
				continue;


			}
			else if (c == EOF)
			{
				if (scopes.size() != 1)
					if(enc_vector.back() == BRACES) throw bad_format("opened braces must be closed.");
					else if (enc_vector.back() == BRACKETS) throw bad_format("opened brackets must be closed.");
				return scopes.back()->back();
			}
			else if (c == '+')
			{

				scopes.back()->push_back(root_node);
				op_vect.push_back(ADD);
				op_pos_vect.push_back(ss.get_column_index());
				if (ss.peek() == '+' || ss.peek() == '-') throw bad_format("double numeric opeation is not allowed.");
				parser_state = 0;
				continue;
			}
			else if (c == '-')
			{
				scopes.back()->push_back(root_node);
				op_vect.push_back(SUB);
				op_pos_vect.push_back(ss.get_column_index());
				if (ss.peek() == '+' || ss.peek() == '-') throw bad_format("double numeric opeation is not allowed.");
				parser_state = 0;
				continue;
			}
			else if (c == '*')
			{
				scopes.back()->push_back(root_node);
				op_vect.push_back(MUL);
				op_pos_vect.push_back(ss.get_column_index());
				if (ss.peek() == '+' || ss.peek() == '-') throw bad_format("double numeric opeation is not allowed.");
				parser_state = 0;
				continue;
			}
			else  if (c == '/')
			{
				scopes.back()->push_back(root_node);
				op_vect.push_back(DIV);
				op_pos_vect.push_back(ss.get_column_index());
				if (ss.peek() == '+' || ss.peek() == '-') throw bad_format("double numeric opeation is not allowed.");
				parser_state = 0;
				continue;
			}
			else if (c == ')')
			{
				if (scopes.size() < 2) throw bad_format("')' must follow '('  in a command string.");
				if (enc_vector.back() != BRACES) throw bad_format("')' cannot enclose '['.");
				enc_vector.pop_back();
				execute_intrinsic_function();
				calculate();
				parser_state = 2;
				continue;
			}
			else if (parser_state == 1 && c == '[')
			{
				enc_vector.push_back(BRACKETS);
				instrinsic_scopes.push_back(NONE);
				if (scopes.back()->back()->get_type() != ARRAY) throw semantic_error("only arrays can be indexed.");
				scopes.push_back(new vector<Node*>());
				scopes.back()->push_back(root_node);
				parser_state = 0;
				continue;
			}

			else if (parser_state == 1 && c == '.')
			{
				if (scopes.back()->back()->get_type() != JSON)throw semantic_error("only JSON objects have named fields.");
				parser_state = 0;
				continue;
			}
			else
			{
				throw bad_format("Unexpected character");
			}
		}
	}

}

//wrapper around execute_command_inner() that handles exceptions thrown
Node* command_processor::execute_command()
{

	
	try
	{
		return execute_command_inner();

	}
	catch (bad_format& e)
	{
		cout << "at position " << ss.get_column_index() << ": " << "Command parsing error: " << e.what();
		exit(9);
	}
	catch (semantic_error& e)
	{
		cout << "at position " << ss.get_column_index() << ": " << "Command parsing error: " << e.what();
		exit(9);
	}
	catch (escaped_char& e)
	{
		cout << "at position " << ss.get_column_index() << ": " << "Command parsing error: cannot escape character '" << e.get_escaped_character() << "'.";
		exit(9);
	}
	catch (out_of_range& e)
	{
		cout << "at position " << ss.get_column_index() << ": " << "Command parsing error: number specified is out of floating point range.";
		exit(9);
	}
	catch (invalid_argument& e)
	{
		cout << "at position " << ss.get_column_index() << ": " << "Command parsing error: field name could not be found.";
		exit(9);
	}
	catch (field_not_found& e)
	{
		cout << "at position " << ss.get_column_index() << ": " << "Command parsing error: field " << *e.getString() << " could not be found.";
		exit(9);
	}
	catch (zero_division& e)
	{
		cout << "at position " << ss.get_column_index() << ": " << "Command parsing error: division by zero is not allowed.";
		exit(9);
	}
	catch (bad_operands& e)
	{
		if (e.get_lo()) cout << "Command parsing error: operations ('" << e.get_op() << "', at position " << ss.get_column_index()
			<< ") left operand is not of NUMBER type.";
		else if (e.get_ro()) cout << "Command parsing error: operations ('" << e.get_op() << "', at position " << ss.get_column_index()
			<< ") right operand is not of NUMBER type.";
		else cout << "Command parsing error: operations ('" << e.get_op() << "', at position " << ss.get_column_index()
			<< ") operands are not of NUMBER type.";
		exit(9);
	}
	catch (redundant_check_succeeded& e)//should never be caught
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


}