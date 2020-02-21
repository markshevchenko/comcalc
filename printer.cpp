#include "printer.h"

class printer
{
private:
	std::ostream& _out;
	unsigned _indent;
	bool is_begin_of_line;

protected:
	void print_indent() {
		if (is_begin_of_line) {
			_out << std::string(2 * _indent, ' ');

			is_begin_of_line = false;
		}
	}

public:
	printer(std::ostream& out) : _out(out), _indent(0), is_begin_of_line(true) { }

	void indent() {
		_indent++;
	}

	void unindent() {
		_indent--;
	}

	void print(const std::string& s) {
		print_indent();

		_out << s;
	}

	void printline(const std::string& s) {
		print_indent();

		_out << s << std::endl;

		is_begin_of_line = true;
	}
};

class print_visitor : public visitor
{
private:
	printer _printer;

public:
	print_visitor(std::ostream& out) : _printer(out) { }

	virtual void visit_function(const ast_function* function) {
		_printer.print("function ");
		_printer.printline(function->name());

		_printer.indent();

		for (auto i = function->parameters().cbegin(); i != function->parameters().cend(); i++) {
			_printer.print("parameter ");
			_printer.print(i->first);
			_printer.print(" ");
			_printer.printline(to_string(i->second));
		}

		visitor::visit_function(function);

		_printer.unindent();
	}

	virtual void visit_assignment(const ast_assignment* assignment) {
		_printer.print("assignment ");
		_printer.printline(assignment->name());

		_printer.indent();

		visitor::visit_assignment(assignment);

		_printer.unindent();
	}

	virtual void visit_binary_operator(const ast_binary_operator* binary_operator) {
		_printer.print("binary operator ");
		_printer.printline(to_string(binary_operator->operation()));

		_printer.indent();

		visitor::visit_binary_operator(binary_operator);

		_printer.unindent();
	}

	virtual void visit_unary_operator(const ast_unary_operator* unary_operator) {
		_printer.print("unary operator ");
		_printer.printline(to_string(unary_operator->operation()));

		_printer.indent();

		visitor::visit_unary_operator(unary_operator);

		_printer.unindent();
	}

	virtual void visit_integer(const ast_long* i) {
		_printer.print("integer ");
		_printer.printline(std::to_string(i->value()));
	}

	virtual void visit_double(const ast_double* d) {
		_printer.print("double ");
		_printer.printline(std::to_string(d->value()));
	}

	virtual void visit_variable(const ast_variable* variable) {
		_printer.print("variable ");
		_printer.printline(variable->name());
	}

	virtual void visit_call(const ast_call* call) {
		_printer.print("call ");
		_printer.printline(call->name());

		_printer.indent();

		visitor::visit_call(call);

		_printer.unindent();
	}

	virtual void visit_logical_binary_operator(const ast_logical_binary_operator* logical_binary_operator) {
		_printer.print("logical binary operator ");
		_printer.printline(logical_binary_operator->operation());

		_printer.indent();

		visitor::visit_logical_binary_operator(logical_binary_operator);

		_printer.unindent();
	}

	virtual void visit_logical_not_operator(const ast_logical_not_operator* logical_not_operator) {
		_printer.print("logical not operator ");

		_printer.indent();

		logical_not_operator->operand()->accept(*this);

		_printer.unindent();
	}

	virtual void visit_condition(const ast_condition* condition) {
		_printer.print("condition ");
		_printer.printline(condition->operation());

		_printer.indent();

		visitor::visit_condition(condition);

		_printer.unindent();
	}

	virtual void visit_if_then_else(const ast_if_then_else* if_then_else) {
		_printer.printline("if then else");

		_printer.indent();

		visitor::visit_if_then_else(if_then_else);

		_printer.unindent();
	}
};

void print(const ast_program* program, std::ostream& out) {
	print_visitor visitor(out);

	program->accept(visitor);
}
