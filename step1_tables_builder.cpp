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
	visitor::visit_assignment(assignment);
	auto name = assignment->name();

	bool is_identifier_already_declared = _output_variables.find(name) != _output_variables.end();
	if (is_identifier_already_declared)
		throw new std::runtime_error("Variable `" + name + "` already declared.");

	auto type = assignment->expression()->type();

	_output_variables[name] = type;

	_assignments.push_back(assignment);
}

void step1_tables_builder::visit_variable(const ast_variable* variable) {
	_input_variables[variable->name()] = variable->type();;

	visitor::visit_variable(variable);
}

static std::map<std::string, function_signature> standard_functions =
{
	{ "acos", function_signature(expression_type::Double, expression_type::Double) },
	{ "asin", function_signature(expression_type::Double, expression_type::Double) },
	{ "atan", function_signature(expression_type::Double, expression_type::Double) },
	{ "atan2", function_signature(expression_type::Double, expression_type::Double, expression_type::Double) },
	{ "cos", function_signature(expression_type::Double, expression_type::Double) },
	{ "exp", function_signature(expression_type::Double, expression_type::Double) },
	{ "fabs", function_signature(expression_type::Double, expression_type::Double) },
	{ "log", function_signature(expression_type::Double, expression_type::Double) },
	{ "log10", function_signature(expression_type::Double, expression_type::Double) },
	{ "sin", function_signature(expression_type::Double, expression_type::Double) },
	{ "sqrt", function_signature(expression_type::Double, expression_type::Double) },
	{ "tan", function_signature(expression_type::Double, expression_type::Double) },
};

void step1_tables_builder::visit_call(const ast_call* call) {
	auto function_name = call->name();
	bool is_standard_function = standard_functions.find(call->name()) != standard_functions.end();
	
	if (is_standard_function) {
		_standard_functions.insert(call->name());
	}

	visitor::visit_call(call);
}
