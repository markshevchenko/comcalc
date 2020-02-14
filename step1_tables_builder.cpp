#include <stdexcept>

#include "step1_tables_builder.h"

table_registry step1_tables_builder::build(const ast_program* program) {
	_input_variables.clear();
	_output_variables.clear();
	_standard_functions.clear();
	_functions.clear();
	_assignments.clear();

	program->accept(*this);

	return table_registry(_input_variables, _output_variables, _standard_functions, _functions, _assignments);
}

void step1_tables_builder::visit_function(const ast_function* function) {
	_parameters.clear();
	auto first_parameter = function->parameters().cbegin();
	auto last_parameter = function->parameters().cend();
	_parameters.insert(first_parameter, last_parameter);

	visitor::visit_function(function);

	_functions.push_back(function);
}

void step1_tables_builder::visit_assignment(const ast_assignment* assignment) {
	_parameters.clear();
	auto declared_identifier = assignment->name();

	bool is_identifier_already_declared = _output_variables.find(declared_identifier) != _output_variables.end();
	if (is_identifier_already_declared)
		throw new std::runtime_error("Variable `" + declared_identifier + "` already declared.");

	_output_variables.insert(declared_identifier);

	visitor::visit_assignment(assignment);

	_assignments.push_back(assignment);
}

void step1_tables_builder::visit_variable(const ast_variable* variable) {
	_input_variables.insert(variable->name());

	visitor::visit_variable(variable);
}

static std::map<std::string, function_signature> standard_functions =
{
	{ "acos", function_signature(expression_type::DOUBLE, expression_type::DOUBLE) },
	{ "asin", function_signature(expression_type::DOUBLE, expression_type::DOUBLE) },
	{ "atan", function_signature(expression_type::DOUBLE, expression_type::DOUBLE) },
	{ "atan2", function_signature(expression_type::DOUBLE, expression_type::DOUBLE, expression_type::DOUBLE) },
	{ "cos", function_signature(expression_type::DOUBLE, expression_type::DOUBLE) },
	{ "exp", function_signature(expression_type::DOUBLE, expression_type::DOUBLE) },
	{ "fabs", function_signature(expression_type::DOUBLE, expression_type::DOUBLE) },
	{ "log", function_signature(expression_type::DOUBLE, expression_type::DOUBLE) },
	{ "log10", function_signature(expression_type::DOUBLE, expression_type::DOUBLE) },
	{ "sin", function_signature(expression_type::DOUBLE, expression_type::DOUBLE) },
	{ "sqrt", function_signature(expression_type::DOUBLE, expression_type::DOUBLE) },
	{ "tan", function_signature(expression_type::DOUBLE, expression_type::DOUBLE) },
};

void step1_tables_builder::visit_call(const ast_call* call) {
	auto function_name = call->name();
	bool is_standard_function = standard_functions.find(call->name()) != standard_functions.end();
	
	if (is_standard_function) {
		_standard_functions.insert(call->name());
	}

	visitor::visit_call(call);
}
