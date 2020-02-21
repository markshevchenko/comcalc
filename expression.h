#ifndef __EXPRESSION_H__
#define __EXPRESSION_H__

#include <string>

enum class expression_type
{
	Long = 1,
	Double,
};

std::string to_string(expression_type type) {
	if (type == expression_type::Long)
		return "long";

	if (type == expression_type::Double)
		return "double";

	throw new std::runtime_error("Unknown type.");
}

struct expression_node
{
private:
	expression_type _type;
	std::string _register_name;

public:	
	expression_node(expression_type type, const std::string& register_name) : _type(type), _register_name(register_name) { }

	expression_type type() const { return _type; }

	std::string register_name() const { return _register_name; }

	std::string to_string() const {
		if (type() == expression_type::Double)
			return "double " + register_name();

		return "i64 " + register_name();
	}
};

#endif