#ifndef __SCANNER_H__
#define __SCANNER_H__

#include <istream>
#include <string>

enum lexem
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
    lexem _lexem;
    std::string _token;

    std::istream &_input;

public:
    scanner(std::istream& input): _input(input) {

        next();
    }

    ::lexem lexem() const {
        return _lexem;
    }

    static std::string lexem_str(::lexem lexem) {
        switch(lexem) {
            case lexem::NewLine:
                return "'\\n'";
                
            case lexem::LParen:
                return "'('";
                
            case lexem::RParen:
                return "')'";
                
            case lexem::Eq:
                return "'='";
                
            case lexem::Comma:
                return "','";
                
            case lexem::Plus:
                return "'+'";
                
            case lexem::Minus:
                return "'-'";
                
            case lexem::Star:
                return "'*'";
                
            case lexem::Slash:
                return "'/'";
                
            case lexem::Percent:
                return "'%'";
                
            case lexem::Caret:
                return "'^'";
                
            case lexem::If:
                return "'if'";
                
            case lexem::Then:
                return "'then'";
                
            case lexem::Else:
                return "'else'";
                
            case lexem::Or:
                return "'or'";
                
            case lexem::And:
                return "'and'";
                
            case lexem::Not:
                return "'not'";
                
            case lexem::Lt:
                return "'<='";
                
            case lexem::Gt:
                return "'>='";
                
            case lexem::Le:
                return "'<'";
                
            case lexem::Ge:
                return "'>'";
                
            case lexem::Ne:
                return "'<>'";
                
            case lexem::Identifier:
                return "identifier";
                
            case lexem::Integer:
                return "integer";
                
            case lexem::Double:
                return "double";
                
            case lexem::Eof:
                return "end of file";
                
            default:
                return "";
        }
    }

    const std::string &token() const {
        return _token;
    }

    void next() {
        _lexem = read_lexem();
    }

protected:
    ::lexem read_lexem();

    bool skip(char c);

    bool take(char c);

    bool take(int (*function)(int));
};

#endif