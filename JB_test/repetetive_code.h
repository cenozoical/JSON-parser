#define calculate \
int c = ss.peek();\
while (c == ' ' || c == '\t')\
{\
	ss.get();\
	c = ss.peek();\
}\
num_op temp_op = ADD;\
if ((c == '*') || (c == '/'))temp_op = MUL;\
if (c == ']' || c == ',' || c == EOF || c == '+' || c == '-' || c == '*' || c == '/')\
while (stack.back()->size() > 1 && op_vect.size() > 0)\
{\
	if (op_vect.back() < temp_op)break;\
	Node* op1, * op2;\
	op1 = stack.back()->back();\
	stack.back()->pop_back();\
	op2 = stack.back()->back();\
	stack.back()->pop_back();\
	if (op1->get_type() != NUMBER || op2->get_type() != NUMBER)\
	{\
char lr =0;\
if (op1->get_type() != NUMBER ) lr+=2;\
if (op2->get_type() != NUMBER )lr+=1;\
custom_istr_stream::column_index = op_pos_vect.back();\
		throw bad_operands(op_vect.back(), lr);\
	}\
	temp_node = new Node();\
	float value;\
	if (op_vect.back() == ADD)\
	{\
		value = op1->get_number_value() + op2->get_number_value();\
	}\
	else if (op_vect.back() == SUB)\
	{\
		value = op2->get_number_value() - op1->get_number_value();\
	}\
	else if (op_vect.back() == MUL)\
	{\
\
		value = op1->get_number_value() * op2->get_number_value();\
	}\
	else \
	{\
\
		if (op1->get_number_value() == 0) \
		{\
			custom_istr_stream::column_index = op_pos_vect.back(); \
			throw zero_division(); \
		}\
		value = op2->get_number_value() / op1->get_number_value();\
	}\
	temp_node->set_number_value(value);\
	temp_node->set_type(NUMBER);\
	string* temp_str = new string(to_string(value));\
	temp_node->set_string_value(*temp_str);\
	op_vect.pop_back();\
op_pos_vect.pop_back();\
	stack.back()->push_back(temp_node);\
			\
}\

