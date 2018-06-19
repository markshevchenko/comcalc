#include "printer.h"

void print(const ast_program* program, std::ostream& out)
{
	print_visitor visitor(out);

	program->accept(visitor);
}

void print_visitor::visit_program(const ast_program* program)
{
	for (size_t i = 0; i < program->functions().size(); i++)
		program->functions()[i]->accept(*this);

	for (size_t i = 0; i < program->assignments().size(); i++)
		program->assignments()[i]->accept(*this);
}

void print_visitor::visit_function(const ast_function* function)
{
	_printer.print("function ");
	_printer.printline(function->name());

	_printer.indent();

	for (size_t i = 0; i < function->parameters().size(); i++) {
		_printer.print("parameter ");
		_printer.printline(function->parameters()[i]);
	}

	function->expression()->accept(*this);

	_printer.unindent();
}

void print_visitor::visit_assignment(const ast_assignment* assignment)
{
	_printer.print("assignment ");
	_printer.printline(assignment->name());

	_printer.indent();

	assignment->expression()->accept(*this);

	_printer.unindent();
}

void print_visitor::visit_binary_operator(const ast_binary_operator* binary_operator)
{
	_printer.print("binary operator ");
	_printer.printline(binary_operator->operation());

	_printer.indent();

	binary_operator->left()->accept(*this);
	binary_operator->right()->accept(*this);

	_printer.unindent();
}

void print_visitor::visit_unary_operator(const ast_unary_operator* unary_operator)
{
	_printer.print("unary operator ");
	_printer.printline(unary_operator->operation());

	_printer.indent();

	unary_operator->operand()->accept(*this);

	_printer.unindent();
}

void print_visitor::visit_integer(const ast_integer* i)
{
	_printer.print("integer ");
	_printer.printline(std::to_string(i->value()));
}

void print_visitor::visit_double(const ast_double* d)
{
	_printer.print("double ");
	_printer.printline(std::to_string(d->value()));
}

void print_visitor::visit_variable(const ast_variable* variable)
{
	_printer.print("variable ");
	_printer.printline(variable->name());
}

void print_visitor::visit_call(const ast_call* call)
{
	_printer.print("call ");
	_printer.printline(call->name());

	_printer.indent();

	for (size_t i = 0; i < call->parameters().size(); i++)
		call->parameters()[i]->accept(*this);

	_printer.unindent();
}

void print_visitor::visit_logical_binary_operator(const ast_logical_binary_operator* logical_binary_operator)
{
	_printer.print("logical binary operator ");
	_printer.printline(logical_binary_operator->operation());

	_printer.indent();

	logical_binary_operator->left()->accept(*this);
	logical_binary_operator->right()->accept(*this);

	_printer.unindent();
}

void print_visitor::visit_logical_not_operator(const ast_logical_not_operator* logical_not_operator)
{
	_printer.print("logical not operator ");

	_printer.indent();

	logical_not_operator->operand()->accept(*this);

	_printer.unindent();
}

void print_visitor::visit_condition(const ast_condition* condition)
{
	_printer.print("condition ");
	_printer.printline(condition->operation());

	_printer.indent();

	condition->left()->accept(*this);
	condition->right()->accept(*this);

	_printer.unindent();
}

void print_visitor::visit_if_then_else(const ast_if_then_else* if_then_else)
{
	_printer.printline("if then else");

	_printer.indent();

	if_then_else->logical_expression()->accept(*this);
	if_then_else->then_expression()->accept(*this);
	if_then_else->else_expression()->accept(*this);

	_printer.unindent();
}
