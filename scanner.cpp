#include <cctype>
#include <stdexcept>
#include <string>

#include "scanner.h"

lexeme scanner::read_lexeme() {
    _token.clear();

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

        if(_token == "if")
            return lexeme::If;
        
        if(_token == "then")
            return lexeme::Then;
        
        if(_token == "else")
            return lexeme::Else;

        if(_token == "or")
            return lexeme::Or;

        if(_token == "and")
            return lexeme::And;

        if (_token == "not")
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

    throw new std::runtime_error("Unknown token '" + _token + "'.");
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
        _token += _input.get();

        return true;
    }

    return false;
}

bool scanner::take(int (*function)(int)) {
    if(function(_input.peek())) {
        _token += _input.get();

        return true;
    }

    return false;
}
