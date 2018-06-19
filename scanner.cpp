#include <cctype>
#include <stdexcept>
#include <string>

#include "scanner.h"

lexem scanner::read_lexem()
{
    _token.clear();

    while(skip(' '))
        ;

    if(skip('\n')) {
        while(skip('\n'))
            ;

        return lexem::NewLine;
    }

    if(take('('))
        return lexem::LParen;

    if(take(')'))
        return lexem::RParen;
  
    if(take(','))
        return lexem::Comma;

    if(take('+'))
        return lexem::Plus;

    if(take('-'))
        return lexem::Minus;

    if(take('*'))
        return lexem::Star;

    if(take('/'))
        return lexem::Slash;

    if(take('%'))
        return lexem::Percent;

    if(take('^'))
        return lexem::Caret;

    if(take('='))
        return lexem::Eq;

    if(take('>')) {
        if(take('='))
            return lexem::Ge;

        return lexem::Gt;
    }

    if(take('<')) {
        if(take('='))
            return lexem::Le;
            
        if(take('>'))
            return lexem::Ne;

        return lexem::Lt;
    }

    if(take(std::isalpha)) {
        while(take(std::isalnum))
            ;

        if(_token == "if")
            return lexem::If;
        
        if(_token == "then")
            return lexem::Then;
        
        if(_token == "else")
            return lexem::Else;

        if(_token == "or")
            return lexem::Or;

        if(_token == "and")
            return lexem::And;

        if (_token == "not")
            return lexem::Not;

        return lexem::Identifier;
    }

    if(take(std::isdigit)) {
        while(take(std::isdigit))
            ;

        if(take('.')) {
            while(take(std::isdigit))
                ;

            return lexem::Double;            
        }

        return lexem::Integer;
    }

    if(_input.peek() == std::char_traits<char>::eof())
        return lexem::Eof;

    throw new std::runtime_error("Unknown token '" + _token + "'.");
}

bool scanner::skip(char c)
{
    if(_input.peek() == c) {
        _input.get();

        return true;
    }

    return false;
}

bool scanner::take(char c)
{
    if(_input.peek() == c) {
        _token += _input.get();

        return true;
    }

    return false;
}

bool scanner::take(int (*function)(int))
{
    if(function(_input.peek())) {
        _token += _input.get();

        return true;
    }

    return false;
}
