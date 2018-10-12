#ifndef __VISITOR_H__
#define __VISITOR_H__

class ast_program;
class ast_function;
class ast_assignment;
class ast_expression;
class ast_binary_operator;
class ast_unary_operator;
class ast_integer;
class ast_double;
class ast_variable;
class ast_call;
class ast_logical_expression;
class ast_logical_binary_operator;
class ast_logical_not_operator;
class ast_condition;
class ast_if_then_else;

class visitor
{
public:
	virtual ~visitor();

	virtual void visit_program(const ast_program* program);

	virtual void visit_function(const ast_function* function);

	virtual void visit_assignment(const ast_assignment* assignment);

	virtual void visit_binary_operator(const ast_binary_operator* binary_operator);

	virtual void visit_unary_operator(const ast_unary_operator* unary_operator);

	virtual void visit_integer(const ast_integer*);

	virtual void visit_double(const ast_double*);

	virtual void visit_variable(const ast_variable*);

	virtual void visit_call(const ast_call* call);

	virtual void visit_logical_binary_operator(const ast_logical_binary_operator* logical_binary_operator);

	virtual void visit_logical_not_operator(const ast_logical_not_operator* logical_not_operator);

	virtual void visit_condition(const ast_condition* condition);

	virtual void visit_if_then_else(const ast_if_then_else* if_then_else);
};

#endif

