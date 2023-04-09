/****************LLVM****************************/
#include "llvm/ADT/APFloat.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;
using namespace llvm::sys;

/************************************************/

#include <iostream>

// 引入目的：将源代码读进input_string，即字符流.
#include <fstream>
#include <sstream>

// 引入目的：使用智能指针.
#include <memory>

// 引入目的：构建符号表.
#include <map>

//
#include <cstring>
#include <vector>


// 运算符、表达式、变量等的类型(其中，除Type_array外，皆是基础类型，即base-type, 或builtin-type).
typedef enum Ty {
    Ty_int,
    Ty_float,
    Ty_bool,
    Ty_void,
    Ty_char,
    Ty_array,
} Ty;

Ty type_returned_of_current_function;

// 字符流，是Lexer的输入. main()函数负责把源代码读入input_string.
static std::string input_string;

static std::string current_break_label;
static std::string current_continue_label;
static std::map<std::string, std::string> print_format_strings; //for assembly code

// 编译过程中，将错误信息打印出来
static void report_error(int line_no, int column_no, const std::string &msg) {
    std::stringstream input_string_stream(input_string);
    std::string line_str;

    for (long i = 0; i <= line_no; i++) std::getline(input_string_stream, line_str, '\n');

    fprintf(stderr, "error at %d:%d\n", line_no + 1, column_no + 1);
    fprintf(stderr, "%s\n", line_str.c_str());
    for (int i = 0; i < column_no; i++)
        fprintf(stderr, " ");
    fprintf(stderr, "^ %s\n", msg.c_str());
}

static void report_error(const std::string &msg) {
    fprintf(stderr, "\nerror: %s\n", msg.c_str());
}

//===----------------------------------------------------------------------===//
// Lexer
//===----------------------------------------------------------------------===//

// 用不同的整数值表示token的不同类型.
typedef enum TOKEN_TYPE {
    TK_INT_LITERAL,                  // [0-9]+
    TK_FLOAT_LITERAL,                // [0-9]+.[0-9]+
    TK_BOOL_LITERAL,                 // true or false
    TK_CHAR_LITERAL,                 // 'a'
    TK_IDENTIFIER,                   // identifier

    // keywords
    TK_INT,                          // int
    TK_FLOAT,                        // float
    TK_BOOL,                         // bool
    TK_VOID,                         // void
    TK_CHAR,                         // char
    TK_RETURN,                       // return
    TK_IF,                           // if
    TK_ELSE,                         // else
    TK_WHILE,                        // while
    TK_DO,                           // do
    TK_FOR,                          // for
    TK_BREAK,                        // break
    TK_CONTINUE,                     // continue
    TK_PRINT,                        // print

    TK_PLUS,                         // "+"
    TK_MINUS,                        // "-"
    TK_MULorDEREF,                   // "*"
    TK_DIV,                          // "/"
    TK_ADDRESS,                      // "&"
    TK_NOT,                          // "!"
    TK_LPAREN,                       // "("
    TK_RPAREN,                       // ")"
    TK_LBRACE,                       // "{"
    TK_RBRACE,                       // "}"
    TK_LBRACK,                       // "["
    TK_RBRACK,                       // "]"
    TK_ASSIGN,                       // "="
    TK_COMMA,                        // ","
    TK_SEMICOLON,                    // ";"

    // comparison operators
    TK_AND,                     // "&&"
    TK_OR,                      // "||"
    TK_EQ,                      // "=="
    TK_NE,                      // "!="
    TK_LT,                      // "<"
    TK_LE,                      // "<="
    TK_GT,                      // ">"
    TK_GE,                      // ">="

    // 人为引入的一个token，用以标识字符流（亦即源代码）的结尾.
    TK_EOF = -1,
} TOKEN_TYPE;

// TOKEN结构体.
struct TOKEN {
    TOKEN_TYPE type = TK_EOF;
    std::string lexeme;
    // 用于定位编译错误在源程序中的位置.
    int lineNo = -1;
    int columnNo = -1;
};


class Lexer {
private:
    // 字符流指针.
    long pos = 0;

    int lineNo = 1;
    int columnNo = 0;

    // 读取当前字符，并将指针指向下一个字符.
    char getChar() {
        columnNo++;
        return input_string[pos++];
    }

    // 回退一个字符.
    void put_backChar() {
        pos--;
        columnNo--;
    }

    // 获得下一个token.
    TOKEN getNextToken();

public:
    // token流，词法分析结果保存于此列表.
    std::vector<TOKEN> tokenList;

    // 构建整个token流.
    std::vector<TOKEN> constructTokenStream();
};

// getNextToken - 读取字符流(input_string)直至分析出一个token，并将此token返回.
TOKEN Lexer::getNextToken() {
    char CurrentChar = getChar();
    // 忽略空白.
    while (isspace(CurrentChar)) {
        if (CurrentChar == '\n') {
            lineNo++;
            columnNo = 1;
        }
        CurrentChar = getChar();
    }

    // 忽略注释.
    while (CurrentChar == '/') { // '/'可能是除运算符，也可能是行注释的开头
        if ((CurrentChar = getChar()) == '/') {       // 是行注释
            while ((CurrentChar = getChar()) != '\n' && input_string.length() >= pos)
                continue;
        } else if (CurrentChar == '*') {              // 是块注释
            if ((CurrentChar = getChar()) == '/') {
                report_error(lineNo, columnNo - 2, "missing '*' before '/'");
                exit(1);
            }
            while (CurrentChar != '/') {
                if (CurrentChar == '\n') {
                    lineNo++;
                    columnNo = 1;
                }
                CurrentChar = getChar();
            }
            if (CurrentChar == '/') {
                put_backChar();
                put_backChar();
                if ((CurrentChar = getChar()) != '*') {
                    report_error(lineNo, columnNo - 2, "missing '*' before '/'");
                    exit(1);
                } else {
                    getChar();
                    CurrentChar = getChar();
                }
            }
        } else {                                      // 是除运算，后面将做此解析
            put_backChar();
            put_backChar();
            CurrentChar = getChar();
            break;
        }

        // 忽略注释后的空白符.
        while (isspace(CurrentChar)) {
            if (CurrentChar == '\n' || CurrentChar == '\r') {
                lineNo++;
                columnNo = 1;
            }
            CurrentChar = getChar();
        }
    }

    // 整数或浮点数
    if (isdigit(CurrentChar) || CurrentChar == '.') { // 有可能是数值
        std::string NumberString;

        if (CurrentChar == '.') { // 浮点数: .[0-9]+
            do {
                NumberString += CurrentChar;
                CurrentChar = getChar();
            } while (isdigit(CurrentChar));
            put_backChar();
            return TOKEN{TK_FLOAT_LITERAL, NumberString, lineNo, columnNo};
        } else {
            do { // 数值的开头: [0-9]+
                NumberString += CurrentChar;
                CurrentChar = getChar();
            } while (isdigit(CurrentChar));

            if (CurrentChar == '.') { // 若出现小数点，是浮点数: [0-9]+.[0-9]+
                do {
                    NumberString += CurrentChar;
                    CurrentChar = getChar();
                } while (isdigit(CurrentChar));
                put_backChar();
                return TOKEN{TK_FLOAT_LITERAL, NumberString, lineNo, columnNo};
            } else {                  // 若未出现小数点，是整数: [0-9]+
                put_backChar();
                return TOKEN{TK_INT_LITERAL, NumberString, lineNo, columnNo};
            }
        }
    }

    // 标识符或关键字
    if (isalpha(CurrentChar) || CurrentChar == '_') { // 标识符以字母或_开头
        std::string IdentifierString;
        do {
            IdentifierString += CurrentChar;
            CurrentChar = getChar();
        } while (isalnum(CurrentChar) || CurrentChar == '_');
        put_backChar();

        // 1.关键字
        if (IdentifierString == "int")
            return TOKEN{TK_INT, IdentifierString, lineNo, columnNo};
        if (IdentifierString == "float")
            return TOKEN{TK_FLOAT, IdentifierString, lineNo, columnNo};
        if (IdentifierString == "bool")
            return TOKEN{TK_BOOL, IdentifierString, lineNo, columnNo};
        if (IdentifierString == "void")
            return TOKEN{TK_VOID, IdentifierString, lineNo, columnNo};
        if (IdentifierString == "char")
            return TOKEN{TK_CHAR, IdentifierString, lineNo, columnNo};
        if (IdentifierString == "true" || IdentifierString == "false")
            return TOKEN{TK_BOOL_LITERAL, IdentifierString, lineNo, columnNo};
        if (IdentifierString == "return")
            return TOKEN{TK_RETURN, IdentifierString, lineNo, columnNo};
        if (IdentifierString == "if" || IdentifierString == "if")
            return TOKEN{TK_IF, IdentifierString, lineNo, columnNo};
        if (IdentifierString == "else")
            return TOKEN{TK_ELSE, IdentifierString, lineNo, columnNo};
        if (IdentifierString == "while")
            return TOKEN{TK_WHILE, IdentifierString, lineNo, columnNo};
        if (IdentifierString == "do")
            return TOKEN{TK_DO, IdentifierString, lineNo, columnNo};
        if (IdentifierString == "for")
            return TOKEN{TK_FOR, IdentifierString, lineNo, columnNo};
        if (IdentifierString == "break")
            return TOKEN{TK_BREAK, IdentifierString, lineNo, columnNo};
        if (IdentifierString == "continue")
            return TOKEN{TK_CONTINUE, IdentifierString, lineNo, columnNo};
        if (IdentifierString == "print")
            return TOKEN{TK_PRINT, IdentifierString, lineNo, columnNo};

        // 2.普通标识符
        return TOKEN{TK_IDENTIFIER, IdentifierString, lineNo, columnNo};
    }

    // 运算符或分割符
    switch (CurrentChar) {
        case '+': {
            std::string s(1, CurrentChar);
            return TOKEN{TK_PLUS, s, lineNo, columnNo};
        }
        case '-': {
            std::string s(1, CurrentChar);
            return TOKEN{TK_MINUS, s, lineNo, columnNo};
        }
        case '*': {
            std::string s(1, CurrentChar);
            return TOKEN{TK_MULorDEREF, s, lineNo, columnNo};
        }
        case '/': {
            std::string s(1, CurrentChar);
            return TOKEN{TK_DIV, s, lineNo, columnNo};
        }
        case '(': {
            std::string s(1, CurrentChar);
            return TOKEN{TK_LPAREN, s, lineNo, columnNo};
        }
        case ')': {
            std::string s(1, CurrentChar);
            return TOKEN{TK_RPAREN, s, lineNo, columnNo};
        }
        case '{': {
            std::string s(1, CurrentChar);
            return TOKEN{TK_LBRACE, s, lineNo, columnNo};
        }
        case '}': {
            std::string s(1, CurrentChar);
            return TOKEN{TK_RBRACE, s, lineNo, columnNo};
        }
        case '[': {
            std::string s(1, CurrentChar);
            return TOKEN{TK_LBRACK, s, lineNo, columnNo};
        }
        case ']': {
            std::string s(1, CurrentChar);
            return TOKEN{TK_RBRACK, s, lineNo, columnNo};
        }
        case '&': {
            std::string s(1, CurrentChar);
            CurrentChar = getChar();
            if (CurrentChar == '&') {
                s += CurrentChar;
                return TOKEN{TK_AND, s, lineNo, columnNo};
            } else put_backChar();
            return TOKEN{TK_ADDRESS, s, lineNo, columnNo};
        }
        case '|': {
            std::string s(1, CurrentChar);
            CurrentChar = getChar();
            if (CurrentChar == '|') {
                s += CurrentChar;
                return TOKEN{TK_OR, s, lineNo, columnNo};
            } else put_backChar();
            break;
        }
        case '=': {
            std::string s(1, CurrentChar);
            CurrentChar = getChar();
            if (CurrentChar == '=') {
                s += CurrentChar;
                return TOKEN{TK_EQ, s, lineNo, columnNo};
            } else put_backChar();
            return TOKEN{TK_ASSIGN, s, lineNo, columnNo};
        }
        case '!': {
            std::string s(1, CurrentChar);
            CurrentChar = getChar();
            if (CurrentChar == '=') {
                s += CurrentChar;
                return TOKEN{TK_NE, s, lineNo, columnNo};
            } else put_backChar();
            return TOKEN{TK_NOT, s, lineNo, columnNo};
        }
        case '<': {
            std::string s(1, CurrentChar);
            CurrentChar = getChar();
            if (CurrentChar == '=') {
                s += CurrentChar;
                return TOKEN{TK_LE, s, lineNo, columnNo};
            } else put_backChar();
            return TOKEN{TK_LT, s, lineNo, columnNo};
        }
        case '>': {
            std::string s(1, CurrentChar);
            CurrentChar = getChar();
            if (CurrentChar == '=') {
                s += CurrentChar;
                return TOKEN{TK_GE, s, lineNo, columnNo};
            } else put_backChar();
            return TOKEN{TK_GT, s, lineNo, columnNo};
        }
        case ',': {
            std::string s(1, CurrentChar);
            return TOKEN{TK_COMMA, s, lineNo, columnNo};
        }
        case ';': {
            std::string s(1, CurrentChar);
            return TOKEN{TK_SEMICOLON, s, lineNo, columnNo};
        }
        case '\'': {// char character ref. https://github.com/rui314/chibicc/blob/aa0accc75e9358d313fef0a6d4005103e2ce25f5/tokenize.c
            if ((CurrentChar = getChar()) == '\0')
                report_error(lineNo, columnNo, "unclosed char literal");
            char c;
            if (CurrentChar == '\\') {
                CurrentChar = getChar();
                switch (CurrentChar) {
                    case 'a':
                        c = '\a';
                        break;
                    case 'b':
                        c = '\b';
                        break;
                    case 't':
                        c = '\t';
                        break;
                    case 'n':
                        c = '\n';
                        break;
                    case 'v':
                        c = '\v';
                        break;
                    case 'f':
                        c = '\f';
                        break;
                    case 'r':
                        c = '\r';
                        break;
                    default:
                        c = CurrentChar;
                }
            } else c = CurrentChar;

            if ((CurrentChar = getChar()) != '\'')
                report_error(lineNo, columnNo, "unclosed char literal");
            std::string s(1, c);
            return TOKEN{TK_CHAR_LITERAL, s, lineNo, columnNo};
        }
        default:
            break;
    }
    // 检查是否是字符流结尾：
    // 1.若是，则返回EOF.
    if (input_string.length() <= pos) {
        return TOKEN{TK_EOF, "EOF"};
    }
    // 2.若不是，则报错
    fprintf(stderr, "%c is an invalid token\n", CurrentChar);
    exit(1);
}

// 持续调用getNextToken()，获得整个token流，存于tokenList
std::vector<TOKEN> Lexer::constructTokenStream() {
    TOKEN currentToken;
    do {
        currentToken = getNextToken();
        tokenList.push_back(currentToken);
    } while (currentToken.type != TK_EOF);
    return std::move(tokenList);
}


//===----------------------------------------------------------------------===//
// Symbols, Symboltable
//===----------------------------------------------------------------------===//

class Symbol {
public:
    std::string name;
    Ty type; //类型：可以是int、float等基本类型，也可以是数组、结构体等结构化类型

    Symbol(std::string sym_name, Ty sym_type) :
            name(std::move(sym_name)), type(sym_type) {}

    // 为了使用std::dynamic_pointer_cast，此virtual析构函数不可少
    virtual ~Symbol() = default;
};

class Variable_Symbol : public Symbol {
public:
    int offset;  // 对于局部变量，包括数组，是相对于rbp的偏移量;对于全局变量，是在.data中相对于rip的偏移量.
    Ty baseType; // 基本类型：只能是int、float、char、bool等.
//    std::vector<std::string> elements; // 用于记录全局数组初始化数据.
    int scopeLevel;

    Variable_Symbol(std::string sym_name, Ty sym_type, Ty sym_base_type, int sym_offset, int level) :
            Symbol(std::move(sym_name), sym_type), baseType(sym_base_type), offset(sym_offset), scopeLevel(level) {}
};

class Function_Symbol : public Symbol {
public:
    Function_Symbol(std::string sym_name, Ty sym_type) :
            Symbol(std::move(sym_name), sym_type) {}
};

// 作用域 = ScopedSymbolTable
class ScopedSymbolTable {
public:
    std::string name;      // 作用域名称
    std::string category;  // 如：全局作用域、函数作用域、block作用域等

    // map是基于红黑树的数据结构，也可以用基于哈希表的unordered_map
    std::map<std::string, std::shared_ptr<Symbol>> symbols;
    int level;
    int child_no_count = 0;
    std::shared_ptr<ScopedSymbolTable> enclosing_scope;
    std::vector<std::shared_ptr<ScopedSymbolTable>> sub_scopes;

    ScopedSymbolTable(std::map<std::string, std::shared_ptr<Symbol>> syms,
                      int scope_level,
                      std::shared_ptr<ScopedSymbolTable> parent_scope
    ) : symbols(std::move(syms)),
        level(scope_level),
        enclosing_scope(std::move(parent_scope)) {}

    void insert(std::shared_ptr<Symbol> symbol) {
        symbols[symbol->name] = std::move(symbol);
    }

    std::shared_ptr<Symbol> lookup(std::string &sym_name, bool current_scope_only) {
        auto i = symbols.find(sym_name);
        if (i != symbols.end()) {
            // 注意：此处不能用std::move(i->second),否则首次找到该符号表项后，会引起删除
            return i->second;
        } else if (!current_scope_only && enclosing_scope != nullptr)
            return enclosing_scope->lookup(sym_name, false);
        else return nullptr;
    }
};

std::shared_ptr<ScopedSymbolTable> global_scope;

//===----------------------------------------------------------------------===//
// AST nodes and abstract visitor
//===----------------------------------------------------------------------===//

// Visitor类会引用各个AST结点类，故在此声明，其定义紧跟Visitor类定义之后
class NumLiteral_AST_Node;

class Assignment_AST_Node;

class Return_AST_Node;

class BinaryOperator_AST_Node;

class UnaryOperator_AST_Node;

class Variable_AST_Node;

class BaseType_AST_Node;

class Block_AST_Node;

class If_AST_Node;

class While_AST_Node;

class DoWhile_AST_Node;

class For_AST_Node;

class Break_AST_Node;

class Continue_AST_Node;

class Print_AST_Node;

class SingleVariableDeclaration_AST_Node;

class VariableDeclarations_AST_Node;

class Parameter_AST_Node;

class FunctionDeclaration_AST_Node;

class FunctionCall_AST_Node;

class Program_AST_Node;

// Visitor类定义. (Visitor类是SemanticVisitor和X86_CodeGenerator的基类，这两个派生类的定义在各个AST_Node派生类的定义之后)
class Visitor {
public:
    virtual Value *visit(NumLiteral_AST_Node &node) = 0;

    virtual Value *visit(Assignment_AST_Node &node) = 0;

    virtual Value *visit(Return_AST_Node &node) = 0;

    virtual Value *visit(BinaryOperator_AST_Node &node) = 0;

    virtual Value *visit(UnaryOperator_AST_Node &node) = 0;

    virtual Value *visit(Variable_AST_Node &node) = 0;

    virtual Value *visit(BaseType_AST_Node &node) = 0;

    virtual Value *visit(Block_AST_Node &node) = 0;

    virtual Value *visit(If_AST_Node &node) = 0;

    virtual Value *visit(While_AST_Node &node) = 0;

    virtual Value *visit(DoWhile_AST_Node &node) = 0;

    virtual Value *visit(For_AST_Node &node) = 0;

    virtual Value *visit(Break_AST_Node &node) = 0;

    virtual Value *visit(Continue_AST_Node &node) = 0;

    virtual Value *visit(Print_AST_Node &node) = 0;

    virtual Value *visit(SingleVariableDeclaration_AST_Node &node) = 0;

    virtual Value *visit(Parameter_AST_Node &node) = 0;

    virtual Value *visit(VariableDeclarations_AST_Node &node) = 0;

    virtual Value *visit(FunctionDeclaration_AST_Node &node) = 0;

    virtual Value *visit(FunctionCall_AST_Node &node) = 0;

    virtual Value *visit(Program_AST_Node &node) = 0;
};

/// AST_Node - 各种结点类***_AST_Node的基类.
class AST_Node {
public:
    /* 为某些结点(比如变量、表达式等)设置数据类型属性，方便各种Visitor访问时的处理.
       当然，这些属性信息也可以保存在符号表中，可依喜好选择. */
    // 类型：可以是int、float等基本类型，也可以是数组、结构体等结构化类型
    Ty type = Ty_void;
    // 基本类型：只能是int、float、char、bool等
    Ty baseType = Ty_void;

    virtual ~AST_Node() = default;

    virtual Value *accept(Visitor &v) = 0;
};

/// NumLiteral_AST_Node - 整数、浮点数、布尔常量的字面量
class NumLiteral_AST_Node : public AST_Node {
public:
    std::string literal;

    explicit NumLiteral_AST_Node(TOKEN &tok) : literal(tok.lexeme) {
        if (tok.type == TK_INT_LITERAL) type = Ty_int;
        else if (tok.type == TK_FLOAT_LITERAL) type = Ty_float;
        else if (tok.type == TK_BOOL_LITERAL) {
            type = Ty_bool;
            if (std::string(literal) == std::string("true")) literal = "1";
            else literal = "0";
        } else if (tok.type == TK_CHAR_LITERAL) type = Ty_char;
    }

    Value *accept(Visitor &v) override {
        return v.visit(*this);
    }
};

/// 赋值语句对应的结点
class Assignment_AST_Node : public AST_Node {
public:
    std::shared_ptr<AST_Node> left;
    std::string operation; // 此处必然是“=”
    std::shared_ptr<AST_Node> right;

    Assignment_AST_Node(std::shared_ptr<AST_Node> leftvalue, std::string op, std::shared_ptr<AST_Node> rightvalue)
            : left(std::move(leftvalue)), operation(std::move(op)), right(std::move(rightvalue)) {}

    Value *accept(Visitor &v) override {
        return v.visit(*this);
    }
};

/// return语句对应的结点
class Return_AST_Node : public AST_Node {
public:
    std::shared_ptr<AST_Node> expression; // 返回值表达式
    std::string functionName;             // 是哪个函数体里面的return？

    explicit Return_AST_Node(std::shared_ptr<AST_Node> expr) :
            expression(std::move(expr)) {}

    Value *accept(Visitor &v) override {
        return v.visit(*this);
    }
};

/// if语句对应的结点
class If_AST_Node : public AST_Node {
public:
    std::shared_ptr<AST_Node> condition;
    std::shared_ptr<AST_Node> then_statement;
    std::shared_ptr<AST_Node> else_statement;

    If_AST_Node(std::shared_ptr<AST_Node> con, std::shared_ptr<AST_Node> then_stmt, std::shared_ptr<AST_Node> else_stmt)
            : condition(std::move(con)), then_statement(std::move(then_stmt)), else_statement(std::move(else_stmt)) {}

    Value *accept(Visitor &v) override {
        return v.visit(*this);
    }
};

/// while语句对应的结点
class While_AST_Node : public AST_Node {
public:

    std::shared_ptr<AST_Node> condition;
    std::shared_ptr<AST_Node> statement;

    While_AST_Node(std::shared_ptr<AST_Node> con, std::shared_ptr<AST_Node> stmt)
            : condition(std::move(con)), statement(std::move(stmt)) {}

    Value *accept(Visitor &v) override {
        return v.visit(*this);
    }
};

/// do-while语句对应的结点
class DoWhile_AST_Node : public AST_Node {
public:
    std::shared_ptr<AST_Node> statement;
    std::shared_ptr<AST_Node> condition;

    DoWhile_AST_Node(std::shared_ptr<AST_Node> stmt, std::shared_ptr<AST_Node> con)
            : statement(std::move(stmt)), condition(std::move(con)) {}

    Value *accept(Visitor &v) override {
        return v.visit(*this);
    }
};

/// for语句对应的结点
class For_AST_Node : public AST_Node {
public:
    std::shared_ptr<AST_Node> initialization;  // for (expression? ;;)
    std::shared_ptr<AST_Node> condition;       // for (;expression;)
    std::shared_ptr<AST_Node> increment;       // for (;;expression)
    std::shared_ptr<AST_Node> statement;       // for () statement

    For_AST_Node(std::shared_ptr<AST_Node> init, std::shared_ptr<AST_Node> con, std::shared_ptr<AST_Node> inc,
                 std::shared_ptr<AST_Node> stmt)
            : initialization(std::move(init)), condition(std::move(con)), increment(std::move(inc)),
              statement(std::move(stmt)) {}

    Value *accept(Visitor &v) override {
        return v.visit(*this);
    }
};

/// break语句对应的结点
class Break_AST_Node : public AST_Node {
public:
    std::string break_label;

    Value *accept(Visitor &v) override {
        return v.visit(*this);
    }
};

/// continue语句对应的结点
class Continue_AST_Node : public AST_Node {
public:
    std::string continue_label;

    Value *accept(Visitor &v) override {
        return v.visit(*this);
    }
};

/// print语句对应的结点
class Print_AST_Node : public AST_Node {
public:
    std::shared_ptr<AST_Node> value;

    explicit Print_AST_Node(std::shared_ptr<AST_Node> v) : value(std::move(v)) {}

    Value *accept(Visitor &v) override {
        return v.visit(*this);
    }
};

/// 二元运算对应的结点（注意，实际上，此即expression对应的node）
class BinaryOperator_AST_Node : public AST_Node {
public:
    std::shared_ptr<AST_Node> left;
    TOKEN_TYPE op;
    std::shared_ptr<AST_Node> right;

    BinaryOperator_AST_Node(std::shared_ptr<AST_Node> Left, TOKEN_TYPE op, std::shared_ptr<AST_Node> Right)
            : left(std::move(Left)), op(op), right(std::move(Right)) {}

    Value *accept(Visitor &v) override {
        return v.visit(*this);
    }
};

/// 单个变量声明对应的结点
class SingleVariableDeclaration_AST_Node : public AST_Node {
public:
    // 成员变量与Parameter_AST_Node类似
    std::shared_ptr<AST_Node> type; // type node
    std::shared_ptr<AST_Node> var;  // variable node
    std::vector<std::shared_ptr<AST_Node>> inits; // assignment nodes
    SingleVariableDeclaration_AST_Node(std::shared_ptr<AST_Node> typeNode, std::shared_ptr<AST_Node> varNode,
                                       std::vector<std::shared_ptr<AST_Node>> assignNodes)
            : type(std::move(typeNode)), var(std::move(varNode)), inits(std::move(assignNodes)) {}

    Value *accept(Visitor &v) override {
        return v.visit(*this);
    }
};

/// 变量声明语句对应的结点，注意：一个变量声明语句可能同时声明多个变量
class VariableDeclarations_AST_Node : public AST_Node {
public:
    std::vector<std::shared_ptr<AST_Node>> varDeclarations;

    explicit VariableDeclarations_AST_Node(std::vector<std::shared_ptr<AST_Node>> newVarDeclarations)
            : varDeclarations(std::move(newVarDeclarations)) {}

    Value *accept(Visitor &v) override {
        return v.visit(*this);
    }
};

/// 一元运算对应的结点
class UnaryOperator_AST_Node : public AST_Node {
public:
    TOKEN_TYPE op;
    std::shared_ptr<AST_Node> right;

    UnaryOperator_AST_Node(TOKEN_TYPE op, std::shared_ptr<AST_Node> Right)
            : op(op), right(std::move(Right)) {}

    Value *accept(Visitor &v) override {
        return v.visit(*this);
    }
};

/// 变量声明时的类型或函数定义中返回值的类型所对应的结点
class BaseType_AST_Node : public AST_Node {
public:

    Value *accept(Visitor &v) override {
        return v.visit(*this);
    }
};

/// 变量结点，变量声明和每次变量引用时都对应单独的变量结点，但对应的符号表项只有一个
class Variable_AST_Node : public AST_Node {
public:
    std::string var_name;
    // 针对数组
    int size = -1;      // 数组长度(声明数组变量时用到)
    std::vector<std::string> elements; // 用于记录全局变量初始化数据.
    std::shared_ptr<AST_Node> indexExpression = nullptr; // 数组下标表达式

    std::shared_ptr<Variable_Symbol> symbol;

    explicit Variable_AST_Node(std::string var) : var_name(std::move(var)) {}

    Value *accept(Visitor &v) override {
        return v.visit(*this);
    }
};

class Parameter_AST_Node : public AST_Node {
public:
    // 成员变量与SingleVariableDeclaration_AST_Node类似
    std::shared_ptr<AST_Node> parameterType;  // 参数类型结点
    std::shared_ptr<AST_Node> parameterVar;   // 参数名称结点
    std::shared_ptr<Variable_Symbol> symbol;

    Parameter_AST_Node(std::shared_ptr<AST_Node> typeNode, std::shared_ptr<AST_Node> varNode)
            : parameterType(std::move(typeNode)), parameterVar(std::move(varNode)) {}

    Value *accept(Visitor &v) override {
        return v.visit(*this);
    }
};

class Block_AST_Node : public AST_Node {
public:
    std::vector<std::shared_ptr<AST_Node>> statements;

    explicit Block_AST_Node(std::vector<std::shared_ptr<AST_Node>> newStatements)
            : statements(std::move(newStatements)) {}

    Value *accept(Visitor &v) override {
        return v.visit(*this);
    }
};

class FunctionDeclaration_AST_Node : public AST_Node {
public:
    std::shared_ptr<AST_Node> funcType;   // 函数返回值类型结点
    std::string funcName;                 // 函数名
    std::vector<std::shared_ptr<AST_Node>> formalParams; // 形参列表，即formal parameters
    std::shared_ptr<AST_Node> funcBlock;  // 函数体

    int offset = -1;                       // 偏移量

    FunctionDeclaration_AST_Node(std::shared_ptr<AST_Node> t, std::string n, std::vector<std::shared_ptr<AST_Node>> &&p,
                                 std::shared_ptr<AST_Node> b) :
            funcType(std::move(t)), funcName(std::move(n)), formalParams(p), funcBlock(std::move(b)) {}

    Value *accept(Visitor &v) override {
        return v.visit(*this);
    }
};

class FunctionCall_AST_Node : public AST_Node {
public:
    std::string funcName;  // 函数名
    std::vector<std::shared_ptr<AST_Node>> arguments; // 实参列表，即arguments

    FunctionCall_AST_Node(std::string name, std::vector<std::shared_ptr<AST_Node>> args) :
            funcName(std::move(name)), arguments(std::move(args)) {}

    Value *accept(Visitor &v) override {
        return v.visit(*this);
    }
};

class Program_AST_Node : public AST_Node {
public:
    // 全局变量List
    std::vector<std::shared_ptr<VariableDeclarations_AST_Node>> gvarDeclarationsList;
    // 函数定义List
    std::vector<std::shared_ptr<FunctionDeclaration_AST_Node>> funcDeclarationList;

    Program_AST_Node(std::vector<std::shared_ptr<VariableDeclarations_AST_Node>> newVarDeclList,
                     std::vector<std::shared_ptr<FunctionDeclaration_AST_Node>> newFuncDeclList) :
            gvarDeclarationsList(std::move(newVarDeclList)), funcDeclarationList(std::move(newFuncDeclList)) {}

    Value *accept(Visitor &v) override {
        return v.visit(*this);
    }
};
//===----------------------------------------------------------------------===//
// Parser
//===----------------------------------------------------------------------===//

// 如果想在.data段列出所有float常量，则用之，否则删掉就行。
std::vector<std::shared_ptr<NumLiteral_AST_Node>> floats;

class Parser {
    Lexer lexer;
    TOKEN CurrentToken;
    std::vector<TOKEN> tokenList;
    long pos = 0; // Token流指针
public:
    explicit Parser(Lexer lex) : lexer(std::move(lex)) {
        tokenList = std::move(lexer.constructTokenStream());
        CurrentToken = tokenList[pos++];
    }

    std::shared_ptr<AST_Node> parse();

private:
    void eatCurrentToken() { CurrentToken = tokenList[pos++]; };

    void put_backToken() { CurrentToken = tokenList[--pos - 1]; }

    std::shared_ptr<AST_Node> primary();

    std::shared_ptr<AST_Node> unary();

    std::shared_ptr<AST_Node> mul_div();

    std::shared_ptr<AST_Node> add_sub();

    std::shared_ptr<AST_Node> relational();

    std::shared_ptr<AST_Node> equality();

    std::shared_ptr<AST_Node> expression();

    std::shared_ptr<AST_Node> expression_statement();

    std::shared_ptr<AST_Node> block();

    std::shared_ptr<AST_Node> declarator();

    std::shared_ptr<AST_Node> type_specification();

    std::shared_ptr<AST_Node> variable_declaration();

    std::shared_ptr<AST_Node> statement();

    std::shared_ptr<AST_Node> formal_parameter();

    std::shared_ptr<AST_Node> function_declaration();

    std::shared_ptr<AST_Node> program();
};

// primary := num_literal
//              | "(" expression ")"
//              | identifier func_args?
//              | identifier "[" expression "]"
// func_args = "(" (expression ("," expression)*)? ")"
std::shared_ptr<AST_Node> Parser::primary() {
    // num_literal
    if (CurrentToken.type == TK_INT_LITERAL ||
        CurrentToken.type == TK_FLOAT_LITERAL ||
        CurrentToken.type == TK_BOOL_LITERAL ||
        CurrentToken.type == TK_CHAR_LITERAL) {
        auto node = std::make_shared<NumLiteral_AST_Node>(CurrentToken);
        eatCurrentToken();
        return node;
    }

    // "(" expression ")"
    if (CurrentToken.type == TK_LPAREN) {
        eatCurrentToken(); // eat "("
        auto node = std::move(expression());
        if (CurrentToken.type == TK_RPAREN)
            eatCurrentToken(); // eat ")"
        return node;
    }

    // identifier func_args?
    //            | "[" expression "]"
    // func_args = "(" (expression ("," expression)*)? ")"
    if (CurrentToken.type == TK_IDENTIFIER) {
        std::string name = CurrentToken.lexeme;  // 函数或变量名称
        eatCurrentToken();
        // 函数调用
        if (CurrentToken.type == TK_LPAREN) {
            eatCurrentToken();  // eat "("
            std::string functionName = name;
            std::vector<std::shared_ptr<AST_Node>> actualParamNodes;
            while (CurrentToken.type != TK_RPAREN) {
                auto paramNode = expression();
                actualParamNodes.push_back(paramNode);
                if (CurrentToken.type == TK_COMMA) eatCurrentToken(); // eat ","
            }
            eatCurrentToken();   // eat ")"
            return std::make_shared<FunctionCall_AST_Node>(name, actualParamNodes);
        }

        // 变量
        auto variableNode = std::make_shared<Variable_AST_Node>(name);
        // 变量1：数组元素
        if (CurrentToken.type == TK_LBRACK) {
            eatCurrentToken();  // eat "["
            variableNode->indexExpression = expression();
            variableNode->type = Ty_array;
            eatCurrentToken();   // eat "]"
        }
        // 变量2：基本变量
        return variableNode;
    }

    // error!
    {
        fprintf(stderr, "error!:  %s\n", CurrentToken.lexeme.c_str());
        exit(1);
    }

}

// unary :=	(“+” | “-” | “!” | “*” | “&”) unary | primary
std::shared_ptr<AST_Node> Parser::unary() {
    if (CurrentToken.type == TK_PLUS ||
        CurrentToken.type == TK_MINUS ||
        CurrentToken.type == TK_NOT ||
        CurrentToken.type == TK_MULorDEREF ||
        CurrentToken.type == TK_ADDRESS) {
        TOKEN tok = CurrentToken;
        eatCurrentToken();
        return std::make_shared<UnaryOperator_AST_Node>(tok.type, std::move(unary()));
    } else return primary();
}


// mul_div := unary ("*" unary | "/" unary)*
std::shared_ptr<AST_Node> Parser::mul_div() {
    std::shared_ptr<AST_Node> node = unary();
    while (true) {
        if (CurrentToken.type == TK_MULorDEREF || CurrentToken.type == TK_DIV) {
            TOKEN op_token = CurrentToken;
            eatCurrentToken();
            auto left = std::move(node);
            node = std::make_shared<BinaryOperator_AST_Node>(std::move(left), op_token.type,
                                                             std::move(unary()));
            continue;
        }
        return node;
    }
}

// add_sub  :=  mul_div ("+" mul_div | "-" mul_div)*
std::shared_ptr<AST_Node> Parser::add_sub() {
    auto node = mul_div();
    while (true) {
        if (CurrentToken.type == TK_PLUS || CurrentToken.type == TK_MINUS) {
            TOKEN op_token = CurrentToken;
            eatCurrentToken();
            auto left = std::move(node);
            node = std::make_shared<BinaryOperator_AST_Node>(std::move(left), op_token.type,
                                                             std::move(mul_div()));
            continue;
        }
        return node;
    }
}

// relational  :=  add_sub ("<" add_sub | "<=" add_sub | ">" add_sub | ">=" add_sub)*
std::shared_ptr<AST_Node> Parser::relational() {
    auto node = add_sub();
    while (true) {
        if (CurrentToken.type == TK_LT ||
            CurrentToken.type == TK_LE ||
            CurrentToken.type == TK_GT ||
            CurrentToken.type == TK_GE) {
            TOKEN op_token = CurrentToken;
            eatCurrentToken();
            auto left = std::move(node);
            node = std::make_shared<BinaryOperator_AST_Node>(std::move(left), op_token.type,
                                                             std::move(add_sub()));
            continue;
        }
        return node;
    }
}

// equality  :=  relational ("==" relational | "!=" relational)*
std::shared_ptr<AST_Node> Parser::equality() {
    auto node = relational();
    while (true) {
        if (CurrentToken.type == TK_EQ || CurrentToken.type == TK_NE) {
            TOKEN op_token = CurrentToken;
            eatCurrentToken();
            auto left = std::move(node);
            node = std::make_shared<BinaryOperator_AST_Node>(std::move(left), op_token.type,
                                                             std::move(relational()));
            continue;
        }
        return node;
    }
}

// expression  :=  equality ("=" expression)?
std::shared_ptr<AST_Node> Parser::expression() {
    auto node = equality();
    if (CurrentToken.type == TK_ASSIGN) {
        TOKEN assign_token = CurrentToken;
        eatCurrentToken();
        auto left = std::move(node);
        node = std::make_shared<Assignment_AST_Node>(std::move(left), std::move(assign_token.lexeme),
                                                     std::move(expression()));
    }
    return node;
}

// expression_statement  :=  expression? ";"
std::shared_ptr<AST_Node> Parser::expression_statement() {
    std::shared_ptr<AST_Node> node = nullptr;
    if (CurrentToken.type == TK_SEMICOLON) eatCurrentToken();
    else {
        node = expression();
        if (CurrentToken.type == TK_SEMICOLON) eatCurrentToken();
        else {
            fprintf(stderr, "error:  %s\n", CurrentToken.lexeme.c_str());
            exit(1);
        }
    }
    return node;
}

// declarator  := identifier type-suffix
// type-suffix  :=  ϵ | ("[" num_literal "]")?
std::shared_ptr<AST_Node> Parser::declarator() {
    if (CurrentToken.type == TK_IDENTIFIER) {
        auto node = std::make_shared<Variable_AST_Node>(std::move(CurrentToken.lexeme));
        eatCurrentToken();
        if (CurrentToken.type == TK_LBRACK) {
            eatCurrentToken(); // eat "["
            node->size = std::stoi(CurrentToken.lexeme);
            node->type = Ty_array;
            eatCurrentToken(); // eat size
            eatCurrentToken(); // eat "]"
        }
        return node;
    }
    return nullptr;
}

// type_specification := "int" | "float" | "bool" | "void" | "char"
std::shared_ptr<AST_Node> Parser::type_specification() {
    auto node = std::make_shared<BaseType_AST_Node>();
    switch (CurrentToken.type) {
        case TK_INT:
            node->type = Ty_int;
            break;
        case TK_FLOAT:
            node->type = Ty_float;
            break;
        case TK_CHAR:
            node->type = Ty_char;
            break;
        case TK_VOID:
            node->type = Ty_void;
            break;
        case TK_BOOL:
            node->type = Ty_bool;
            break;
        default:
            break;
    }
    eatCurrentToken();
    return node;
}

// variable_declaration	 :=  type_specification declarator ("=" expression)? ("," declarator ("=" expression)?)* ";"
//                         | type_specification declarator ("=" "{" (expression)? ("," expression)* "}")? ("," declarator ("=" expression)?)* ";"
std::shared_ptr<AST_Node> Parser::variable_declaration() {
    std::vector<std::shared_ptr<AST_Node>> VarDeclarations;
    std::shared_ptr<BaseType_AST_Node> basetypeNode = std::dynamic_pointer_cast<BaseType_AST_Node>(
            type_specification());
    while (CurrentToken.type != TK_SEMICOLON) {
        auto variableNode = std::dynamic_pointer_cast<Variable_AST_Node>(declarator());
        // 学习chibicc的做法，将变量初始化视为一个(或多个，对数组)赋值语句
        std::vector<std::shared_ptr<AST_Node>> initNodes;
        if (CurrentToken.type == TK_ASSIGN) {
            eatCurrentToken(); // eat "="
            if (CurrentToken.type == TK_LBRACE) { // 数组初始化
                eatCurrentToken(); // eat "{"
                int i = 0;   // 数组的下标从0开始
                while (CurrentToken.type != TK_RBRACE) {
                    // newVarNode是赋值语句左边的数组元素对应的变量
                    auto newVarNode = std::make_shared<Variable_AST_Node>(variableNode->var_name);
                    newVarNode->type = Ty_array;
                    newVarNode->baseType = basetypeNode->type;
                    // 构建一个与数组元素下标对应的表达式，即primary := identifier "[" expression "]"中的expression.
                    TOKEN tok = TOKEN{TK_INT_LITERAL, std::to_string(i), 0, 0};
                    auto indexNode = std::make_shared<NumLiteral_AST_Node>(tok);
                    indexNode->type = Ty_int;
                    newVarNode->indexExpression = indexNode;

                    initNodes.push_back(
                            std::make_shared<Assignment_AST_Node>(std::move(newVarNode), "=", std::move(expression())));
                    if (CurrentToken.type == TK_COMMA) eatCurrentToken(); // eat ","
                    i++;
                }
                eatCurrentToken(); // eat "}"
            } else {                              // 普通变量，即基本类型变量初始化
                initNodes.push_back(std::make_shared<Assignment_AST_Node>(variableNode, "=", std::move(expression())));
            }
        }
        auto node = std::make_shared<SingleVariableDeclaration_AST_Node>(basetypeNode, variableNode,
                                                                         std::move(initNodes));
        VarDeclarations.push_back(node);
        if (CurrentToken.type == TK_COMMA) eatCurrentToken(); // eat ","
    }
    eatCurrentToken();  // eat ";"
    return std::make_shared<VariableDeclarations_AST_Node>(VarDeclarations);
}

// statement  := expression_statement
//                   | block
//                   | variable_declaration
//                   | "return" expression-statement
//                   | "if" "(" expression ")" statement ("else" statement)?
//                   | "while" "(" expression ")" statement
//                   | "do" statement "while" "(" expression ")" ";"
//                   | for" "(" expression? ";" expression ";" expression? ")" statement
//                   | "break" ";"
//                   | "continue" ";"
//                   | "print" "(" expression ")" ";"
std::shared_ptr<AST_Node> Parser::statement() {
    // block
    if (CurrentToken.type == TK_LBRACE) return block();

    // variable_declaration
    if (CurrentToken.type == TK_INT ||
        CurrentToken.type == TK_FLOAT ||
        CurrentToken.type == TK_BOOL ||
        CurrentToken.type == TK_CHAR)
        return variable_declaration();

    // "return" expression-statement
    if (CurrentToken.type == TK_RETURN) {
        eatCurrentToken();  // eat "return"
        return std::make_shared<Return_AST_Node>(std::move(expression_statement()));
    }

    // "if" "(" expression ")" statement ("else" statement)?
    if (CurrentToken.type == TK_IF) {
        eatCurrentToken(); // eat "if"
        std::shared_ptr<AST_Node> condition, then_stmt, else_stmt;
        if (CurrentToken.type == TK_LPAREN) {
            eatCurrentToken(); // eat "("
            condition = expression();
            if (CurrentToken.type == TK_RPAREN)
                eatCurrentToken(); // eat ")"
        }
        then_stmt = statement();
        if (CurrentToken.type == TK_ELSE) {
            eatCurrentToken();  // eat "else"
            else_stmt = statement();
        }
        return std::make_shared<If_AST_Node>(condition, then_stmt, else_stmt);
    }

    // "while" "(" expression ")" statement
    if (CurrentToken.type == TK_WHILE) {
        eatCurrentToken(); // eat "while"
        std::shared_ptr<AST_Node> condition, stmt;
        if (CurrentToken.type == TK_LPAREN) {
            eatCurrentToken(); // eat "("
            condition = expression();
            if (CurrentToken.type == TK_RPAREN)
                eatCurrentToken(); // eat ")"
        }
        stmt = statement();
        return std::make_shared<While_AST_Node>(condition, stmt);
    }

    // "do" statement "while" "(" expression ")" ";"
    if (CurrentToken.type == TK_DO) {
        eatCurrentToken(); // eat "do"
        std::shared_ptr<AST_Node> stmt, condition;
        stmt = statement();
        if (CurrentToken.type == TK_WHILE) {
            eatCurrentToken(); // eat "while"
            eatCurrentToken(); // eat "("
            condition = expression();
            eatCurrentToken(); // eat ")"
            eatCurrentToken(); // eat ";"
        }

        return std::make_shared<DoWhile_AST_Node>(stmt, condition);
    }

    // "for" "(" expression? ";" expression ";" expression? ")" statement
    if (CurrentToken.type == TK_FOR) {
        eatCurrentToken(); // eat "for"
        std::shared_ptr<AST_Node> initialization, condition, increment, stmt;
        if (CurrentToken.type == TK_LPAREN) {
            eatCurrentToken(); // eat "("
            if (CurrentToken.type != TK_SEMICOLON)
                initialization = expression();
            eatCurrentToken(); // eat ";"
            condition = expression();
            eatCurrentToken(); // eat ";"
            if (CurrentToken.type != TK_RPAREN)
                increment = expression();
            eatCurrentToken(); // eat ")"
        }
        stmt = statement();
        return std::make_shared<For_AST_Node>(initialization, condition, increment, stmt);
    }

    // "break" ";"
    if (CurrentToken.type == TK_BREAK) {
        eatCurrentToken(); // eat "break"
        auto breakNode = std::make_shared<Break_AST_Node>();
        eatCurrentToken(); // eat ";"
        return breakNode;
    }

    // "continue" ";"
    if (CurrentToken.type == TK_CONTINUE) {
        eatCurrentToken(); // eat "continue"
        auto continueNode = std::make_shared<Continue_AST_Node>();
        eatCurrentToken(); // eat ";"
        return continueNode;
    }

    //                   | "print" "(" expression ")" ";"
    if (CurrentToken.type == TK_PRINT) {
        eatCurrentToken(); // eat "print"
        eatCurrentToken(); // eat "("
        auto valueNode = expression();
        eatCurrentToken(); // eat ")"
        eatCurrentToken(); // eat ";"
        return std::make_shared<Print_AST_Node>(valueNode);
    }

    // expression_statement
    return expression_statement();
}

// block  := "{" statement* "}"
std::shared_ptr<AST_Node> Parser::block() {
    std::vector<std::shared_ptr<AST_Node>> newStatements;
    eatCurrentToken();   // eat "{"
    // construct newStatements
    while (CurrentToken.type != TK_RBRACE && CurrentToken.type != TK_EOF) {
        std::shared_ptr<AST_Node> newStatement = statement();
        if (newStatement != nullptr)
            newStatements.push_back(std::move(newStatement));
    }
    std::shared_ptr<AST_Node> node = std::make_shared<Block_AST_Node>(std::move(newStatements));
    eatCurrentToken();   // eat "}"
    return node;
}

// formal_parameter  :=  type_specification identifier
std::shared_ptr<AST_Node> Parser::formal_parameter() {
    std::shared_ptr<BaseType_AST_Node> typeNode = std::dynamic_pointer_cast<BaseType_AST_Node>(type_specification());
    if (CurrentToken.type == TK_IDENTIFIER) {
        auto varNode = std::make_shared<Variable_AST_Node>(std::move(CurrentToken.lexeme));
        eatCurrentToken();  // eat parameter name
        auto node = std::make_shared<Parameter_AST_Node>(typeNode, varNode);
        return node;
    } else return nullptr;

}

// function_declaration  :=  type_specification identifier "(" formal_parameters? ")" block
// formal_parameters  :=  formal_parameter ("," formal_parameter)*
std::shared_ptr<AST_Node> Parser::function_declaration() {
    // 函数类型
    std::shared_ptr<BaseType_AST_Node> typeNode = std::dynamic_pointer_cast<BaseType_AST_Node>(type_specification());

    // 函数名称
    std::string funcName;
    if (CurrentToken.type == TK_IDENTIFIER) {
        funcName = CurrentToken.lexeme;
        eatCurrentToken();
    }
    // 函数形参列表
    if (CurrentToken.type == TK_LPAREN) eatCurrentToken(); // eat "("
    else {
        fprintf(stderr, "missing (");
        exit(1);
    }
    std::vector<std::shared_ptr<AST_Node>> formalParams;
    if (CurrentToken.type != TK_RPAREN) {
        while (CurrentToken.type != TK_COMMA && CurrentToken.type != TK_RPAREN) {
            auto newFormalParam = formal_parameter();
            formalParams.push_back(newFormalParam);
            if (CurrentToken.type == TK_COMMA) eatCurrentToken();
        }
    }
    if (CurrentToken.type == TK_RPAREN) eatCurrentToken(); // eat ")"
    // 函数body
    auto body = block();

    return std::make_shared<FunctionDeclaration_AST_Node>(std::move(typeNode), std::move(funcName),
                                                          std::move(formalParams), std::move(body));
}

// program  :=  (variable_declaration | function_declaration)*
std::shared_ptr<AST_Node> Parser::program() {
    std::vector<std::shared_ptr<VariableDeclarations_AST_Node>> variableDeclarationsList;
    std::vector<std::shared_ptr<FunctionDeclaration_AST_Node>> functionDeclarationList;
    while (CurrentToken.type != TK_EOF) {
        if (CurrentToken.type == TK_INT || CurrentToken.type == TK_FLOAT ||
            CurrentToken.type == TK_BOOL || CurrentToken.type == TK_VOID ||
            CurrentToken.type == TK_CHAR) {
            eatCurrentToken();
            if (CurrentToken.type == TK_IDENTIFIER) {
                eatCurrentToken();
                if (CurrentToken.type == TK_LPAREN) {
                    put_backToken();
                    put_backToken();
                    auto newFunction = std::dynamic_pointer_cast<FunctionDeclaration_AST_Node>(function_declaration());
                    functionDeclarationList.push_back(newFunction);
                } else {
                    put_backToken();
                    put_backToken();
                    auto newVariableDeclarations = std::dynamic_pointer_cast<VariableDeclarations_AST_Node>(
                            variable_declaration());
                    variableDeclarationsList.push_back(newVariableDeclarations);
                }
            }
        }
    }
    return std::make_shared<Program_AST_Node>(std::move(variableDeclarationsList), std::move(functionDeclarationList));
}


/*
 * program = variable-declarations list + function-declaration list
 * 1. 为什么第一个list的成员是variable-declarations呢？这是因为每个varialbe-declaration语句都有可能包含多个变量声明，如：
 *                            int a,b=5,c;
 * 每个变量的声明对应一个single-variable-declaration，它们组合成variable-declarations. 因此，
 * variable-declarations list又可以理解为“变量声明语句”的list.
 * 2. 对于函数，只能一个一个的声明，因此反而简洁，好理解.
 */

/******************** CFG (c-like)************************************
 *
 * program  :=  (variable_declaration | function_declaration)*
 * function_declaration  :=  type_specification identifier "(" formal_parameters? ")" block
 * formal_parameters  :=  formal_parameter ("," formal_parameter)*
 * formal_parameter  :=  type_specification identifier
 * block  :=  "{" statement* "}"
 * statement  := expression_statement
 *                   | block
 *                   | variable_declaration
 *                   | "return" expression-statement
 *                   | "if" "(" expression ")" statement ("else" statement)?
 *                   | "while" "(" expression ")" statement
 *                   | "do" statement "while" "(" expression ")" ";"
 *                   | for" "(" expression? ";" expression ";" expression? ")" statement
 *                   | "break" ";"
 *                   | "continue" ";"
 *                   | "print" "(" expression ")" ";"
 * variable_declaration	 :=  type_specification declarator ("=" expression)? ("," declarator ("=" expression)?)* ";"
 *                         | type_specification declarator ("=" "{" (expression)? ("," expression)* "}")? ("," declarator ("=" expression)?)* ";"
 * type_specification  :=  "int" | "float" | "bool" | "void" | "char"
 * declarator  :=  identifier type-suffix
 * type-suffix  :=  ϵ | ("[" num_literal "]")?
 * expression_statement  :=  expression? ";"
 * expression  :=  equality ("=" expression)?
 * equality  :=  relational ("==" relational | "!=" relational)*
 * relational  :=  add_sub ("<" add_sub | "<=" add_sub | ">" add_sub | ">=" add_sub)*
 * add_sub  :=  mul_div ("+" mul_div | "-" mul_div)*
 * mul_div  :=  unary ("*" unary | "/" unary)*
 * unary :=	(“+” | “-” | “!” | “*” | “&”) unary | primary
 * primary  :=  num_literal | "(" expression ")" | identifier func_args? | identifier "[" expression "]"
 * func_args = "(" (expression ("," expression)*)? ")"
 *
********************/
std::shared_ptr<AST_Node> Parser::parse() {
    return program();
}

//===----------------------------------------------------------------------===//
// Semantic analyzer derived from Visitor
//===----------------------------------------------------------------------===//

// 如果想在.data段列出所有全局变量，则用之，否则删掉就行.
std::vector<std::shared_ptr<Variable_AST_Node>> globals;

class SemanticAnalyzer : public Visitor {
public:
    std::shared_ptr<ScopedSymbolTable> currentScope;
    int offsetSum = 0;

    void analyze(AST_Node &tree) {
        tree.accept(*this);
    }

    Value *visit(NumLiteral_AST_Node &node) override {
        node.baseType = node.type;
        return nullptr;
    }

    Value *visit(Return_AST_Node &node) override {
        // 确定与return语句配对的函数名.
        std::shared_ptr<ScopedSymbolTable> tempScope = currentScope;
        while (tempScope != nullptr) {
            if (tempScope->category != "function") tempScope = tempScope->enclosing_scope;
            else {
                node.functionName = tempScope->name;
                break;
            }
        }

        // case: "return ;"
        if (node.expression == nullptr) node.baseType = Ty_void;
            // other cases:
        else {
            node.expression->accept(*this);
            node.baseType = node.expression->baseType;
        }
        type_returned_of_current_function = node.baseType;
        // 检查return语句的类型是否与函数返回值类型一致，不一致的话，则需要类型转换.
        // 首先获取函数的符号表项.
        std::shared_ptr<Symbol> functionSymbol = currentScope->lookup(node.functionName, false);
        if (functionSymbol == nullptr) {
            fprintf(stderr, "something wrong! info about function %s is missing\n", node.functionName.c_str());
            exit(1);
        }
        // 类型不一致，则为用户提供warning信息，并记录实际返回值类型.
        if (node.baseType != functionSymbol->type)
            fprintf(stderr, "type returned does not match the type of function \"%s\".\n", node.functionName.c_str());

        // 类型转换也可以放在visit BinaryOperation的时候进行.
        // 下面这种不一致有点严重，退出.
        if (node.baseType == Ty_void && functionSymbol->type != Ty_void) {
            fprintf(stderr, "return type of %s should not be void!\n", node.functionName.c_str());
            exit(1);
        }
        return nullptr;
    }

    Value *visit(Assignment_AST_Node &node) override {
        node.left->accept(*this);
        node.right->accept(*this);
        node.type = node.left->type;
        node.baseType = node.left->baseType;
        return nullptr;
    }

    Value *visit(BinaryOperator_AST_Node &node) override {
        node.left->accept(*this);
        node.right->accept(*this);
        // 检查运算数是否兼容.
        Ty left_Type = node.left->baseType;
        Ty right_Type = node.right->baseType;
        // 若兼容，取大，即bool < char < int < float
        node.baseType = (left_Type > right_Type) ? left_Type : right_Type;
        return nullptr;
    }

    Value *visit(UnaryOperator_AST_Node &node) override {
        node.right->accept(*this);
        node.baseType = node.right->baseType;
        return nullptr;
    }

    Value *visit(BaseType_AST_Node &node) override {
        node.baseType = node.type;
        return nullptr;
    }

    Value *visit(SingleVariableDeclaration_AST_Node &node) override {
        // 类型.
        node.type->accept(*this);
        std::shared_ptr<BaseType_AST_Node> basetypeNode = std::dynamic_pointer_cast<BaseType_AST_Node>(node.type);
        std::shared_ptr<Variable_AST_Node> varNode = std::dynamic_pointer_cast<Variable_AST_Node>(node.var);
        // varNode与basetypeNode是兄弟结点，用属性文法的术语讲，varNode的baseType属性是继承属性.
        // 该属性值的计算属SDT的内容，还是属于语义分析的内容？
        if (varNode->type == Ty_array) varNode->baseType = basetypeNode->type;
        else varNode->type = varNode->baseType = basetypeNode->type;
        node.baseType = varNode->type;

        std::vector<std::shared_ptr<Assignment_AST_Node>> initNodes;
        for (auto &init: node.inits)
            initNodes.push_back(std::dynamic_pointer_cast<Assignment_AST_Node>(init));

//        std::shared_ptr<Symbol> varSymbol = currentScope->lookup(varNode->var_name, true);
        std::shared_ptr<Variable_Symbol> varSymbol;
        if (currentScope->lookup(varNode->var_name, true) == nullptr) { // 在符号表中找不到，即尚未定义.
            if (currentScope->level == 0) {  // 全局变量.
                varSymbol = std::make_shared<Variable_Symbol>(varNode->var_name, varNode->type, varNode->baseType,
                                                              0, currentScope->level);
                for (auto &ele: node.inits) {
                    std::string initValueLiteral = std::dynamic_pointer_cast<NumLiteral_AST_Node>(
                            std::dynamic_pointer_cast<Assignment_AST_Node>(ele)->right)->literal;
                    varNode->elements.push_back(initValueLiteral);
                }

                globals.push_back(varNode);
            } else {  // 局部变量.
                if (varNode->type != Ty_array) {    // 1.非数组.
                    if (basetypeNode->type == Ty_char) offsetSum += 1;
                    else offsetSum += 8;
                } else {                            // 2.数组.
                    if (basetypeNode->type == Ty_int || basetypeNode->type == Ty_float)
                        offsetSum = offsetSum + varNode->size * 8;
                }
                varSymbol = std::make_shared<Variable_Symbol>(varNode->var_name, varNode->type, basetypeNode->type,
                                                              -offsetSum, currentScope->level);
            }
            varNode->symbol = varSymbol;
            currentScope->insert(varSymbol);
            varNode->symbol = std::dynamic_pointer_cast<Variable_Symbol>(varSymbol);
        } else {
            fprintf(stderr, "%s is declared more than one time.\n", varNode->var_name.c_str());
            exit(1);
        }

        // 初始化.
        for (auto &init: node.inits)
            if (init != nullptr) init->accept(*this);
        return nullptr;
    }

    Value *visit(VariableDeclarations_AST_Node &node) override {
        if (!node.varDeclarations.empty())
            for (auto &n: node.varDeclarations) n->accept(*this);
        return nullptr;
    }

    Value *visit(Variable_AST_Node &node) override {
        auto varSymbol = std::dynamic_pointer_cast<Variable_Symbol>(currentScope->lookup(node.var_name, false));
        if (varSymbol == nullptr) {
            fprintf(stderr, "something wrong with %s\n", node.var_name.c_str());
            exit(1);
        }
        node.type = varSymbol->type;
        node.baseType = varSymbol->baseType;
        if (node.symbol == nullptr) node.symbol = varSymbol;
        if (node.indexExpression != nullptr) node.indexExpression->accept(*this);
        return nullptr;
    }

    Value *visit(Block_AST_Node &node) override {
        // 创建block scope symbol_table.
        std::map<std::string, std::shared_ptr<Symbol>> symbols;
        auto scope = std::make_shared<ScopedSymbolTable>(std::move(symbols), 1, nullptr);
        scope->level = currentScope->level + 1;
        currentScope->child_no_count++;
        scope->name = currentScope->name + "." + std::to_string(currentScope->child_no_count);
        scope->category = "block";
        scope->enclosing_scope = currentScope;
        currentScope->sub_scopes.push_back(scope);
        currentScope = scope;

        if (!node.statements.empty())
            for (auto &n: node.statements) n->accept(*this);

        currentScope = currentScope->enclosing_scope;
        return nullptr;
    }

    Value *visit(If_AST_Node &node) override {
        if (node.condition != nullptr)
            node.condition->accept(*this);
        if (node.then_statement != nullptr)
            node.then_statement->accept(*this);
        if (node.else_statement != nullptr)
            node.else_statement->accept(*this);
        return nullptr;
    }

    Value *visit(While_AST_Node &node) override {
        if (node.condition != nullptr)
            node.condition->accept(*this);
        if (node.statement != nullptr)
            node.statement->accept(*this);
        return nullptr;
    }


    Value *visit(DoWhile_AST_Node &node) override {
        if (node.statement != nullptr)
            node.statement->accept(*this);
        if (node.condition != nullptr)
            node.condition->accept(*this);
        return nullptr;
    }

    Value *visit(For_AST_Node &node) override {
        if (node.initialization != nullptr)
            node.initialization->accept(*this);

        node.condition->accept(*this);

        if (node.increment != nullptr)
            node.increment->accept(*this);

        node.statement->accept(*this);
        return nullptr;
    }

    Value *visit(Break_AST_Node &node) override { return nullptr; }

    Value *visit(Continue_AST_Node &node) override { return nullptr; }

    Value *visit(Print_AST_Node &node) override {
        if (node.value != nullptr) node.value->accept(*this);
        return nullptr;
    }

    Value *visit(FunctionCall_AST_Node &node) override {
        std::shared_ptr<Symbol> functionSymbol = currentScope->lookup(node.funcName, false);
        if (functionSymbol == nullptr) {
            fprintf(stderr, "something wrong!!!\n");
            exit(1);
        }
        node.type = node.baseType = functionSymbol->type;
        // 目的是为了确定每一个实参node的Type.
        for (auto &eachArg: node.arguments) eachArg->accept(*this);
        return nullptr;
    }

    Value *visit(Parameter_AST_Node &node) override {
        // 类型.
        node.parameterType->accept(*this);
        node.type = node.baseType = node.parameterType->type;
        std::shared_ptr<BaseType_AST_Node> paramTypeNode = std::dynamic_pointer_cast<BaseType_AST_Node>(
                node.parameterType);

        std::shared_ptr<Variable_AST_Node> paramVarNode = std::dynamic_pointer_cast<Variable_AST_Node>(
                node.parameterVar);

        offsetSum += 8;
        auto sym = std::make_shared<Variable_Symbol>(paramVarNode->var_name, paramTypeNode->type, paramTypeNode->type,
                                                     -offsetSum, currentScope->level);
        currentScope->insert(sym);
        node.symbol = sym;
        return nullptr;
    }

    Value *visit(FunctionDeclaration_AST_Node &node) override {
        offsetSum = 0;  // 为每个函数初始化offsetSum.

        // visit类型结点.
        node.funcType->accept(*this);
        // 构建function symbol.
        auto sym = std::make_shared<Function_Symbol>(node.funcName, node.funcType->type);
        // 将function symbol插入gloabal scope符号表.
        global_scope->insert(sym);

        // 创建function scope symbol_table.
        std::map<std::string, std::shared_ptr<Symbol>> symbols;
        // level = 1
        auto scope = std::make_shared<ScopedSymbolTable>(std::move(symbols), 1, nullptr);
        scope->name = node.funcName;
        scope->category = "function";

        scope->enclosing_scope = global_scope;
        global_scope->sub_scopes.push_back(scope);
        currentScope = scope;

        // visit形参列表.
        for (auto &eachParam: node.formalParams)
            eachParam->accept(*this);

        // visit函数体.
        auto fun_block = std::dynamic_pointer_cast<Block_AST_Node>(node.funcBlock);
        for (auto &stmt: fun_block->statements) stmt->accept(*this);

        node.offset = offsetSum; // 此值即代码生成阶段的stack_size.
        return nullptr;
    }

    Value *visit(Program_AST_Node &node) override {
        // 创建global scope (program) symbol_table.
        std::map<std::string, std::shared_ptr<Symbol>> symbols;
        // level = 0
        global_scope = std::make_shared<ScopedSymbolTable>(std::move(symbols), 0, nullptr);
        global_scope->name = "c-link program symbol-table";
        global_scope->category = "global scope";

        if (!node.gvarDeclarationsList.empty()) {
            currentScope = global_scope;
            for (auto &var: node.gvarDeclarationsList) var->accept(*this);
        }

        if (!node.funcDeclarationList.empty())
            for (auto &func: node.funcDeclarationList) func->accept(*this);
        return nullptr;
    }
};

//===----------------------------------------------------------------------===//
// X86_Code generator derived from Visitor
//===----------------------------------------------------------------------===//

std::string parameter_registers[6] = {{"rdi"},
                                      {"rsi"},
                                      {"rdx"},
                                      {"rcx"},
                                      {"r8"},
                                      {"r9"}};

std::string parameter_registers_float[8] = {{"xmm0"},
                                            {"xmm1"},
                                            {"xmm2"},
                                            {"xmm3"},
                                            {"xmm4"},
                                            {"xmm5"},
                                            {"xmm6"},
                                            {"xmm7"}};

class X86_CodeGenerator : public Visitor {
private:
    // Round up `n` to the nearest multiple of `align`. For instance,
    // align_to(5, 8) returns 8 and align_to(11, 8) returns 16.
    static int align_to(int n, int align) {
        return int((n + align - 1) / align) * align;
    };

    int auto_label_no = 0;

public:
    void X86_code_generate(AST_Node &tree);

    Value *visit(NumLiteral_AST_Node &node) override {
        if (node.baseType == Ty_int)
            std::cout << "    mov $" << node.literal << ", %rax\n";
        else if (node.baseType == Ty_float) {
            union {
                double f64;
                uint64_t u64;
            } u{};
            u.f64 = strtod(node.literal.c_str(), nullptr);
            std::cout << "    mov $" << u.u64 << ", %rax   # float " << u.f64 << "\n";
            std::cout << "    movq %rax, %xmm0\n";
        } else if (node.baseType == Ty_bool) {
            // 类似c语言，true用整数1表示，flase用整数0表示.
            if (node.literal == "1") std::cout << "    mov $1, %rax\n";
            else std::cout << "    mov $0, %rax\n";
        } else if (node.baseType == Ty_char) {
            char c = node.literal.front();
            std::cout << "    mov $" << (int) c << ", %rax   # char " << "\n";
        }
        return nullptr;
    }

    Value *visit(Return_AST_Node &node) override {
        if (node.expression != nullptr) node.expression->accept(*this);
        std::cout << "    jmp L." << node.functionName << ".return\n";
        return nullptr;
    }

    void generate_variable_address(Variable_AST_Node &varNode) {
        /* 1. 全局变量 */
        if (varNode.symbol->scopeLevel == 0) {
            if (varNode.type == Ty_array) { // 全局数组变量，位于.data段.
                varNode.indexExpression->accept(*this);
                std::cout << "    mov %rax, %rcx\n";    // 数组元素index
                std::cout << "    lea " << varNode.var_name << "(%rip), %rax\n"; // 数组起始地址
            }
            else // 全局非结构化变量，位于.data段.
                std::cout << "    lea " << varNode.var_name << "(%rip), %rax\n";
        }
        /* 2. 局部变量 */
        else if (varNode.type == Ty_array) { // 若是数组元素，数组各个元素从左往右，内存地址从小到大(从下到上).
            varNode.indexExpression->accept(*this);
            std::cout << "    imul $8, %rax" << std::endl;
            std::cout << "    push %rax" << std::endl;
            std::cout << "    lea " << varNode.symbol->offset << "(%rbp), %rax" << std::endl;
            std::cout << "    pop %rdi" << std::endl;
            std::cout << "    add %rdi, %rax" << std::endl;
        } else // 若不是数组
            std::cout << "    lea " << varNode.symbol->offset << "(%rbp), %rax\n";
    }

    Value *visit(Assignment_AST_Node &node) override {
        // 赋值语句的左值是地址.
        std::shared_ptr<Variable_AST_Node> leftNode = std::dynamic_pointer_cast<Variable_AST_Node>(node.left);
        /* 1. 全局变量 */
        if (leftNode->symbol->scopeLevel == 0) {
            // 获取该地址，因是全局变量.
            generate_variable_address(*leftNode);
            std::cout << "    push %rax\n";
            std::cout << "    push %rcx\n";
            // 赋值语句的右值是数值.
            node.right->accept(*this);
            std::cout << "    pop %rcx\n";
            std::cout << "    pop %rdi\n";
            // 若right-side是int类型的结果，则位于%rax; 若是float类型，则位于%xxm0.
            if (node.baseType == Ty_float) { // float类型的结果.
                if (node.right->baseType < Ty_float) // 若%rax参与float类型的运算，
                    std::cout << "    cvtsi2sd %rax, %xmm0\n"; // 则需将之转换成float类型，并放入%xmm0.
                std::cout << "    movsd %xmm0, (%rdi, %rcx, 8)\n";
            } else std::cout << "    mov %rax, (%rdi, %rcx, 8)\n"; // int or bool, 即int类型的结果.
        }
        /* 2. 局部变量 */
        else {
            // 获取该地址，因是局部变量，该地址被放入%rax.
            generate_variable_address(*leftNode);
            // 由%rax入栈.
            std::cout << "    push %rax\n";
            // 赋值语句的右值是数值.
            node.right->accept(*this);
            std::cout << "    pop %rdi\n";
            // 若right-side是int类型的结果，则位于%rax; 若是float类型，则位于%xxm0.
            if (node.baseType == Ty_float) { // float类型的结果.
                if (node.right->baseType < Ty_float) // 若%rax参与float类型的运算，
                    std::cout << "    cvtsi2sd %rax, %xmm0\n"; // 则需将之转换成float类型，并放入%xmm0.
                std::cout << "    movsd %xmm0, (%rdi)\n";
            } else std::cout << "    mov %rax, (%rdi)\n"; // int or bool, 即int类型的结果.
        }
        return nullptr;
    }

    Value *visit(BinaryOperator_AST_Node &node) override {
        /* 1. float类型 */
        if (node.baseType == Ty_float) {
            // 算符右侧运算数.
            node.right->accept(*this);
            if (node.right->baseType < Ty_float) { // 整数转换为浮点数.
                std::cout << "    cvtsi2sd %rax, %xmm0" << std::endl;
            }
            // push float
            std::cout << "    sub $8, %rsp" << std::endl;
            std::cout << "    movsd %xmm0, (%rsp)" << std::endl;

            // 算符左侧运算数.
            node.left->accept(*this);
            if (node.left->baseType < Ty_float) { // 整数转换为浮点数.
                std::cout << "    cvtsi2sd %rax, %xmm0" << std::endl;
            }
            // pop float
            std::cout << "    movsd (%rsp), %xmm1" << std::endl;
            std::cout << "    add $8, %rsp" << std::endl;

            switch (node.op) {
                case TK_PLUS:
                    std::cout << "    addsd %xmm1, %xmm0" << std::endl;
                    return nullptr;
                case TK_MINUS:
                    std::cout << "    subsd %xmm1, %xmm0" << std::endl;
                    return nullptr;
                case TK_MULorDEREF:
                    std::cout << "    mulsd %xmm1, %xmm0" << std::endl;
                    return nullptr;
                case TK_DIV:
                    std::cout << "    divsd %xmm1, %xmm0" << std::endl;
                    return nullptr;
                case TK_EQ: // "=="
                case TK_NE: // "!="
                case TK_LT: // "<"
                case TK_LE: // "<="
                case TK_GT: // ">"
                case TK_GE: // ">="
                {
                    std::cout << "    ucomisd %xmm0, %xmm1" << std::endl;
                    switch (node.op) {
                        case TK_EQ:
                            std::cout << "    sete %al" << std::endl;
                            std::cout << "    setnp %dl" << std::endl;
                            std::cout << "    and %dl, %al" << std::endl;
                            break;
                        case TK_NE:
                            std::cout << "    setne %al" << std::endl;
                            std::cout << "    setp %dl" << std::endl;
                            std::cout << "    or %dl, %al" << std::endl;
                            break;
                        case TK_LT:
                            std::cout << "    setb %al" << std::endl;
                            break;
                        case TK_LE:
                            std::cout << "    setbe %al" << std::endl;
                            break;
                        case TK_GT:
                            std::cout << "    seta %al" << std::endl;
                            break;
                        case TK_GE:
                            std::cout << "    setae %al" << std::endl;
                            break;
                        default:
                            break;
                    }
                    std::cout << "    and $1, %al" << std::endl;
                    std::cout << "    movzb %al, %rax" << std::endl;
                    return nullptr;
                }

                default:
                    return nullptr;
            }
        }

        /* 2. 非float类型 */
        // 算符右侧运算数.
        node.right->accept(*this);
        std::cout << "    push %rax" << std::endl;
        // 算符左侧运算数.
        node.left->accept(*this);
        std::cout << "    pop %rdi" << std::endl;
        switch (node.op) {
            case TK_PLUS: {
                std::cout << "    add %rdi, %rax" << std::endl;
                return nullptr;
            }
            case TK_MINUS: {
                std::cout << "    sub %rdi, %rax" << std::endl;
                return nullptr;
            }
            case TK_MULorDEREF: {
                std::cout << "    imul %rdi, %rax" << std::endl;
                return nullptr;
            }
            case TK_DIV: {
                std::cout << "    div %rdi, %rax" << std::endl;
                return nullptr;
            }
            case TK_EQ: // "=="
            case TK_NE: // "!="
            case TK_LT: // "<"
            case TK_LE: // "<="
            case TK_GT: // ">"
            case TK_GE: // ">="
            {
                std::cout << "    cmp %rdi, %rax" << std::endl;
                switch (node.op) {
                    case TK_EQ:
                        std::cout << "    sete %al" << std::endl;
                        break;
                    case TK_NE:
                        std::cout << "    setne %al" << std::endl;
                        break;
                    case TK_LT:
                        std::cout << "    setl %al" << std::endl;
                        break;
                    case TK_LE:
                        std::cout << "    setle %al" << std::endl;
                        break;
                    case TK_GT:
                        std::cout << "    setg %al" << std::endl;
                        break;
                    case TK_GE:
                        std::cout << "    setge %al" << std::endl;
                        break;
                    default:
                        break;
                }
                std::cout << "    movzb %al, %rax" << std::endl;
                return nullptr;
            }
            default:
                return nullptr;
        }
    }

    Value *visit(UnaryOperator_AST_Node &node) override {
        switch (node.op) {
            case TK_PLUS:  // "+"
                return nullptr; // do nothing
            case TK_MINUS: { // "-" 取相反数.
                node.right->accept(*this); // 获得右值，存于%rax.
                if (node.baseType == Ty_int)
                    std::cout << "    neg %rax" << std::endl;
                else {
                    // 下面的四步将一个float取负数，即实现 %xmm0 = (-1) * %xmm0.
                    // 是有点啰嗦. chibicc有另一个解决方案，但是也需要四步，同样啰嗦 :)
                    std::cout << "    mov $1, %rax" << std::endl;
                    std::cout << "    neg %rax" << std::endl;
                    std::cout << "    cvtsi2sd %rax, %xmm1" << std::endl;
                    std::cout << "    mulsd %xmm1, %xmm0" << std::endl;
                }
                return nullptr;
            }
            case TK_NOT: { // "!" 取反.
                node.right->accept(*this); // 该步获得右值，存于%rax.
                if (node.baseType == Ty_bool) std::cout << "    not %rax" << std::endl;
                return nullptr;
            }

            case TK_ADDRESS: { // "&" 取地址.
                std::shared_ptr<Variable_AST_Node> varNode = std::dynamic_pointer_cast<Variable_AST_Node>(node.right);
                // 获取其在内存中的地址：
                int var_offset = varNode->symbol->offset;
                if (var_offset == 0)  // offset=0，则是全局变量，位于.data段
                    std::cout << "    lea " << varNode->var_name << "(%rip), %rax\n";
                else                  // 否则，是局部变量或参数，位于栈
                    std::cout << "    lea " << var_offset << "(%rbp), %rax\n";
                return nullptr;
            }
            case TK_MULorDEREF: { // "*" 取值.
                node.right->accept(*this); // 该步获得左值(即地址)，存于%rax.
                // 然后，将其值由内存放入寄存器.
                if (node.baseType == Ty_float)          // 若是float，
                    std::cout << "    movsd (%rax), %xmm0\n";  // 其值放入%xmm0.
                else if (node.baseType == Ty_int ||     // 否则，
                         node.baseType == Ty_bool ||
                         node.baseType == Ty_char)
                    std::cout << "    mov (%rax), %rax\n";     // 其值放入%rax.
                return nullptr;
            }
            default:
                return nullptr;
        }
    }

    Value *visit(BaseType_AST_Node &node) override { return nullptr; }

    Value *visit(SingleVariableDeclaration_AST_Node &node) override {
        for (auto &init: node.inits)
            if (init != nullptr) init->accept(*this);
        return nullptr;
    }

    Value *visit(VariableDeclarations_AST_Node &node) override {
        if (!node.varDeclarations.empty())
            for (auto &n: node.varDeclarations) n->accept(*this);
        return nullptr;
    }

    Value *visit(Variable_AST_Node &node) override {
        /* 变量是右值 */
        // 1.首先，获取其在内存中的地址
        generate_variable_address(node);

        // 2.然后，将其值由内存放入寄存器.
            // 2.1 全局变量
        if (node.symbol->scopeLevel == 0) {
            if (node.type == Ty_array) {
                if (node.baseType == Ty_float)          // 若是float，
                    std::cout << "    movsd (%rax,%rcx,8), %xmm0\n";  // 其值放入%xmm0.
                else std::cout << "    mov (%rax,%rcx,8), %rax\n";     // 其值放入%rax.
            } else {
                if (node.baseType == Ty_float)          // 若是float，
                    std::cout << "    movsd (%rax), %xmm0\n";  // 其值放入%xmm0.
                else std::cout << "    mov (%rax), %rax\n";     // 其值放入%rax.
            }
        }
        else {
            // 2.2 局部变量
            if (node.baseType == Ty_float)          // 若是float，
                std::cout << "    movsd (%rax), %xmm0\n";  // 其值放入%xmm0.
            else  // 否则，
                std::cout << "    mov (%rax), %rax\n";     // 其值放入%rax.
        }
        return nullptr;
    }

    Value *visit(Block_AST_Node &node) override {
        if (!node.statements.empty())
            for (auto &n: node.statements) n->accept(*this);
        return nullptr;
    }

    static void compare_zero(std::shared_ptr<AST_Node> &condition) {
        if (condition->baseType != Ty_float)
            std::cout << "    cmp $0, %rax" << std::endl;
        else { //  比如 if (3.1 - 3.1)，即 if (0.0) 的场合
            std::cout << "    xorpd %xmm1, %xmm1" << std::endl;
            std::cout << "    ucomisd %xmm1, %xmm0" << std::endl;
        }
    }

    Value *visit(If_AST_Node &node) override {
        // 此处的label，也可以如龙书第2章2.8.4小节所述，作为If_AST_Node的属性，在其构造函数中确定。
        auto_label_no += 1;
        std::string auto_label = std::to_string(auto_label_no);

        node.condition->accept(*this);
        compare_zero(node.condition);
        std::cout << "    je  .L.else." << auto_label << std::endl;

        node.then_statement->accept(*this);
        std::cout << "    jmp .L.end." << auto_label << std::endl;

        std::cout << ".L.else." << auto_label << ":\n";
        if (node.else_statement != nullptr)
            node.else_statement->accept(*this);
        std::cout << ".L.end." << auto_label << ":\n";
        return nullptr;
    }

    Value *visit(While_AST_Node &node) override {
        auto_label_no += 1;
        std::string auto_label = std::to_string(auto_label_no);
        std::string previous_break_label = current_break_label;
        std::string previous_continue_label = current_continue_label;
        current_break_label = ".L.end." + auto_label;
        current_continue_label = ".L.condition." + auto_label;

        std::cout << ".L.condition." << auto_label << ":\n";
        node.condition->accept(*this);
        compare_zero(node.condition);
        std::cout << "    je  .L.end." << auto_label << std::endl;

        node.statement->accept(*this);
        std::cout << "    jmp  .L.condition." << auto_label << std::endl;
        std::cout << ".L.end." << auto_label << ":\n";

        current_break_label = previous_break_label;
        current_continue_label = previous_continue_label;
        return nullptr;
    }

    Value *visit(DoWhile_AST_Node &node) override {
        auto_label_no += 1;
        std::string auto_label = std::to_string(auto_label_no);

        std::cout << ".L.do." << auto_label << ":\n";
        node.statement->accept(*this);

        node.condition->accept(*this);
        compare_zero(node.condition);
        std::cout << "    je  .L.end." << auto_label << std::endl;
        std::cout << "    jmp  .L.do." << auto_label << std::endl;
        std::cout << ".L.end." << auto_label << ":\n";
        return nullptr;
    }

    Value *visit(For_AST_Node &node) override {
        auto_label_no += 1;
        std::string auto_label = std::to_string(auto_label_no);
        std::string previous_break_label = current_break_label;
        std::string previous_continue_label = current_continue_label;

        if (node.initialization != nullptr) node.initialization->accept(*this);
        std::cout << ".L.condition." << auto_label << ":\n";
        node.condition->accept(*this);
        std::cout << "    je  .L.end." << auto_label << std::endl;
        node.statement->accept(*this);
        if (node.increment != nullptr) node.increment->accept(*this);
        std::cout << "    jmp  .L.condition." << auto_label << std::endl;
        std::cout << ".L.end." << auto_label << ":\n";

        current_break_label = previous_break_label;
        current_continue_label = previous_continue_label;
        return nullptr;
    }

    Value *visit(Break_AST_Node &node) override {
        std::cout << "    jmp " << current_break_label << std::endl;
        return nullptr;
    }

    Value *visit(Continue_AST_Node &node) override {
        std::cout << "    jmp " << current_continue_label << std::endl;
        return nullptr;
    }

    Value *visit(Print_AST_Node &node) override {
        node.value->accept(*this);
        switch (node.value->baseType) {
            case Ty_float:
                print_format_strings.emplace("printf_format_float", R"(    .string   "%f\n")");
                std::cout << "    lea printf_format_float(%rip), %rdi\n";
                break;
            case Ty_int:
                print_format_strings.emplace("printf_format_int", R"(    .string   "%d\n")");
                std::cout << "    lea printf_format_int(%rip), %rdi\n";
                std::cout << "    mov %rax, %rsi\n";
                break;
            case Ty_char:
                print_format_strings.emplace("printf_format_char", R"(    .string   "%c\n")");
                std::cout << "    lea printf_format_char(%rip), %rdi\n";
                std::cout << "    mov %rax, %rsi\n";
                break;
            default:
                break;
        }
        std::cout << "    call printf\n"; // 调用外部函数printf.
        return nullptr;
    }

    Value *visit(FunctionCall_AST_Node &node) override {
        int float_count = 0, others_count = 0;
        // 实参入栈.
        for (auto &argument: node.arguments) {
            argument->accept(*this);
            if (argument->baseType == Ty_float) {
                // push float.
                std::cout << "    sub $8, %rsp" << std::endl;
                std::cout << "    movsd %xmm0, (%rsp)" << std::endl;
                float_count += 1;
            } else {
                std::cout << "    push %rax" << std::endl;
                others_count += 1;
            }
        }
        // 实参分发，将数值存入寄存器.
        for (int j = node.arguments.size() - 1; j >= 0; j--) {
            if (node.arguments[j]->baseType == Ty_float) {
                float_count = float_count - 1;
                // pop float
                std::cout << "    movsd (%rsp), %" << parameter_registers_float[float_count] << std::endl;
                std::cout << "    add $8, %rsp" << std::endl;
            } else {
                others_count = others_count - 1;
                std::cout << "    pop %" << parameter_registers[others_count] << std::endl;
            }
        }

        std::cout << "    mov $0, %rax\n";
        std::cout << "    call " << node.funcName << std::endl;
        return nullptr;
    }

    Value *visit(Parameter_AST_Node &node) override {
        node.accept(*this);
        return nullptr;
    }

    Value *visit(FunctionDeclaration_AST_Node &node) override {
        // Prologue
        std::cout << "\n" << "    .text\n";
        std::cout << "    .global " << node.funcName << std::endl;
        std::cout << node.funcName << ":\n";
        std::cout << "    push %rbp\n";
        std::cout << "    mov %rsp, %rbp\n";
        int stack_size = align_to(node.offset, 16);
        if (stack_size != 0)
            std::cout << "    sub $" << stack_size << ", %rsp\n";

        // 从左往右，将参数由寄存器压读出，放入栈.
        int i = 0, f = 0;
        for (auto &param: node.formalParams) {
            std::shared_ptr<Parameter_AST_Node> p = std::dynamic_pointer_cast<Parameter_AST_Node>(param);
            if (param->type == Ty_float)
                std::cout << "    movq %" << parameter_registers_float[f++] << ", " << p->symbol->offset << "(%rbp)\n";
            else std::cout << "    mov %" << parameter_registers[i++] << ", " << p->symbol->offset << "(%rbp)\n";
        }

        node.funcBlock->accept(*this);

        std::cout << "L." << node.funcName << ".return:\n";

        // Epilogue
        std::cout << "    mov %rbp, %rsp\n";
        std::cout << "    pop %rbp\n";
        std::cout << "    ret\n";
        return nullptr;
    }

    Value *visit(Program_AST_Node &node) override {
        // 1. 遍历函数List，生成各个函数对应的汇编代码
        if (!node.funcDeclarationList.empty())
            for (auto &n: node.funcDeclarationList) n->accept(*this);

        // 2. 对于全局变量，已经在语义分析阶段将其信息导入globals，
        //    故无需遍历全局变量List，而只需根据globals，
        //    将全局或静态变量对应的汇编代码放在最后即可
        if (!globals.empty()) std::cout << "\n" << "    .data\n";
        for (auto &gvar: globals) {
            std::cout << gvar->var_name << ":\n";
            for (auto &e : gvar->elements)
                if (gvar->baseType == Ty_float)
                    std::cout << "    .double " << e << "\n";
                else // if (gvar.type == Ty_int)
                    std::cout << "    .quad " << e << "\n";
        }
        return nullptr;
    }
};

void X86_CodeGenerator::X86_code_generate(AST_Node &tree) {

    // 遍历AST
    tree.accept(*this);

    // 若想调用printf打印，则在.data段的最后添加“格式控制字符串”.
    for (auto &f: print_format_strings) {
        std::cout << f.first << ":\n";
        std::cout << f.second << std::endl;
    }
}



//===----------------------------------------------------------------------===//
// llvm_IR_Code generator derived from Visitor
//===----------------------------------------------------------------------===//


class Scope {
public:
    std::vector<std::map<std::string, Value *>> inner;

    // enter a new scope
    void enter_new() {
        std::map<std::string, Value *> mp = {};
        inner.push_back(mp);
    }

    // exit a scope
    void exit() {
        inner.pop_back();
    }

    bool push(std::string &name, Value *val) {
        auto result = inner[inner.size() - 1].insert({name, val});
        return result.second;
    }

    Value *find(std::string &name) {
        for (auto s = inner.rbegin(); s != inner.rend(); s++) {
            auto iter = s->find(name);
            if (iter != s->end()) {
                return iter->second;
            }
        }
        return nullptr;
    }
};

static LLVMContext TheContext;
static IRBuilder<> Builder(TheContext);
static std::unique_ptr<Module> TheModule;
// 即符号表
Scope scope;

void *LogErrorV(const char *Str) {
    printf("Code generation error: \n%s\n", Str);
    return nullptr;
}

class IR_CodeGenerator : public Visitor {

public:
    void IR_code_generate(AST_Node &tree);

    Value *visit(NumLiteral_AST_Node &node) override {
        switch (node.baseType) {
            case Ty_int:
                return ConstantInt::get(TheContext, APInt(32, std::stoi(node.literal)));
            case Ty_bool:
                return ConstantInt::get(TheContext, APInt(1, std::stoi(node.literal)));
            case Ty_float:
                return ConstantFP::get(TheContext, APFloat(std::stof(node.literal)));
            default:
                return nullptr;
        }
    }

    Value *visit(Return_AST_Node &node) override {
        if (node.expression == nullptr) {
            return Builder.CreateRetVoid();
        } else {
            Value *val = node.expression->accept(*this);
            return Builder.CreateRet(val);
        }
    }

    Value *visit(Assignment_AST_Node &node) override {
        /* 赋值语句的左值是地址，获取该地址。该地址在访问SingleVariableDeclaration_AST_Node时，
         * 已经分配，并记录在符号表，即scope.                 */
        auto v = std::dynamic_pointer_cast<Variable_AST_Node>(node.left);
        Value *var = scope.find(v->var_name);

        Value *value = node.right->accept(*this);
        return Builder.CreateStore(value, var);
    }

    Value *visit(BinaryOperator_AST_Node &node) override {
        Value *L = node.left->accept(*this);
        Value *R = node.right->accept(*this);
        if (!L || !R) {
            return nullptr;
        }

        auto lefttype = L->getType();
        auto righttype = R->getType();

        if (lefttype == righttype) {
            if (lefttype == Type::getInt32Ty(TheContext)) {
                if (node.op == TK_PLUS) {
                    return Builder.CreateAdd(L, R, "addtmp");
                } else if (node.op == TK_MINUS) {
                    return Builder.CreateSub(L, R, "subtmp");
                } else if (node.op == TK_MULorDEREF) {
                    return Builder.CreateMul(L, R, "multmp");
                } else if (node.op == TK_DIV) {
                    return Builder.CreateSDiv(L, R, "dictmp");
                } else if (node.op == TK_LT) {
                    L = Builder.CreateICmpULT(L, R, "cmptemp");
                    return Builder.CreateUIToFP(L, Type::getDoubleTy(TheContext), "booltmp");
                } else if (node.op == TK_GT) {
                    L = Builder.CreateICmpUGT(L, R, "cmptemp");
                    return Builder.CreateUIToFP(L, Type::getDoubleTy(TheContext), "booltmp");
                } else if (node.op == TK_LE) {
                    L = Builder.CreateICmpULE(L, R, "cmptmp");
                    return Builder.CreateUIToFP(L, Type::getDoubleTy(TheContext), "booltmp");
                } else if (node.op == TK_GE) {
                    L = Builder.CreateICmpUGE(L, R, "cmptmp");
                    return Builder.CreateUIToFP(L, Type::getDoubleTy(TheContext), "booltmp");
                } else if (node.op == TK_EQ) {
                    Value *LF = Builder.CreateSIToFP(L, Type::getFloatTy(TheContext));
                    Value *RF = Builder.CreateSIToFP(R, Type::getFloatTy(TheContext));
                    L = Builder.CreateFCmpUEQ(LF, RF, "cmptmp");
                    return Builder.CreateUIToFP(L, Type::getDoubleTy(TheContext), "booltmp");
                } else if (node.op == TK_NE) {
                    Value *LF = Builder.CreateSIToFP(L, Type::getFloatTy(TheContext));
                    Value *RF = Builder.CreateSIToFP(R, Type::getFloatTy(TheContext));
                    L = Builder.CreateFCmpUNE(LF, RF, "cmptmp");
                    return Builder.CreateUIToFP(L, Type::getDoubleTy(TheContext), "booltmp");
                }
            } else if (lefttype == Type::getFloatTy(TheContext)) {
                if (node.op == TK_PLUS) {
                    return Builder.CreateFAdd(L, R, "addtmp");
                } else if (node.op == TK_MINUS) {
                    return Builder.CreateFSub(L, R, "subtmp");
                } else if (node.op == TK_MULorDEREF) {
                    return Builder.CreateFMul(L, R, "multmp");
                } else if (node.op == TK_DIV) {
                    return Builder.CreateFDiv(L, R, "dictmp");
                } else if (node.op == TK_LT) {
                    L = Builder.CreateFCmpULT(L, R, "cmptemp");
                    return Builder.CreateUIToFP(L, Type::getDoubleTy(TheContext), "booltmp");
                } else if (node.op == TK_GT) {
                    L = Builder.CreateFCmpUGT(L, R, "cmptemp");
                    return Builder.CreateUIToFP(L, Type::getDoubleTy(TheContext), "booltmp");
                } else if (node.op == TK_LE) {
                    L = Builder.CreateFCmpULE(L, R, "cmptmp");
                    return Builder.CreateUIToFP(L, Type::getDoubleTy(TheContext), "booltmp");
                } else if (node.op == TK_GE) {
                    L = Builder.CreateFCmpUGE(L, R, "cmptmp");
                    return Builder.CreateUIToFP(L, Type::getDoubleTy(TheContext), "booltmp");
                } else if (node.op == TK_EQ) {
                    L = Builder.CreateFCmpUEQ(L, R, "cmptmp");
                    return Builder.CreateUIToFP(L, Type::getDoubleTy(TheContext), "booltmp");
                } else if (node.op == TK_NE) {
                    L = Builder.CreateFCmpUNE(L, R, "cmptmp");
                    return Builder.CreateUIToFP(L, Type::getDoubleTy(TheContext), "booltmp");
                }
            }

        }
        return nullptr;
    }

    Value *visit(UnaryOperator_AST_Node &node) override {
        Value *rightValue = node.right->accept(*this);
        if (!rightValue) return nullptr;
        auto righttype = rightValue->getType();

        if (righttype == Type::getInt32Ty(TheContext)) {
            if (node.op == TK_MINUS) {
                if (rightValue->getType() == Type::getInt32Ty(TheContext)) {
                    return Builder.CreateFPToSI(Builder.CreateFNeg(
                                                        Builder.CreateSIToFP(rightValue, Type::getFloatTy(TheContext), "int->float"), "neg temp"),
                                                Type::getInt32Ty(TheContext), "int->float");
                } else if (rightValue->getType() == Type::getFloatTy(TheContext)) {
                    return Builder.CreateFNeg(rightValue, "neg temp");
                } else report_error("the operand of \"-\" should be an integer or a float");
            }
        }
        return nullptr;
    }

    Value *visit(BaseType_AST_Node &node) override {
        return nullptr;
    }

    Value *visit(SingleVariableDeclaration_AST_Node &node) override {
        Function *func = Builder.GetInsertBlock()->getParent();
        Type *type;
        Value *value;
        switch (node.baseType) {
            case Ty_int:
                type = Type::getInt32Ty(TheContext);
                value = ConstantInt::get(TheContext, APInt(32, 0));
                break;
            case Ty_bool:
                type = Type::getInt1Ty(TheContext);
                value = ConstantInt::get(TheContext, APInt(1, 0));
                break;
            case Ty_float:
                type = Type::getFloatTy(TheContext);
                value = ConstantFP::get(TheContext, APFloat(0.0));
                break;
            default:
                type = nullptr;
                break;
        }

        IRBuilder<> Tmp(&func->getEntryBlock(), func->getEntryBlock().begin());
        auto v = std::dynamic_pointer_cast<Variable_AST_Node>(node.var);
        AllocaInst *allocation = Tmp.CreateAlloca(type, nullptr, v->var_name);

        scope.push(v->var_name, allocation);

        for (auto &each: node.inits)
            each->accept(*this);

        return nullptr;
    }

    Value *visit(VariableDeclarations_AST_Node &node) override {
        if (!node.varDeclarations.empty())
            for (auto &n: node.varDeclarations) n->accept(*this);
        return nullptr;
    }

    Value *visit(Variable_AST_Node &node) override {
        Value *var = scope.find(node.var_name);
        Type *type;
        switch (node.baseType) {
            case Ty_int:
                type = Type::getInt32Ty(TheContext);
                break;
            case Ty_bool:
                type = Type::getInt1Ty(TheContext);
                break;
            case Ty_float:
                type = Type::getFloatTy(TheContext);
                break;
            default:
                type = nullptr;
                break;
        }
        return Builder.CreateLoad(type, var, node.var_name);
    }

    Value *visit(Block_AST_Node &node) override {
        scope.enter_new(); //  进入{}scope
        for (auto &stmt: node.statements)
            stmt->accept(*this);
        scope.exit(); // 退出 {} scope
        return nullptr;
    }

    Value *visit(If_AST_Node &node) override {
        // 计算condition的值，可能是各种类型的值.
        Value *condV = node.condition->accept(*this);
        if (!condV) return nullptr;
        // 通过与0或0.0比较，确认将该值转换为bool类型的值.
        if (condV->getType() == Type::getInt1Ty(TheContext)) {
            condV = Builder.CreateICmpNE(condV, ConstantInt::get(TheContext, APInt(1, 0, false)), "ifcondition");
        } else if (condV->getType() == Type::getInt32Ty(TheContext)) {
            condV = Builder.CreateICmpNE(condV, ConstantInt::get(TheContext, APInt(32, 0, false)), "ifcondition");
        } else {
            condV = Builder.CreateFCmpONE(condV, ConstantFP::get(TheContext, APFloat(0.0)), "ifcondition");
        }

        Function *TheFunction = Builder.GetInsertBlock()->getParent();

        if (node.else_statement != nullptr) { // 有else
            auto trueBB = BasicBlock::Create(TheContext, "if.then", TheFunction);
            auto falseBB = BasicBlock::Create(TheContext, "if.else", TheFunction);
            // 创建after_ifBB基本块时，无需参数TheFunction，
            // 因为尚不确定在上面两个基本块中是否有return指令.
            // 若任性加上参数TheFunction，则与后面的after_ifBB->insertInto(TheFunction)冲突
            // 程序出现异常.
            auto after_ifBB = BasicBlock::Create(TheContext, "after-if");
            Builder.CreateCondBr(condV, trueBB, falseBB);

            // trueBB
            Builder.SetInsertPoint(trueBB);
            node.then_statement->accept(*this); // 插入trueBB基本块的代码.
            int insertedFlag = 0;
            if (Builder.GetInsertBlock()->getTerminator() == nullptr) { // not returned inside the block
                after_ifBB->insertInto(TheFunction); // 确定插入after_ifBB.
                insertedFlag = 1;                    // 设置标志，说明已确定插入
                Builder.CreateBr(after_ifBB);        // 跳到after_ifBB基本块
            }

            // falseBB
            Builder.SetInsertPoint(falseBB);
            node.else_statement->accept(*this); // 插入falseBB基本块的代码.
            if (Builder.GetInsertBlock()->getTerminator() == nullptr) { // not returned inside the block
                if (!insertedFlag) {     // 若尚未决定是否插入after_ifBB.
                    after_ifBB->insertInto(TheFunction); // 确定插入after_ifBB.
                    insertedFlag = 1;                    // 设置标志，说明已确定插入.
                }
                Builder.CreateBr(after_ifBB); // 跳到after_ifBB基本块
            }
            // 在这里插入after_ifBB基本块代码.
            if (insertedFlag) Builder.SetInsertPoint(after_ifBB);
        } else {  // 没有else部分
            auto trueBranch = BasicBlock::Create(TheContext, "trueBranch", TheFunction);
            auto after_ifBB = BasicBlock::Create(TheContext, "after-if", TheFunction);
            Builder.CreateCondBr(condV, trueBranch, after_ifBB);

            // trueBB
            Builder.SetInsertPoint(trueBranch);
            node.then_statement->accept(*this);

            // after_ifBB
            if (Builder.GetInsertBlock()->getTerminator() == nullptr)
                Builder.CreateBr(after_ifBB); // not returned inside the block
            Builder.SetInsertPoint(after_ifBB);
        }
        return nullptr;
    }

    Value *visit(While_AST_Node &node) override {
        Function *func = Builder.GetInsertBlock()->getParent();
        BasicBlock *condition = BasicBlock::Create(TheContext, "while.condition", func);
        BasicBlock *whileBody = BasicBlock::Create(TheContext, "while.body", func);
        BasicBlock *afterWhile = BasicBlock::Create(TheContext, "after-while", func);

        if (Builder.GetInsertBlock()->getTerminator() == nullptr)
            Builder.CreateBr(condition);

        // Prepare to emit the body of while
        Builder.SetInsertPoint(condition);
        // Emit the code to compute the value of condition.
        Value *condV = node.condition->accept(*this);
        if (!condV)
            return nullptr;
        // condition的值可能是各种类型的值，通过与0或0.0比较，确认将其转换为bool类型的值.
        if (condV->getType() == Type::getInt1Ty(TheContext)) {
            condV = Builder.CreateICmpNE(condV, ConstantInt::get(TheContext, APInt(1, 0, false)), "whilecondition");
        } else if (condV->getType() == Type::getInt32Ty(TheContext)) {
            condV = Builder.CreateICmpNE(condV, ConstantInt::get(TheContext, APInt(32, 0, false)), "whilecondition");
        } else {
            condV = Builder.CreateFCmpONE(condV, ConstantFP::get(TheContext, APFloat(0.0)), "whilecondition");
        }
        Builder.CreateCondBr(condV, whileBody, afterWhile);

        // Prepare to emit the body of while
        Builder.SetInsertPoint(whileBody);
        // Emit the body of
        node.statement->accept(*this);
        if (Builder.GetInsertBlock()->getTerminator() == nullptr)
            Builder.CreateBr(condition);

        // Prepare to emit the code after while
        Builder.SetInsertPoint(afterWhile);

        return nullptr;
    }

    Value *visit(DoWhile_AST_Node &node) override {
        return nullptr;
    }

    Value *visit(For_AST_Node &node) override {
        return nullptr;
    }

    Value *visit(Break_AST_Node &node) override {
        return nullptr;
    }

    Value *visit(Continue_AST_Node &node) override {
        return nullptr;
    }

    Value *visit(Print_AST_Node &node) override {
        return nullptr;
    }

    Value *visit(FunctionCall_AST_Node &node) override {
        Function *callerFunc = TheModule->getFunction(node.funcName);
        if (callerFunc == nullptr) {
            exit(0);
        } else {
            std::vector<Value *> funargs;
            for (auto &expr: node.arguments) {
                auto argValue = expr->accept(*this);
                funargs.push_back(argValue);
            }

            return Builder.CreateCall(callerFunc, funargs, "calltmp");
        }
        return nullptr;
    }

    Value *visit(Parameter_AST_Node &node) override {
        Value *pAlloc;
        switch (node.baseType) {
            case Ty_int:
                pAlloc = Builder.CreateAlloca(Type::getInt32Ty(TheContext));
                break;
            case Ty_bool:
                pAlloc = Builder.CreateAlloca(Type::getInt1Ty(TheContext));
                break;
            case Ty_float:
                pAlloc = Builder.CreateAlloca(Type::getFloatTy(TheContext));
                break;
            default:
                pAlloc = nullptr;
                break;
        }
        auto v = std::dynamic_pointer_cast<Variable_AST_Node>(node.parameterVar);
        scope.push(v->var_name, pAlloc);
        return nullptr;
    }

    // helper function: construct_function
    static Function *construct_function(FunctionDeclaration_AST_Node &node) {
        // 返回值类型
        Type *return_type;
        switch (node.funcType->type) {
            case Ty_int:
                return_type = Type::getInt32Ty(TheContext);
                break;
            case Ty_bool:
                return_type = Type::getInt1Ty(TheContext);
                break;
            case Ty_float:
                return_type = Type::getFloatTy(TheContext);
                break;
            default:  // i.e., Ty_void
                return_type = Type::getVoidTy(TheContext);
        }

        // 形参类型
        std::vector<Type *> parameterTypes;
        if (!node.formalParams.empty()) {
            for (auto &each: node.formalParams) {
                switch (each->type) {
                    case Ty_int:
                        parameterTypes.push_back(Type::getInt32Ty(TheContext));
                        break;
                    case Ty_bool:
                        parameterTypes.push_back(Type::getInt1Ty(TheContext));
                        break;
                    case Ty_float:
                        parameterTypes.push_back(Type::getFloatTy(TheContext));
                        break;
                    default:
                        break;
                }
            }
        }

        // 构建函数头（不含函数体）
        FunctionType *FunctionType = FunctionType::get(return_type, parameterTypes,
                                                       false/* doesn't have variadic args */);
        Function *F = Function::Create(FunctionType,
                                       Function::ExternalLinkage,
                                       node.funcName,
                                       TheModule.get());
        return F;
    }

    Value *visit(FunctionDeclaration_AST_Node &node) override {

        // 构建函数头
        Function *f = construct_function(node); // call helper function: construct_function
        if (!f) {
            std::string stringy = "Function '" + node.funcName + "' is not defined properly\n";
            return (Function *) LogErrorV(stringy.c_str());
        }

        // for later function-call
        scope.push(node.funcName, f); // into global scope

        // 准备构建函数体
        scope.enter_new();  // enter current function scope

        BasicBlock *basicblock = BasicBlock::Create(TheContext, "entry", f);
        Builder.SetInsertPoint(basicblock);

        // 在函数栈帧为各个参数开辟空间
        if (!node.formalParams.empty())
            for (auto &each: node.formalParams)
                each->accept(*this);

        // 存储参数的具体取值（由caller提供）
        std::vector<Value *> args_value;
        for (auto arg = f->arg_begin(); arg != f->arg_end(); arg++) {
            args_value.push_back(arg);
        }
        if (!node.formalParams.empty() && !args_value.empty()) { // assert node params size = args_value size
            int i = 0;
            for (auto &arg: node.formalParams) {
                auto p = std::dynamic_pointer_cast<Parameter_AST_Node>(arg);
                auto pVar = std::dynamic_pointer_cast<Variable_AST_Node>(p->parameterVar);
                auto pAlloc = scope.find(pVar->var_name);
                if (pAlloc == nullptr) {
                    exit(0);
                } else {
                    Builder.CreateStore(args_value[i++], pAlloc);
                }
            }
        }


        Value *returner = node.funcBlock->accept(*this);
        // 注意到这里用了llvm::verifyFunction, 它的作用是检查我们创建的函数是否正确，确保它是符合LLVM IR规范的。
        verifyFunction(*f);

        scope.exit();  // exist current function scope
        return f;
    }

    Value *visit(Program_AST_Node &node) override {
        // 1. 对于全局变量，已经在语义分析阶段将其信息导入globals，
        //    故无需遍历全局变量List，而只需根据globals处理
        scope.enter_new(); // enter global scope
//        for (auto &gvar: globals) {
//            switch (gvar.type) {
//                case Ty_int: {
//                    auto gdAlloc = new GlobalVariable(*TheModule, Type::getInt32Ty(TheContext), false,
//                                                      GlobalVariable::LinkageTypes::ExternalLinkage,
//                                                      ConstantInt::get(TheContext,
//                                                                       APInt(32, std::stoi(gvar.initialValueLiteral))));
//                    scope.push(gvar.name, gdAlloc);
//                    break;
//                }
//                case Ty_bool: {
//                    ConstantAggregateZero *zeroNum = ConstantAggregateZero::get(Type::getInt1Ty(TheContext));
//                    auto gdAlloc = new GlobalVariable(*TheModule, Type::getInt1Ty(TheContext), false,
//                                                      GlobalVariable::LinkageTypes::ExternalLinkage,
//                                                      ConstantInt::get(TheContext,
//                                                                       APInt(1, std::stoi(gvar.initialValueLiteral))));
//                    scope.push(gvar.name, gdAlloc);
//                    break;
//                }
//                case Ty_float: {
//                    auto gdAlloc = new GlobalVariable(*TheModule, Type::getFloatTy(TheContext), false,
//                                                      GlobalVariable::LinkageTypes::ExternalLinkage,
//                                                      ConstantFP::get(TheContext,
//                                                                      APFloat(std::stof(gvar.initialValueLiteral))));
//                    scope.push(gvar.name, gdAlloc);
//                    break;
//                }
//                default:
//                    break;
//            }
//        }

        // 2. 对于函数，逐一处理
        if (!node.funcDeclarationList.empty())
            for (auto &func: node.funcDeclarationList)
                func->accept(*this);

        scope.exit(); // exit global scope
        return nullptr;
    }
};

void IR_CodeGenerator::IR_code_generate(AST_Node &tree) {
    // Prepare the module, which holds all the SSA code.
    TheModule = std::make_unique<Module>("source.c", TheContext);

    // 遍历AST，填充TheModule
    tree.accept(*this);

    auto Filename = "output.ll";
    std::error_code EC;
    raw_fd_ostream dest(Filename, EC, sys::fs::OF_None);
    if (EC) {
        errs() << "Could not open file: " << EC.message();
        return;
    }
    TheModule->print(dest, nullptr);
}


//===----------------------------------------------------------------------===//
// Main driver code.
//===----------------------------------------------------------------------===//


int main(int argc, char *argv[]) {
    if (argc == 2) {
        /* source code can be read from one of two places:
         * 1. standard input terminal. As in test.sh, it's represented by "-".
         *    This approach is convenient for testing a large number of cases
         *    by running "make test" command.
         * 2. a file. This approach is convenient for debugging a single test case.
         * */
        // for convenience, source code will be read into a string
        // ref: https://www.zhihu.com/question/426117879/answer/2618969836?utm_source=qq&utm_medium=social&utm_oi=867698515231522816
        std::ostringstream out;         // 辅助流
        if (strcmp(argv[1], "-") == 0) {  // "-" is exactly the string in argv[1]
            out << std::cin.rdbuf();
        } else {   // argv[1] is the name of test file
            std::ifstream fin(argv[1]); // 以流的形式打开源代码文件
            out << fin.rdbuf();         // 将之注入out
            fin.close();                // 关闭源代码文件
        }
        input_string = out.str(); // with the help of out, source code is read into a string
    } else {
        std::cout << "Usage: ./code input_source_code missing.\n";
        return 1;
    }


    // 词法分析.
    Lexer lexer;
//    // 打印所有的tokens
//    for (auto &currentToken: lexer.constructTokenStream()) {
//        fprintf(stderr, "%s : type %d\n", currentToken.lexeme.c_str(),
//                currentToken.type);
//    }
//    return 0;

    // 语法分析.
    Parser parser(lexer);
    std::shared_ptr<AST_Node> tree = parser.parse();

    // 语义分析.
    SemanticAnalyzer semantic_analyzer;
    semantic_analyzer.analyze(*tree);

    // X86汇编代码生成.
    X86_CodeGenerator X86_code_generator;
    X86_code_generator.X86_code_generate(*tree);

    // LLVM IR代码生成
//    IR_CodeGenerator IR_code_generator;
//    IR_code_generator.IR_code_generate(*tree);

    return 0;
}


