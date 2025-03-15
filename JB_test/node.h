#ifndef NODE_H
#define NODE_H
#include <string>
#include <iostream>
#include "custom_input_stream.h"
using namespace std;
/*
Each node represents an element of the JSON file. It can be either a JSON file itself, JSON field or an array element.
In case, node is of type JSON or array, class fields "children" are used for representing its elements - essentially 
a linked list is used, where "children" is a pointer to the head of the list, while "sibling" can be used to 
traverse the list.
*/
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
	/*name is a JSON field name in case the node represents a JSON field
	str_value represents a string value in case node is of STRING type
	for example in file {"a":"abc"}, "a" is the field name, while "abc" is a string value
	*/
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


#endif // !Node

