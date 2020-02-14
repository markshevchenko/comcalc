#ifndef __AST_H__
#define __AST_H__

#include <string>
#include <set>
#include <vector>

#include "expression.h"
#include "visitor.h"

class ast_node
{
public:
    virtual ~ast_node() { }

    virtual void accept(visitor&) const = 0;
};

class ast_expression : public ast_node
{
};

class ast_logical_expression : public ast_node
{
};

class ast_logical_binary_operator : public ast_logical_expression
{
private:
	std::string _operation;
	const ast_logical_expression* _left;
	const ast_logical_expression* _right;

public:
	ast_logical_binary_operator(const std::string& operation, const ast_logical_expression* left, const ast_logical_expression* right) {
		_operation = operation;
		_left = left;
		_right = right;
	}

	virtual ~ast_logical_binary_operator() {
		delete _left;
		delete _right;
	}

	virtual void accept(visitor& visitor) const {
		visitor.visit_logical_binary_operator(this);
	}

	const std::string& operation() const {
		return _operation;
	}

	const ast_logical_expression* left() const {
		return _left;
	}

	const ast_logical_expression* right() const {
		return _right;
	}
};

class ast_logical_not_operator : public ast_logical_expression
{
private:
	const ast_logical_expression* _operand;

public:
	ast_logical_not_operator(const ast_logical_expression* operand) {
		_operand = operand;
	}

	virtual ~ast_logical_not_operator() {
		delete _operand;
	}

	virtual void accept(visitor& visitor) const {
		visitor.visit_logical_not_operator(this);
	}

	const ast_logical_expression* operand() const {
		return _operand;
	}
};

class ast_condition : public ast_logical_expression
{
private:
	std::string _operation;
	const ast_expression* _left;
	const ast_expression* _right;

public:
	ast_condition(const std::string& operation, const ast_expression* left, const ast_expression* right) {
		_operation = operation;
		_left = left;
		_right = right;
	}

	virtual ~ast_condition() {
		delete _left;
		delete _right;
	}

	virtual void accept(visitor& visitor) const {
		visitor.visit_condition(this);
	}

	const std::string& operation() const {
		return _operation;
	}

	const ast_expression* left() const {
		return _left;
	}

	const ast_expression* right() const {
		return _right;
	}
};

class ast_binary_operator : public ast_expression
{
private:
    std::string _operation;
    const ast_expression* _left;
    const ast_expression* _right;

public:
    ast_binary_operator(const std::string& operation, const ast_expression* left, const ast_expression* right) {
        _operation = operation;
        _left = left;
        _right = right;
    }

    virtual ~ast_binary_operator() {
        delete _left;
        delete _right;
    }

    virtual void accept(visitor& visitor) const {
        visitor.visit_binary_operator(this);
    }

    const std::string& operation() const {
        return _operation;
    }

    const ast_expression* left() const {
        return _left;
    }

    const ast_expression* right() const {
        return _right;
    }
};

class ast_unary_operator : public ast_expression
{
private:
    std::string _operation;
    const ast_expression* _operand;

public:
    ast_unary_operator(const std::string& operation, const ast_expression* operand) {
        _operation = operation;
        _operand = operand;
    }

    virtual ~ast_unary_operator() {
        delete _operand;
    }

    virtual void accept(visitor& visitor) const {
        visitor.visit_unary_operator(this);
    }

    const std::string& operation() const {
        return _operation;
    }

    const ast_expression* operand() const {
        return _operand;
    }
};

class ast_long : public ast_expression
{
private:
    long _value;

public:
    ast_long(long value) {
        _value = value;
    }

    virtual void accept(visitor& visitor) const {
        visitor.visit_long(this);
    }

    long value() const {
        return _value;
    }
};

class ast_double : public ast_expression
{
private:
    double _value;

public:
    ast_double(double value) {
        _value = value;
    }

    virtual void accept(visitor& visitor) const {
        visitor.visit_double(this);
    }

    double value() const {
        return _value;
    }
};

class ast_variable : public ast_expression
{
private:
    std::string _name;
    bool _hasType;
    expression_type _type;

public:
    ast_variable(std::string name) {
        _name = name;
        _hasType = false;
        _type = (expression_type)0;
    }

    ast_variable(std::string name, expression_type type) {
        _name = name;
        _hasType = true;
        _type = type;
    }

    virtual void accept(visitor& visitor) const {
        visitor.visit_variable(this);
    }

    expression_type get_type() const {
        return _type;
    }

    const std::string &name() const {
        return _name;
    }
};

class ast_call : public ast_variable
{
private:
    std::vector<const ast_expression*> _parameters;

public:
    ast_call(std::string name, std::vector<const ast_expression*> parameters) : ast_variable(name) {
        _parameters = parameters;
    }

    ~ast_call() {
        for (std::vector<const ast_expression*>::iterator i = _parameters.begin(); i != _parameters.end(); i++)
            delete *i;
    }

    virtual void accept(visitor& visitor) const {
        visitor.visit_call(this);
    }

    const std::vector<const ast_expression*> &parameters() const {
        return _parameters;
    }
};

class ast_if_then_else : public ast_expression
{
private:
	const ast_logical_expression* _logical_expression;
	const ast_expression* _then_expression;
	const ast_expression* _else_expression;

public:
	ast_if_then_else(const ast_logical_expression* logical_expression, const ast_expression* then_expression, const ast_expression* else_expression)	{
		_logical_expression = logical_expression;
		_then_expression = then_expression;
		_else_expression = else_expression;
	}

	~ast_if_then_else() {
		delete _logical_expression;
		delete _then_expression;
		delete _else_expression;
	}

	virtual void accept(visitor& visitor) const {
		visitor.visit_if_then_else(this);
	}

	const ast_logical_expression* logical_expression() const {
		return _logical_expression;
	}

	const ast_expression* then_expression() const {
		return _then_expression;
	}

	const ast_expression* else_expression() const {
		return _else_expression;
	}
};

class ast_function : public ast_node
{
private:
    std::string _name;
    std::map<std::string, expression_type> _parameters;
    const ast_expression* _expression;

public:
    ast_function(const std::string& name, const std::map<std::string, expression_type>& parameters, const ast_expression* expression) {
        _name = name;
        _parameters = parameters;
        _expression = expression;
    }

    virtual ~ast_function() {
        delete _expression;
    }

    virtual void accept(visitor& visitor) const {
        visitor.visit_function(this);
    }

    const std::string &name() const {
        return _name;
    }

    const std::map<std::string, expression_type> &parameters() const {
        return _parameters;
    }

    const ast_expression* expression() const {
        return _expression;
    }
};

class ast_assignment : public ast_node
{
private:
    std::string _name;
    const ast_expression* _expression;

public:
    ast_assignment(const std::string& name, const ast_expression* expression) {
        _name = name;
        _expression = expression;
    }

    virtual ~ast_assignment() {
        delete _expression;
    }

    virtual void accept(visitor& visitor) const {
        visitor.visit_assignment(this);
    }

    const std::string& name() const {
        return _name;
    }

    const ast_expression* expression() const {
        return _expression;
    }
};

class ast_program : public ast_node
{
private:
    std::vector<const ast_function*> _functions;
    std::vector<const ast_assignment*> _assignments;

public:
    ast_program(const std::vector<const ast_function*>& functions, const std::vector<const ast_assignment*>& assignments) {
        _functions = functions;
        _assignments = assignments;
    }

    virtual ~ast_program() {
        for (std::vector<const ast_function*>::iterator i = _functions.begin(); i != _functions.end(); i++)
            delete *i;

        for (std::vector<const ast_assignment*>::iterator i = _assignments.begin(); i != _assignments.end(); i++)
            delete *i;
    }

    virtual void accept(visitor& visitor) const {
        visitor.visit_program(this);
    }

    const std::vector<const ast_function*>& functions() const {
        return _functions;
    }

    const std::vector<const ast_assignment*>& assignments() const {
        return _assignments;
    }
};

#endif