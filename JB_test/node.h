#ifndef NODE_H
#define NODE_H
#include <string>
#include <iostream>
#include "custom_input_stream.h"
using namespace std;
enum node_type
{
	JSON,
	STRING,
	NUMBER,
	BOOLEAN,
	ARRAY,
	typeNULL
};
class Node
{
private:

	string *name, *str_value;
	node_type type;
	float number_value;
	bool bool_value;
	int arr_size;
	Node* sibling;
	Node* children;
	void name_the_node(string& name);


public :
	
	Node();
	void set_type(node_type type);
	void set_number_value(float float_value);
	void set_bool_value(bool bool_value);
	void set_string_value(string& str_value);
	void set_name(string& str_value);
	float get_number_value();
	bool get_bool_value();
	string* get_string_value();
	string* get_name();
	node_type get_type();
	Node* get_child(string* s);
	Node* get_child(int index);
	int get_size();


	void add_child(Node*);


	friend custom_is& operator >> (custom_is&, Node&);
	friend ostream& operator << (ostream&, Node&);

};

custom_is& operator >> (custom_is&, Node&);
class bad_format :public exception
{
public:
	
	bad_format(const char* s) : exception(s)
	{

	}
};
class escaped_char :public exception
{
	char c;
public:
	escaped_char(char c) 
	{
		this->c = c;
	}
	char get_escaped_character()
	{
		return c;
	}
};
class unreachable_logic :public exception
{
public :
	unreachable_logic() : exception("This part of program shouldn't be executing.")
	{

	}
};
class control_character :public exception
{

};
class redundant_check_succeeded :public exception
{

};

#endif // !Node

