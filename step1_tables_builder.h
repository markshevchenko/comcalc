#ifndef __STEP1_TABLES_BUILDER_H__
#define __STEP1_TABLES_BUILDER_H__

#include <map>
#include <set>
#include <string>
#include <vector>

#include "ast.h"
#include "table_registry.h"

class function_signature
{
private:
	expression_type _result;
	std::vector<expression_type> _parameters;

public:
	expression_type result_type() const { return _result; }

	const std::vector<expression_type> parameters() const { return _parameters; }

	function_signature(expression_type result) {
		_result = result;
	}

	function_signature(expression_type parameter1, expression_type result) {
		_result = result;
		_parameters.push_back(parameter1);
	}

	function_signature(expression_type parameter1, expression_type parameter2, expression_type result) {
		_result = result;
		_parameters.push_back(parameter1);
		_parameters.push_back(parameter2);
	}
};

class step1_tables_builder : private visitor
{
public:
	table_registry build(const ast_program* program);

private:
	std::map<std::string, expression_type> _parameters;
	std::map<std::string, expression_type> _input_variables;
	std::map<std::string, expression_type> _output_variables;
	std::set<std::string> _standard_functions;
	std::vector<const ast_function*> _functions;
	std::vector<const ast_assignment*> _assignments;

	virtual void visit_function(const ast_function* function);

	virtual void visit_assignment(const ast_assignment* assignment);

	virtual void visit_variable(const ast_variable* variable);

	virtual void visit_call(const ast_call* call);
};

#endif