#ifndef __PREGENERATOR_H__
#define __PREGENERATOR_H__

#include "ast.h"

#include <set>
#include <string>
#include <vector>

class pregenerator : public iterating_visitor
{
private:
	static std::set<std::string> predefined_functions;

	std::set<std::string> _parameters;
	std::set<std::string> _input_variables;
	std::set<std::string> _output_variables;
	std::set<std::string> _standard_functions;
	std::vector<const ast_function*> _functions;
	std::vector<const ast_assignment*> _assignments;

public:
	const std::set<std::string> &input_variables() const { return _input_variables; }

	const std::set<std::string> &output_variables() const { return _output_variables; }

	const std::set<std::string> &standard_functions() const { return _standard_functions; }

	const std::vector<const ast_function*> &functions() const { return _functions; }

	const std::vector<const ast_assignment*> &assignments() const { return _assignments; }

	virtual void visit_function(const ast_function* function) {
		_parameters.clear();
		auto first_parameter = function->parameters().cbegin();
		auto last_parameter = function->parameters().cend();
		_parameters.insert(first_parameter, last_parameter);

		iterating_visitor::visit_function(function);

		_functions.push_back(function);
	}

	virtual void visit_assignment(const ast_assignment* assignment) {
		_parameters.clear();
		auto declared_identifier = assignment->name();

		if (_output_variables.find(declared_identifier) != _output_variables.end())
			throw new std::runtime_error("Variable `" + declared_identifier + "` already declared.");

		_output_variables.insert(declared_identifier);

		iterating_visitor::visit_assignment(assignment);

		_assignments.push_back(assignment);
	}

	virtual void visit_variable(const ast_variable* variable) {
		_input_variables.insert(variable->name());

		iterating_visitor::visit_variable(variable);
	}

	virtual void visit_call(const ast_call* call) {
		if (predefined_functions.find(call->name()) != predefined_functions.end()
			&& _standard_functions.find(call->name()) == _standard_functions.end()) {
			_standard_functions.insert(call->name());
		}

		iterating_visitor::visit_call(call);
	}
};

#endif
