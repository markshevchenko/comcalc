#ifndef __SCANNER_H__
#define __SCANNER_H__

#include <istream>
#include <string>
#include <map>

enum lexeme
{
    NewLine,
    LParen,
    RParen,
    Eq,
    Comma,
    Plus,
    Minus,
    Star,
    Slash,
    Percent,
    Caret,
    If,
    Then,
    Else,
    Or,
    And,
    Not,
    Lt,
    Gt,
    Le,
    Ge,
    Ne,
    Identifier,
    Integer,
    Double,
    Eof,
};

class scanner
{
private:
	static std::map<::lexeme, const char*> _names;

    lexeme _lexeme;
    std::string _buffer;

    std::istream &_input;

public:
    scanner(std::istream& input): _input(input) {
        next();
    }

    ::lexeme lexeme() const {
        return _lexeme;
    }

    const std::string &buffer() const {
        return _buffer;
    }

    void next() {
        _lexeme = read_lexeme();
    }

	static std::string name(::lexeme lexeme) {
		return _names[lexeme];
	}

protected:
    ::lexeme read_lexeme();

    bool skip(char c);

    bool take(char c);

    bool take(int (*function)(int));
};

#endif