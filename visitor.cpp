#include "ast.h"

visitor::~visitor() { }

void visitor::visit_program(const ast_program* program) {
	for (auto i = program->functions().cbegin(); i != program->functions().cend(); i++)
		(*i)->accept(*this);

	for (auto i = program->assignments().cbegin(); i != program->assignments().cend(); i++)
		(*i)->accept(*this);
}

void visitor::visit_function(const ast_function* function) {
	function->expression()->accept(*this);
}

void visitor::visit_assignment(const ast_assignment* assignment) {
	assignment->expression()->accept(*this);
}

void visitor::visit_binary_operator(const ast_binary_operator* binary_operator) {
	binary_operator->left()->accept(*this);
	binary_operator->right()->accept(*this);
}

void visitor::visit_unary_operator(const ast_unary_operator* unary_operator) {
	unary_operator->operand()->accept(*this);
}

void visitor::visit_integer(const ast_integer*) { }

void visitor::visit_double(const ast_double*) { }

void visitor::visit_variable(const ast_variable*) { }

void visitor::visit_call(const ast_call* call) {
	for (auto i = call->parameters().cbegin(); i != call->parameters().cend(); i++)
		(*i)->accept(*this);
}

void visitor::visit_logical_binary_operator(const ast_logical_binary_operator* logical_binary_operator) {
	logical_binary_operator->left()->accept(*this);
	logical_binary_operator->right()->accept(*this);
}

void visitor::visit_logical_not_operator(const ast_logical_not_operator* logical_not_operator) {
	logical_not_operator->operand()->accept(*this);
}

void visitor::visit_condition(const ast_condition* condition) {
	condition->left()->accept(*this);
	condition->right()->accept(*this);
}

void visitor::visit_if_then_else(const ast_if_then_else* if_then_else) {
	if_then_else->logical_expression()->accept(*this);
	if_then_else->then_expression()->accept(*this);
	if_then_else->else_expression()->accept(*this);
}

