#ifndef __PRINTER_H__
#define __PRINTER_H__

#include <ostream>

#include "ast.h"

void print(const ast_program* program, std::ostream& out);

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

	virtual void visit_program(const ast_program*);

	virtual void visit_function(const ast_function*);

	virtual void visit_assignment(const ast_assignment*);

	virtual void visit_binary_operator(const ast_binary_operator*);

	virtual void visit_unary_operator(const ast_unary_operator*);

	virtual void visit_integer(const ast_integer*);

	virtual void visit_double(const ast_double*);

	virtual void visit_variable(const ast_variable*);

	virtual void visit_call(const ast_call*);

	virtual void visit_logical_binary_operator(const ast_logical_binary_operator*);

	virtual void visit_logical_not_operator(const ast_logical_not_operator*);

	virtual void visit_condition(const ast_condition*);

	virtual void visit_if_then_else(const ast_if_then_else*);
};

#endif