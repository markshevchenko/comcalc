#ifndef __EXPRESSION_H__
#define __EXPRESSION_H__

enum expression_type
{
	I64,
	DOUBLE,
};

struct expression_node
{
private:
	expression_type _type;
	std::string _register_name;

public:	
	expression_node(expression_type type, const std::string& register_name) : _type(type), _register_name(register_name) { }

	expression_type type() const { return _type; }

	std::string register_name() const { return _register_name; }
};

std::string get_node_as_text(expression_node node);

expression_type get_variable_type(const std::string& variable_name);


#endif