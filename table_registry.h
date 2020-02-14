#ifndef __TABLE_REGISTRY_H__
#define __TABLE_REGISTRY_H__

#include <map>
#include <set>
#include <string>
#include <vector>

#include "ast.h"

class table_registry
{
private:
	std::map<std::string, expression_type> _input_variables;
	std::map<std::string, expression_type> _output_variables;
	std::set<std::string> _standard_functions;
	std::vector<const ast_function*> _functions;
	std::vector<const ast_assignment*> _assignments;

public:
	const std::map<std::string, expression_type>& input_variables() const { return _input_variables; }

	const std::map<std::string, expression_type>& output_variables() const { return _output_variables; }

	const std::set<std::string>& standard_functions() const { return _standard_functions; }

	const std::vector<const ast_function*>& functions() const { return _functions; }

	const std::vector<const ast_assignment*>& assignments() const { return _assignments; }

	table_registry(
		std::map<std::string, expression_type> input_variables,
		std::map<std::string, expression_type> output_variables,
		std::set<std::string> standard_functions,
		std::vector<const ast_function*> functions,
		std::vector<const ast_assignment*> assignments)
	{
		_input_variables = input_variables;
		_output_variables = output_variables;
		_standard_functions = standard_functions;
		_functions = functions;
		_assignments = assignments;
	}
};

#endif
