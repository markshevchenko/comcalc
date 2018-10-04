#ifndef __SCANNER_H__
#define __SCANNER_H__

#include <istream>
#include <string>

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
    lexeme _lexeme;
    std::string _token;

    std::istream &_input;

public:
    scanner(std::istream& input): _input(input) {

        next();
    }

    ::lexeme lexeme() const {
        return _lexeme;
    }

    static std::string lexeme_str(::lexeme lexem) {
        switch(lexem) {
            case lexeme::NewLine:
                return "'\\n'";
                
            case lexeme::LParen:
                return "'('";
                
            case lexeme::RParen:
                return "')'";
                
            case lexeme::Eq:
                return "'='";
                
            case lexeme::Comma:
                return "','";
                
            case lexeme::Plus:
                return "'+'";
                
            case lexeme::Minus:
                return "'-'";
                
            case lexeme::Star:
                return "'*'";
                
            case lexeme::Slash:
                return "'/'";
                
            case lexeme::Percent:
                return "'%'";
                
            case lexeme::Caret:
                return "'^'";
                
            case lexeme::If:
                return "'if'";
                
            case lexeme::Then:
                return "'then'";
                
            case lexeme::Else:
                return "'else'";
                
            case lexeme::Or:
                return "'or'";
                
            case lexeme::And:
                return "'and'";
                
            case lexeme::Not:
                return "'not'";
                
            case lexeme::Lt:
                return "'<='";
                
            case lexeme::Gt:
                return "'>='";
                
            case lexeme::Le:
                return "'<'";
                
            case lexeme::Ge:
                return "'>'";
                
            case lexeme::Ne:
                return "'<>'";
                
            case lexeme::Identifier:
                return "identifier";
                
            case lexeme::Integer:
                return "integer";
                
            case lexeme::Double:
                return "double";
                
            case lexeme::Eof:
                return "end of file";
                
            default:
                return "";
        }
    }

    const std::string &token() const {
        return _token;
    }

    void next() {
        _lexeme = read_lexeme();
    }

protected:
    ::lexeme read_lexeme();

    bool skip(char c);

    bool take(char c);

    bool take(int (*function)(int));
};

#endif