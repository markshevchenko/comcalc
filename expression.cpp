#include "expression.h"

#include <string>

static std::string integer_first_letters = "ijklmnIJKLMN";

expression_type get_variable_type(const std::string& variable_name) {
	auto first_variable_letter = variable_name[0];

	if (integer_first_letters.find(first_variable_letter) == std::string::npos)
		return DOUBLE;

	return I64;
}

std::string get_node_as_text(expression_node node) {
	if (node.type() == DOUBLE)
		return "double " + node.register_name();

	return "i64 " + node.register_name();
}
