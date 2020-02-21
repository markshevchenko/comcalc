#ifndef __STEP2_GENERATOR_H__
#define __STEP2_GENERATOR_H__

#include <map>
#include <ostream>
#include <set>
#include <stack>
#include <string>
#include <vector>

#include "ast.h"
#include "table_registry.h"

class step2_generator : private visitor
{
public:
	step2_generator(const table_registry& table_registry, std::ostream& out);

	void print_code();

private:
	std::map<std::string, expression_type> _input_only_static_variables;
	std::map<std::string, expression_type> _output_only_static_variables;
	std::map<std::string, expression_type> _all_static_variables;
	std::map<std::string, expression_type> _all_local_variables;
	std::set<std::string> _standard_functions;
	std::vector<const ast_function*> _functions;
	std::vector<const ast_assignment*> _assignments;
	std::ostream& _out;
	std::map<std::string, int> _named_variables;
	std::stack<expression_node> _expressions;
	int _last_variable_index = 0;

	void print_declarations();

	void print_input_formats();

	void print_output_formats();

	void print_main_header();

	void print_inputs();

	int get_next_variable_index();

	int get_named_variable_index(const std::string& variable_name);

	void set_named_variable_index(const std::string& variable_name, expression_node node);

	void print_assignments();

	void print_outputs();

	void print_main_footer();

	void print_external_functions();

	expression_node cast_to_double(expression_node node);

	virtual void visit_assignment(const ast_assignment* assignment);

	virtual void visit_long(const ast_long* _long);

	virtual void visit_double(const ast_double* _double);

	virtual void visit_variable(const ast_variable* variable);

	virtual void visit_call(const ast_call* call);

	virtual void visit_unary_operator(const ast_unary_operator* unary_operator);

	virtual void visit_binary_operator(const ast_binary_operator* binary_operator);
};

#endif