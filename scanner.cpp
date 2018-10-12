#include <cctype>
#include <stdexcept>

#include "scanner.h"

std::map<lexeme, const char*> scanner::_names =
{
	{ lexeme::NewLine, "'\\n'" },
	{ lexeme::LParen, "'('" },
	{ lexeme::RParen, "')'" },
	{ lexeme::Eq, "'='" },
	{ lexeme::Comma, "','" },
	{ lexeme::Plus, "'+'" },
	{ lexeme::Minus, "'-'" },
	{ lexeme::Star, "'*'" },
	{ lexeme::Slash, "'/'" },
	{ lexeme::Percent, "'%'" },
	{ lexeme::Caret, "'^'" },
	{ lexeme::If, "'if'" },
	{ lexeme::Then, "'then'" },
	{ lexeme::Else, "'else'" },
	{ lexeme::Or, "'or'" },
	{ lexeme::And, "'and'" },
	{ lexeme::Not, "'not'" },
	{ lexeme::Lt, "'<'" },
	{ lexeme::Gt, "'>'" },
	{ lexeme::Le, "'<='" },
	{ lexeme::Ge, "'>='" },
	{ lexeme::Ne, "'<>'" },
	{ lexeme::Identifier, "identifier" },
	{ lexeme::Integer, "integer" },
	{ lexeme::Double, "double" },
	{ lexeme::Eof, "end of file" },
};

lexeme scanner::read_lexeme() {
    _buffer.clear();

    while(skip(' '))
        ;

    if(skip('\n')) {
        while(skip('\n'))
            ;

        return lexeme::NewLine;
    }

    if(take('('))
        return lexeme::LParen;

    if(take(')'))
        return lexeme::RParen;
  
    if(take(','))
        return lexeme::Comma;

    if(take('+'))
        return lexeme::Plus;

    if(take('-'))
        return lexeme::Minus;

    if(take('*'))
        return lexeme::Star;

    if(take('/'))
        return lexeme::Slash;

    if(take('%'))
        return lexeme::Percent;

    if(take('^'))
        return lexeme::Caret;

    if(take('='))
        return lexeme::Eq;

    if(take('>')) {
        if(take('='))
            return lexeme::Ge;

        return lexeme::Gt;
    }

    if(take('<')) {
        if(take('='))
            return lexeme::Le;
            
        if(take('>'))
            return lexeme::Ne;

        return lexeme::Lt;
    }

    if(take(std::isalpha)) {
        while(take(std::isalnum))
            ;

        if(_buffer == "if")
            return lexeme::If;
        
        if(_buffer == "then")
            return lexeme::Then;
        
        if(_buffer == "else")
            return lexeme::Else;

        if(_buffer == "or")
            return lexeme::Or;

        if(_buffer == "and")
            return lexeme::And;

        if (_buffer == "not")
            return lexeme::Not;

        return lexeme::Identifier;
    }

    if(take(std::isdigit)) {
        while(take(std::isdigit))
            ;

        if(take('.')) {
            while(take(std::isdigit))
                ;

            return lexeme::Double;            
        }

        return lexeme::Integer;
    }

    if(_input.peek() == std::char_traits<char>::eof())
        return lexeme::Eof;

    throw new std::runtime_error("Unknown token '" + _buffer + "'.");
}

bool scanner::skip(char c) {
    if(_input.peek() == c) {
        _input.get();

        return true;
    }

    return false;
}

bool scanner::take(char c) {
    if(_input.peek() == c) {
        _buffer += _input.get();

        return true;
    }

    return false;
}

bool scanner::take(int (*function)(int)) {
    if(function(_input.peek())) {
        _buffer += _input.get();

        return true;
    }

    return false;
}
