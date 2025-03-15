#include "node.h"
#include <cctype>
#include "exception_classes.h"

Node::Node() :name(nullptr), str_value(nullptr), sibling(nullptr), children(nullptr)
{

}
void Node::set_type(node_type type)
{
	this->type = type;
}
void Node::set_number_value(float value)
{
	this->number_value = value;
}
void Node::set_string_value(string& value)
{
	this->str_value = &value;
}
void Node::set_name(string& value)
{
	this->name = &value;
}
void Node::set_bool_value(bool value)
{
	this->bool_value = value;
}
void Node::name_the_node(string& name)
{
	this->name = &name;
}

float Node::get_number_value()
{
	return this->number_value;
}
bool Node::get_bool_value()
{
	return this->bool_value;
}
node_type Node::get_type()
{
	return this->type;
}
string* Node::get_string_value()
{
	return this->str_value;
}
string* Node::get_name()
{
	return this->name;
}
int Node::get_size()
{
	return arr_size;
}
Node* Node::get_child(string* s)
{
	Node* temp = this->children;
	while (temp != nullptr)
	{
		if (*s == *(temp->name))break;
		temp = temp->sibling;
	}
	return temp;
}
Node* Node::get_child(int index)//throws nullptr exception
{
	Node* temp = this->children;
	for (int i = 0; i < index; i++) temp = temp->sibling;
	return temp;
		
	
}
void Node::add_child(Node* node)
{
	Node** temp = &(this->children);
	while (*temp != nullptr) temp = &((*temp)->sibling);
	*temp = node;
	arr_size++;
}

custom_is& operator >> (custom_is &is , Node& node)
{
	char c = is.peek();
	if (c == '"') 
	{
		node.str_value = new string();
		is.get();
		while ((c = is.get()) != '"')
		{
			if (c == EOF) throw bad_format("Syntax error");
			else if (iscntrl(c))throw control_character();
			else if (c != '\\') *( node.str_value) += c;
			else
			{
				c = is.get();
				if (c == 'b') *(node.str_value) += '\b';
				else if (c == 'n')*( node.str_value) += '\n';
				else if (c == 'f')*( node.str_value) += '\f';
				else if (c == 'r') *(node.str_value) += '\r';
				else if (c == 't') *(node.str_value) += '\t';
				else if (c == '"') *(node.str_value) += '"';
				else if (c == '\\')*(node.str_value) += '\\';
				else if (c == '/')*(node.str_value) += '/';
				else throw escaped_char(c);
			}
			node.type = STRING;
		}
		
	}
	else if (isdigit(is.peek()) || is.peek() == '+' || is.peek() == '-')
	{
		bool point_encountered = false;
		string *str_float = new string();
		if (c == '+' || c == '-') *str_float += is.get();
		c = is.peek();
		while (isdigit(c) || c == '.')
		{
			is.get();
			if (c == '.')
			{
				if (point_encountered) throw bad_format("JSON number cannot have multiple floating points.");
				if (!isdigit(is.peek())) throw bad_format("JSON number cannot end with a floating point.");
				point_encountered = true;
			}
			*str_float += c;
			c = is.peek();
		}
		try
		{

			node.str_value = str_float;
			node.number_value = stof(*str_float, nullptr);//throws invalid_argument or out_of_range
			node.type = NUMBER;
		}
		catch (out_of_range& e)
		{
			
			is.subtract_column_index(str_float->length());
			is.peek();//prevents column index decrementation when printing errors
			throw out_of_range(e);
		}
		


	}
	else if (c == 't')
	{

		is.get();
		if ((c = is.get()) != 'r') throw bad_format("String value must be enclosed inside double quotation marks.");
		if ((c = is.get()) != 'u') throw bad_format("Unrecognized syntax (did you mean 'true'?).");
		if ((c = is.get()) != 'e') throw bad_format("Unrecognized syntax (did you mean 'true'?).");
		node.type = BOOLEAN;
		node.bool_value = true;
	}
	else if (c == 'f')
	{
		is.get();
		if ((c = is.get()) != 'a') throw bad_format("String value must be enclosed inside double quotation marks.");
		if ((c = is.get()) != 'l') throw bad_format("String value must be enclosed inside double quotation marks.");
		if ((c = is.get()) != 's') throw bad_format("Unrecognized syntax (did you mean 'false'?).");
		if ((c = is.get()) != 'e') throw bad_format("Unrecognized syntax (did you mean 'false'.?)x");
		node.type = BOOLEAN;
		node.bool_value = false;
	}
	else if (c == 'n')
	{
		is.get();
		if ((c = is.get()) != 'u') throw bad_format("String value must be enclosed inside double quotation marks.");
		if ((c = is.get()) != 'l') throw bad_format("String value must be enclosed inside double quotation marks.");
		if ((c = is.get()) != 'l') throw bad_format("Unrecognized syntax (did you mean 'null'?).");
		node.type = typeNULL;
	}
	else if (c == '[')
	{
		is.get();
		node.type = ARRAY;
		node.arr_size = 0;

	}
	else if (c == '{')
	{
		is.get();
		node.type = JSON;
	}
	else
	{
		is.peek();//prevents column index decrementation when printing errors
		throw bad_format("String value must be enclosed inside double quotation marks.");
	}


	return is;


}

ostream& operator << (ostream& os, Node& node)
{
	node_type temp_type = node.get_type();
	if (temp_type == STRING) os << "\"" << *(node.str_value) << "\"";
	else if (temp_type == NUMBER) os << *(node.str_value);
	else if (temp_type == BOOLEAN)
	{
		if (node.bool_value) os << "true";
		else os << "false";
	}
	else if (temp_type == typeNULL) os << "null";
	else if (temp_type == JSON)
	{
		os << "{";
		Node* iterator = node.children;
		while (iterator != nullptr)
		{
			os << "\"" << *(iterator->name) << "\"" << ":" << *iterator;
			if (iterator->sibling != nullptr) os << ",";
			iterator = iterator->sibling;
		}
		os << "}";
	}
	else if (temp_type == ARRAY)
	{
		os << "[";
		Node* iterator = node.children;
		while (iterator != nullptr)
		{
			os <<  *iterator;
			if (iterator->sibling != nullptr) os << ",";
			iterator = iterator->sibling;
		}
		os << "]";
	}
	return os;




}