#include <iostream>

// 包含fstream和sstream的目的，是为了将源代码读入到一个string(input_string，在main()中完成)，
// 做为字符流.
#include <fstream>
#include <sstream>

#include <cstring>
#include <vector>

// 使用智能指针需要.
#include <memory>

// 构建符号表需要.
#include <map>


// 运算符、表达式、变量等的基础类型(即，base-type, 或builtin-type).
typedef enum base_type {
    base_type_int,
    base_type_float,
    base_type_bool,
    base_type_void,
} base_type;


// main()函数负责把源代码读入input_string，即字符流，做为Lexer的输入.
static std::string input_string;

//===----------------------------------------------------------------------===//
// Lexer
//===----------------------------------------------------------------------===//

// 用不同的整数值表示token的不同类型.
enum TOKEN_TYPE {
    TK_INT_LITERAL = -1,            // [0-9]+
    TK_FLOAT_LITERAL = -2,          // [0-9]+.[0-9]+
    TK_BOOL_LITERAL = -3,
    TK_IDENTIFIER = -4,             // identifier
    // keywords
    TK_INT = -5,                    // int
    TK_FLOAT = -6,                  // float
    TK_BOOL = -7,                   // bool
    TK_VOID = -17,                  // void
    TK_TRUE = -8,                   // true
    TK_FALSE = -9,                  // false
    TK_RETURN = -16,                // return

    TK_PLUS = int('+'),             // addition or unary plus
    TK_MINUS = int('-'),            // subtraction or unary negative
    TK_MUL = int('*'),              // multiplication
    TK_DIV = int('/'),              // division
    TK_NOT = int('!'),              // not
    TK_LPAREN = int('('),
    TK_RPAREN = int(')'),
    TK_LBRACE = int('{'),
    TK_RBRACE = int('}'),
    TK_ASSIGN = int('='),
    TK_COMMA = int(','),
    TK_SEMICOLON = int(';'),

    // comparison operators
    TK_AND = -10,      // "&&"
    TK_OR = -11,       // "||"
    TK_EQ = -12,       // equal  "=="
    TK_NE = -13,       // not equal "!="
    TK_LT = int('<'),  // less than
    TK_LE = -14,       // less than or equal to "<="
    TK_GT = int('>'),  // greater than
    TK_GE = -15,       // greater than or equal to ">="

    // special tokens
    TK_EOF = 0, // 字符流（亦即源代码）的结尾.
};

// TOKEN结构体.
struct TOKEN {
    int type;
    std::string lexeme;
};


class Lexer {
private:
    // 字符流指针.
    long pos = 0;

    // getChar()和put_backChar()是两个用于移动字符流指针的辅助函数.
    char getChar() { return input_string[pos++]; }

    void put_backChar() { pos--; };

    TOKEN getNextToken();

public:
    // token流，词法分析结果.
    std::vector<TOKEN> tokenList;

    std::vector<TOKEN> constructTokenStream();
};

// getNextToken - 从字符流(input_string)返回下一个token.
TOKEN Lexer::getNextToken() {
    char CurrentChar = getChar();
    // skip any whitespace.
    while (isspace(CurrentChar)) {
        CurrentChar = getChar();
    }

    // int or float number
    if (isdigit(CurrentChar) || CurrentChar == '.') { // Number
        std::string NumberString;

        if (CurrentChar == '.') { // Float Number: .[0-9]+
            do {
                NumberString += CurrentChar;
                CurrentChar = getChar();
            } while (isdigit(CurrentChar));
            put_backChar();
            return TOKEN{TK_FLOAT_LITERAL, NumberString};
        } else {
            do { // Start of Number: [0-9]+
                NumberString += CurrentChar;
                CurrentChar = getChar();
            } while (isdigit(CurrentChar));

            if (CurrentChar == '.') { // Float Number: [0-9]+.[0-9]+
                do {
                    NumberString += CurrentChar;
                    CurrentChar = getChar();
                } while (isdigit(CurrentChar));
                put_backChar();
                return TOKEN{TK_FLOAT_LITERAL, NumberString};
            } else { // Integer : [0-9]+
                put_backChar();
                return TOKEN{TK_INT_LITERAL, NumberString};
            }
        }
    }

    if (isalpha(CurrentChar) || CurrentChar == '_') {
        std::string IdentifierString;
        do {
            IdentifierString += CurrentChar;
            CurrentChar = getChar();
        } while (isalnum(CurrentChar) || CurrentChar == '_');
        put_backChar();

        // if keywords
        if (IdentifierString == "int")
            return TOKEN{TK_INT, IdentifierString};
        if (IdentifierString == "float")
            return TOKEN{TK_FLOAT, IdentifierString};
        if (IdentifierString == "bool")
            return TOKEN{TK_BOOL, IdentifierString};
        if (IdentifierString == "void")
            return TOKEN{TK_VOID, IdentifierString};
        if (IdentifierString == "true" || IdentifierString == "false")
            return TOKEN{TK_BOOL_LITERAL, IdentifierString};
        if (IdentifierString == "return")
            return TOKEN{TK_RETURN, IdentifierString};

        // otherwise, identifiers
        return TOKEN{TK_IDENTIFIER, IdentifierString};
    }


    switch (CurrentChar) {
        case '+': {
            std::string s(1, CurrentChar);
            TOKEN token = TOKEN{TK_PLUS, s};
            return token;
        }
        case '-': {
            std::string s(1, CurrentChar);
            TOKEN token = TOKEN{TK_MINUS, s};
            return token;
        }
        case '*': {
            std::string s(1, CurrentChar);
            TOKEN token = TOKEN{TK_MUL, s};
            return token;
        }
        case '/': {
            std::string s(1, CurrentChar);
            TOKEN token = TOKEN{TK_DIV, s};
            return token;
        }
        case '(': {
            std::string s(1, CurrentChar);
            TOKEN token = TOKEN{TK_LPAREN, s};
            return token;
        }
        case ')': {
            std::string s(1, CurrentChar);
            TOKEN token = TOKEN{TK_RPAREN, s};
            return token;
        }
        case '{': {
            std::string s(1, CurrentChar);
            TOKEN token = TOKEN{TK_LBRACE, s};
            return token;
        }
        case '}': {
            std::string s(1, CurrentChar);
            TOKEN token = TOKEN{TK_RBRACE, s};
            return token;
        }
        case '&': {
            std::string s(1, CurrentChar);
            CurrentChar = getChar();
            if (CurrentChar == '&') {
                s += CurrentChar;
                TOKEN token = TOKEN{TK_AND, s};
                return token;
            } else CurrentChar = s.front();
            break;
        }
        case '|': {
            std::string s(1, CurrentChar);
            CurrentChar = getChar();
            if (CurrentChar == '|') {
                s += CurrentChar;
                TOKEN token = TOKEN{TK_OR, s};
                return token;
            } else CurrentChar = s.front();
            break;
        }
        case '=': {
            std::string s(1, CurrentChar);
            CurrentChar = getChar();
            if (CurrentChar == '=') {
                s += CurrentChar;
                TOKEN token = TOKEN{TK_EQ, s};
                return token;
            } else put_backChar();
            TOKEN token = TOKEN{TK_ASSIGN, s};
            return token;
        }
        case '!': {
            std::string s(1, CurrentChar);
            CurrentChar = getChar();
            if (CurrentChar == '=') {
                s += CurrentChar;
                TOKEN token = TOKEN{TK_NE, s};
                return token;
            } else put_backChar();
            TOKEN token = TOKEN{TK_NOT, s};
            return token;
        }
        case '<': {
            std::string s(1, CurrentChar);
            CurrentChar = getChar();
            if (CurrentChar == '=') {
                s += CurrentChar;
                TOKEN token = TOKEN{TK_LE, s};
                return token;
            } else put_backChar();
            TOKEN token = TOKEN{TK_LT, s};
            return token;
        }
        case '>': {
            std::string s(1, CurrentChar);
            CurrentChar = getChar();
            if (CurrentChar == '=') {
                s += CurrentChar;
                TOKEN token = TOKEN{TK_GE, s};
                return token;
            } else put_backChar();
            TOKEN token = TOKEN{TK_GT, s};
            return token;
        }
        case ',': {
            std::string s(1, CurrentChar);
            TOKEN token = TOKEN{TK_COMMA, s};
            return token;
        }
        case ';': {
            std::string s(1, CurrentChar);
            TOKEN token = TOKEN{TK_SEMICOLON, s};
            return token;
        }
        default:
            break;
    }
    // check for end of source code, and signal it by EOF.
    if (input_string.length() <= pos) {
        return TOKEN{TK_EOF, "EOF"};
    }
    fprintf(stderr, "%c is an invalid token\n", CurrentChar);
    exit(1);
}

// get the list of tokens
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
    std::string category;  // such as: function, variable, formal parameter, etc..
    base_type type;

    Symbol(std::string sym_name, std::string sym_cat, base_type sym_type) :
            name(std::move(sym_name)), category(sym_cat), type(sym_type) {}

    // 为了使用std::dynamic_pointer_cast，此virtual析构函数不可少
    virtual ~Symbol() = default;
};

class Variable_Symbol : public Symbol {
public:
    int offset;

    Variable_Symbol(std::string sym_name, std::string sym_cat, base_type sym_type, int sym_offset) :
            Symbol(std::move(sym_name), std::move(sym_cat), sym_type), offset(sym_offset) {}
};

class Function_Symbol : public Symbol {
public:
    Function_Symbol(std::string sym_name, std::string sym_cat, base_type sym_type) :
            Symbol(std::move(sym_name), std::move(sym_cat), sym_type) {}
};

class ScopedSymbolTable {
public:
    // map是基于红黑树的数据结构，也可以用基于哈希表的unordered_map
    std::map<std::string, std::shared_ptr<Symbol>> symbols;
    int level;
    std::shared_ptr<ScopedSymbolTable> enclosing_scope;

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


//===----------------------------------------------------------------------===//
// AST nodes and abstract visitor
//===----------------------------------------------------------------------===//

// Visitor类会引用各个AST结点类，故在此声明，其定义紧跟Visitor类定义之后
class IntLiteral_AST_Node;

class FloatLiteral_AST_Node;

class BoolLiteral_AST_Node;

class Assignment_AST_Node;

class Return_AST_Node;

class BinaryOperator_AST_Node;

class UnaryOperator_AST_Node;

class Variable_AST_Node;

class Type_AST_Node;

class Block_AST_Node;

class SingleVariableDeclaration_AST_Node;

class VariableDeclarations_AST_Node;

class Parameter_AST_Node;

class FunctionDeclaration_AST_Node;

class FunctionCall_AST_Node;

class Program_AST_Node;

// Visitor类定义. (Visitor类是SemanticVisitor和CodeGenerator的基类，这两个派生类的定义在各个AST_Node派生类的定义之后)
class Visitor {
public:
    virtual void visit(IntLiteral_AST_Node &node) = 0;

    virtual void visit(FloatLiteral_AST_Node &node) = 0;

    virtual void visit(BoolLiteral_AST_Node &node) = 0;

    virtual void visit(Assignment_AST_Node &node) = 0;

    virtual void visit(Return_AST_Node &node) = 0;

    virtual void visit(BinaryOperator_AST_Node &node) = 0;

    virtual void visit(UnaryOperator_AST_Node &node) = 0;

    virtual void visit(Variable_AST_Node &node) = 0;

    virtual void visit(Type_AST_Node &node) = 0;

    virtual void visit(Block_AST_Node &node) = 0;

    virtual void visit(SingleVariableDeclaration_AST_Node &node) = 0;

    virtual void visit(Parameter_AST_Node &node) = 0;

    virtual void visit(VariableDeclarations_AST_Node &node) = 0;

    virtual void visit(FunctionDeclaration_AST_Node &node) = 0;

    virtual void visit(FunctionCall_AST_Node &node) = 0;

    virtual void visit(Program_AST_Node &node) = 0;
};

// 注意：各种AST_Node的定义中，若出现TOKEN是不太合适的
/// AST_Node - 各个AST_Node派生类的基类.
class AST_Node {
public:
    // 为某些结点(比如变量、表达式等对应的结点)设置数据类型属性，将在Visitor访问该结点时的相关处理带来方便.
    // 当然，这些属性信息也可以保存在符号表中，至于如何选择看喜好.
    base_type b_type = base_type_void;

    virtual ~AST_Node() = default;

    virtual void accept(Visitor &v) = 0;
};

/// IntLiteral_AST_Node - Class for integer literals like 1, 2, 10
class IntLiteral_AST_Node : public AST_Node {
public:
    std::string literal;

    explicit IntLiteral_AST_Node(TOKEN &tok) : literal(tok.lexeme) {}

    void accept(Visitor &v) override { v.visit(*this); }
};

/// FloatLiteral_AST_Node - Class for float literals like 1.1, 2.00
class FloatLiteral_AST_Node : public AST_Node {
public:
    std::string literal;

    explicit FloatLiteral_AST_Node(TOKEN &tok) : literal(tok.lexeme) {}

    void accept(Visitor &v) override { v.visit(*this); }
};

/// BoolLiteral_AST_Node - Class for boolean literals: true or false
class BoolLiteral_AST_Node : public AST_Node {
public:
    std::string literal;

    explicit BoolLiteral_AST_Node(TOKEN &tok) : literal(tok.lexeme) {}

    void accept(Visitor &v) override { v.visit(*this); }
};

/// 赋值语句对应的结点
class Assignment_AST_Node : public AST_Node {
public:
    std::shared_ptr<AST_Node> left;
    std::string operation; // 此处必然是“=”
    std::shared_ptr<AST_Node> right;

    Assignment_AST_Node(std::shared_ptr<AST_Node> leftvalue, std::string op, std::shared_ptr<AST_Node> rightvalue)
            : left(std::move(leftvalue)), operation(std::move(op)), right(std::move(rightvalue)) {}

    void accept(Visitor &v) override { v.visit(*this); }
};

/// return语句对应的结点
class Return_AST_Node : public AST_Node {
public:
    std::shared_ptr<AST_Node> expression;
    std::string functionName;

    Return_AST_Node(std::shared_ptr<AST_Node> expr) : //, std::string funcName) :
            expression(std::move(expr)) {}

    void accept(Visitor &v) override { v.visit(*this); }
};

// 二元运算对应的结点
class BinaryOperator_AST_Node : public AST_Node {
public:
    std::shared_ptr<AST_Node> left;
    std::string operation;
    std::shared_ptr<AST_Node> right;

    BinaryOperator_AST_Node(std::shared_ptr<AST_Node> Left, std::string op, std::shared_ptr<AST_Node> Right)
            : left(std::move(Left)), operation(std::move(op)), right(std::move(Right)) {}

    void accept(Visitor &v) override { v.visit(*this); }
};

/// 单个变量声明(或同时进行初始化)对应的结点
class SingleVariableDeclaration_AST_Node : public AST_Node {
public:
    std::shared_ptr<AST_Node> type; // type node
    std::shared_ptr<AST_Node> var;  // variable node
    std::shared_ptr<AST_Node> init; // assignment node
    SingleVariableDeclaration_AST_Node(std::shared_ptr<AST_Node> typeNode, std::shared_ptr<AST_Node> varNode,
                                       std::shared_ptr<AST_Node> assignNode)
            : type(std::move(typeNode)), var(std::move(varNode)), init(std::move(assignNode)) {}

    void accept(Visitor &v) override { v.visit(*this); }
};

/// 变量声明语句对应的结点，注意：一个变量声明语句可能同时声明多个变量
class VariableDeclarations_AST_Node : public AST_Node {
public:
    std::vector<std::shared_ptr<AST_Node>> varDeclarations;

    VariableDeclarations_AST_Node(std::vector<std::shared_ptr<AST_Node>> newVarDeclarations)
            : varDeclarations(std::move(newVarDeclarations)) {}

    void accept(Visitor &v) override { v.visit(*this); }
};

/// 一元运算对应的结点
class UnaryOperator_AST_Node : public AST_Node {
public:
    std::string operation;
    std::shared_ptr<AST_Node> right;

    UnaryOperator_AST_Node(std::string op, std::shared_ptr<AST_Node> Right)
            : operation(std::move(op)), right(std::move(Right)) {}

    void accept(Visitor &v) override { v.visit(*this); }
};

/// 变量声明类型或函数返回值类型对应的结点
class Type_AST_Node : public AST_Node {
public:
    std::string type_name;

    explicit Type_AST_Node(std::string t) : type_name(std::move(t)) {}

    void accept(Visitor &v) override { v.visit(*this); }
};

/// 变量结点，变量声明和每次变量引用时都对应单独的变量结点，但对应的符号表项只有一个
class Variable_AST_Node : public AST_Node {
public:
    std::string var_name;
    std::shared_ptr<Variable_Symbol> symbol;

    Variable_AST_Node(std::string var) : var_name(std::move(var)) {}

    void accept(Visitor &v) override { v.visit(*this); }
};

class Parameter_AST_Node : public AST_Node {
public:
    std::shared_ptr<AST_Node> parameterType;  // type node
    std::shared_ptr<AST_Node> parameterVar;   // var node
    std::shared_ptr<Variable_Symbol> symbol;

    Parameter_AST_Node(std::shared_ptr<AST_Node> typeNode, std::shared_ptr<AST_Node> varNode)
            : parameterType(std::move(typeNode)), parameterVar(std::move(varNode)) {}

    void accept(Visitor &v) override { v.visit(*this); }
};

class Block_AST_Node : public AST_Node {
public:
    std::vector<std::shared_ptr<AST_Node>> statements;

    Block_AST_Node(std::vector<std::shared_ptr<AST_Node>> newStatements)
            : statements(std::move(newStatements)) {}

    void accept(Visitor &v) override { v.visit(*this); }
};

class FunctionDeclaration_AST_Node : public AST_Node {
public:
    std::shared_ptr<AST_Node> funcType;  // type node
    std::string funcName;  // function name
    std::vector<std::shared_ptr<AST_Node>> formalParams; // formal parameters
    std::shared_ptr<AST_Node> funcBlock;  // function body

    int offset = -1;

    FunctionDeclaration_AST_Node(std::shared_ptr<AST_Node> t, std::string n, std::vector<std::shared_ptr<AST_Node>> &&p,
                                 std::shared_ptr<AST_Node> b) :
            funcType(std::move(t)), funcName(std::move(n)), formalParams(p), funcBlock(std::move(b)) {}

    void accept(Visitor &v) override { v.visit(*this); }
};

class FunctionCall_AST_Node : public AST_Node {
public:
    std::string funcName;  // function name
    std::vector<std::shared_ptr<AST_Node>> arguments; // 实参

    FunctionCall_AST_Node(std::string name, std::vector<std::shared_ptr<AST_Node>> args) :
            funcName(std::move(name)), arguments(std::move(args)) {}

    void accept(Visitor &v) override { v.visit(*this); }
};

class Program_AST_Node : public AST_Node {
public:
    std::vector<std::shared_ptr<VariableDeclarations_AST_Node>> varDeclarationsList;
    std::vector<std::shared_ptr<FunctionDeclaration_AST_Node>> funcDeclarationList;

    Program_AST_Node(std::vector<std::shared_ptr<VariableDeclarations_AST_Node>> newVarDeclList,
                     std::vector<std::shared_ptr<FunctionDeclaration_AST_Node>> newFuncDeclList) :
            varDeclarationsList(std::move(newVarDeclList)), funcDeclarationList(std::move(newFuncDeclList)) {}

    void accept(Visitor &v) override { v.visit(*this); }
};
//===----------------------------------------------------------------------===//
// Parser
//===----------------------------------------------------------------------===//

// 如果想在.data段列出所有float常量，则用之，否则删掉就行。
std::vector<std::shared_ptr<FloatLiteral_AST_Node>> floats;

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

// primary := int_literal | float_literal | bool_literal | "(" expression ")" | identifier func_args?
// func_args = "(" (expression ("," expression)*)? ")"
std::shared_ptr<AST_Node> Parser::primary() {
    // int_literal
    if (CurrentToken.type == TK_INT_LITERAL) {
        auto node = std::make_shared<IntLiteral_AST_Node>(CurrentToken);
        eatCurrentToken();
        return node;
    }

    // float_literal
    if (CurrentToken.type == TK_FLOAT_LITERAL) {
        auto node = std::make_shared<FloatLiteral_AST_Node>(CurrentToken);
        // 如果想在.data段列出所有float常量，则用之，否则删掉就行。
        floats.push_back(std::make_shared<FloatLiteral_AST_Node>(CurrentToken));
        eatCurrentToken();
        return node;
    }

    // bool_literal
    if (CurrentToken.type == TK_BOOL_LITERAL) {
        auto node = std::make_shared<BoolLiteral_AST_Node>(CurrentToken);
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
    // func_args = "(" (expression ("," expression)*)? ")"
    if (CurrentToken.type == TK_IDENTIFIER) {
        std::string name = CurrentToken.lexeme;  // name of variable or function
        eatCurrentToken();
        // function call
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
        // Variable
        return std::make_shared<Variable_AST_Node>(name);
    }

    // error!
    {
        fprintf(stderr, "error!:  %s\n", CurrentToken.lexeme.c_str());
        exit(1);
    }

}

// unary :=	(“+” | “-” | “!”) unary | primary
std::shared_ptr<AST_Node> Parser::unary() {
    if (CurrentToken.type == TK_PLUS ||
        CurrentToken.type == TK_MINUS ||
        CurrentToken.type == TK_NOT) {
        TOKEN tok = CurrentToken;
        eatCurrentToken();
        return std::make_shared<UnaryOperator_AST_Node>(tok.lexeme, std::move(unary()));
    } else return primary();
}


// mul_div := unary ("*" unary | "/" unary)*
std::shared_ptr<AST_Node> Parser::mul_div() {
    std::shared_ptr<AST_Node> node = unary();
    while (true) {
        if (CurrentToken.type == TK_MUL || CurrentToken.type == TK_DIV) {
            TOKEN op_token = CurrentToken;
            eatCurrentToken();
            auto left = std::move(node);
            node = std::make_shared<BinaryOperator_AST_Node>(std::move(left), std::move(op_token.lexeme),
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
            node = std::make_shared<BinaryOperator_AST_Node>(std::move(left), std::move(op_token.lexeme),
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
            node = std::make_shared<BinaryOperator_AST_Node>(std::move(left), std::move(op_token.lexeme),
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
            node = std::make_shared<BinaryOperator_AST_Node>(std::move(left), std::move(op_token.lexeme),
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
// type-suffix  :=  ϵ | ("[" expression "]")? ("[" expression "]")? ";"
std::shared_ptr<AST_Node> Parser::declarator() {
    if (CurrentToken.type == TK_IDENTIFIER) {
        auto node = std::make_shared<Variable_AST_Node>(std::move(CurrentToken.lexeme));
        eatCurrentToken();
        return node;
    }
    return nullptr;
    // 暂时不考虑type-suffix，等处理数组的时候再考虑
}

// type_specification := "int" | "float" | "bool" | "void"
std::shared_ptr<AST_Node> Parser::type_specification() {
    if (CurrentToken.type == TK_INT) {
        eatCurrentToken();
        return std::make_shared<Type_AST_Node>("int");
    }
    if (CurrentToken.type == TK_FLOAT) {
        eatCurrentToken();
        return std::make_shared<Type_AST_Node>("float");
    }
    if (CurrentToken.type == TK_BOOL) {
        eatCurrentToken();
        return std::make_shared<Type_AST_Node>("bool");
    }
    if (CurrentToken.type == TK_VOID) {
        eatCurrentToken();
        return std::make_shared<Type_AST_Node>("void");
    }
    fprintf(stderr, "%s is not a type\n", (CurrentToken.lexeme).c_str());
    return nullptr;
}

// variable_declaration	 :=  type_specification declarator ("=" expression)? ("," declarator ("=" expression)?)* ";"
std::shared_ptr<AST_Node> Parser::variable_declaration() {
    std::vector<std::shared_ptr<AST_Node>> VarDeclarations;
    std::shared_ptr<Type_AST_Node> typeNode = std::dynamic_pointer_cast<Type_AST_Node>(type_specification());
    while (CurrentToken.type != TK_SEMICOLON) {
        auto variableNode = declarator();
        // 学习chibicc的做法，将变量初始化视为一个(或多个，对数组)赋值语句
        std::shared_ptr<AST_Node> initNode = nullptr;
        if (CurrentToken.type == TK_ASSIGN) {
            TOKEN assignToken = CurrentToken;
            eatCurrentToken();
            initNode = std::make_shared<Assignment_AST_Node>(variableNode, std::move(assignToken.lexeme),
                                                             std::move(expression()));
        }
        auto node = std::make_shared<SingleVariableDeclaration_AST_Node>(typeNode, variableNode, std::move(initNode));
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
std::shared_ptr<AST_Node> Parser::statement() {
    if (CurrentToken.type == TK_LBRACE) return block();

    if (CurrentToken.type == TK_INT ||
        CurrentToken.type == TK_FLOAT ||
        CurrentToken.type == TK_BOOL)
        return variable_declaration();

    if (CurrentToken.type == TK_RETURN) {
        eatCurrentToken();  // eat "return"
        return std::make_shared<Return_AST_Node>(std::move(expression_statement()));
    }

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
    std::shared_ptr<Type_AST_Node> typeNode = std::dynamic_pointer_cast<Type_AST_Node>(type_specification());
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
    std::shared_ptr<Type_AST_Node> typeNode = std::dynamic_pointer_cast<Type_AST_Node>(type_specification());

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
            CurrentToken.type == TK_BOOL || CurrentToken.type == TK_VOID) {
            eatCurrentToken();
            if (CurrentToken.type == TK_IDENTIFIER) {
                eatCurrentToken();
                if (CurrentToken.type == TK_LPAREN) {
                    put_backToken();
                    put_backToken();
                    auto newFunction = std::dynamic_pointer_cast<FunctionDeclaration_AST_Node>(function_declaration());
                    functionDeclarationList.push_back(newFunction);
                }
            } else {
                put_backToken();
                put_backToken();
                auto newVariableDeclarations = std::dynamic_pointer_cast<VariableDeclarations_AST_Node>(
                        variable_declaration());
                variableDeclarationsList.push_back(newVariableDeclarations);
            }
        }
    }
    return std::make_shared<Program_AST_Node>(std::move(variableDeclarationsList), std::move(functionDeclarationList));
}


/*
 * program = variable-declarations list + function-declaration list
 * 1. 为什么第一个list的成员是variable-declarations呢？这是因为每个varialbe-declaration语句都有可能包含多个变量声明，如：
 *                            int a,b=5,c;
 * 每个变量的声明对应一个single-variable-declaration，它们组合成variable-declarations。因此，
 * variable-declarations list又可以理解为“变量声明语句”的list。
 * 2. 因为函数只能一个一个的声明，就简洁好理解了。
 */

/******************** CFG (c-like)************************************
 *
 * program  :=  (variable_declaration | function_declaration)*
 * function_declaration  :=  type_specification identifier "(" formal_parameters? ")" block
 * formal_parameters  :=  formal_parameter ("," formal_parameter)*
 * formal_parameter  :=  type_specification identifier
 * block  :=  "{" statement* "}"
 * statement  :=  variable_declaration
 *                  | expression_statement
 *                  | "return" expression-statement
 *                  | block
 * variable_declaration  :=  type_specification declarator ("=" expression)? ("," declarator ("=" expression)?)* ";"
 * type_specification  :=  "int" | "float" | "bool" | "void"
 * declarator  :=  identifier type-suffix
 * expression_statement  :=  expression? ";"
 * expression  :=  equality ("=" expression)?
 * equality  :=  relational ("==" relational | "!=" relational)*
 * relational  :=  add_sub ("<" add_sub | "<=" add_sub | ">" add_sub | ">=" add_sub)*
 * add_sub  :=  mul_div ("+" mul_div | "-" mul_div)*
 * mul_div  :=  unary ("*" unary | "/" unary)*
 * unary  :=  ("+" | "-" | "!") unary | primary
 * primary  :=  int_literal | float_literal | bool_literal | "(" expression ")" | identifier func_args?
 *
********************/
std::shared_ptr<AST_Node> Parser::parse() {
    return program();
}

//===----------------------------------------------------------------------===//
// Semantic analyzer derived from Visitor
//===----------------------------------------------------------------------===//


class SemanticAnalyzer : public Visitor {
public:
    std::shared_ptr<ScopedSymbolTable> currentScope;
    int offsetSum;

    void analyze(AST_Node &tree) {
        tree.accept(*this);
    }

    void visit(IntLiteral_AST_Node &node) override {
        node.b_type = base_type_int;
    }

    void visit(FloatLiteral_AST_Node &node) override {
        node.b_type = base_type_float;
    }

    void visit(BoolLiteral_AST_Node &node) override {
        node.b_type = base_type_bool;
    }

    void visit(Return_AST_Node &node) override {
        // 确定与return语句配对的函数名
        std::shared_ptr<ScopedSymbolTable> tempScope = currentScope;
        while (tempScope != nullptr) {
            for (auto iter = tempScope->symbols.rbegin(); iter != tempScope->symbols.rend(); iter++)
                if (iter->second->category == "function") {
                    node.functionName = iter->second->name;
                    tempScope = nullptr; // 既然已经找到函数名，强行赋空tempScope，为退出while做准备
                    break;
                }
            if (tempScope != nullptr) tempScope = tempScope->enclosing_scope;
        }


        // case: "return ;"
        if (node.expression == nullptr) node.b_type = base_type_void;
        else { // other cases:
            node.expression->accept(*this);
            node.b_type = node.expression->b_type;
        }
        // 检查return语句的类型是否与函数声明开头的类型一致，不一致的话，则需要类型转换
        // (可能还需要为用户提供warning信息)
        // 类型转换也可以放在visit BinaryOperation的时候进行
        std::shared_ptr<Symbol> functionSymbol = currentScope->lookup(node.functionName, false);
        if (functionSymbol == nullptr) {
            fprintf(stderr, "something wrong!\n");
            exit(1);
        }
        if (node.b_type == base_type_void && functionSymbol->type != base_type_void) {
            fprintf(stderr, "the return value should not be void!\n");
            exit(1);
        }
    }

    void visit(Assignment_AST_Node &node) override {
        node.left->accept(*this);
        node.right->accept(*this);
        node.b_type = node.left->b_type;
    }

    void visit(BinaryOperator_AST_Node &node) override {
        node.left->accept(*this);
        node.right->accept(*this);
        // 检查运算数是否兼容
        base_type left_base_type = node.left->b_type;
        base_type right_base_type = node.right->b_type;
        // 若兼容
        node.b_type = (left_base_type > right_base_type) ? left_base_type : right_base_type;
    }

    void visit(UnaryOperator_AST_Node &node) override {
        node.right->accept(*this);
        node.b_type = node.right->b_type;
    }

    void visit(Type_AST_Node &node) override {
        // 确定变量或函数返回值的基本类型
        if (node.type_name == "int") node.b_type = base_type_int;
        else if (node.type_name == "float") node.b_type = base_type_float;
        else if (node.type_name == "bool") node.b_type = base_type_bool;
        else if (node.type_name == "void") node.b_type = base_type_void;
    }

    void visit(SingleVariableDeclaration_AST_Node &node) override {
        // 类型
        node.type->accept(*this);
        std::shared_ptr<Type_AST_Node> typeNode = std::dynamic_pointer_cast<Type_AST_Node>(node.type);
        std::shared_ptr<Variable_AST_Node> varNode = std::dynamic_pointer_cast<Variable_AST_Node>(node.var);

        std::shared_ptr<Symbol> varSymbol = currentScope->lookup(varNode->var_name, true);
        if (varSymbol == nullptr) {
            offsetSum += 8;
            varSymbol = std::make_shared<Variable_Symbol>(varNode->var_name, "variable", typeNode->b_type, -offsetSum);
            currentScope->insert(varSymbol);
            varNode->symbol = std::dynamic_pointer_cast<Variable_Symbol>(varSymbol);
        } else {
            fprintf(stderr, "symbol info of is wrong\n");
            exit(1);
        }
        // 变量
        node.var->accept(*this);
        // 初始化
        if (node.init != nullptr) node.init->accept(*this);
    }

    void visit(VariableDeclarations_AST_Node &node) override {
        if (node.varDeclarations.size() != 0)
            for (auto &n: node.varDeclarations) n->accept(*this);
    }

    void visit(Variable_AST_Node &node) override {
        std::shared_ptr<Symbol> varSymbol = currentScope->lookup(node.var_name, true);
        if (varSymbol == nullptr) {
            fprintf(stderr, "something wrong!!\n");
            exit(1);
        }
        node.b_type = varSymbol->type;
        if (node.symbol == nullptr) node.symbol = std::dynamic_pointer_cast<Variable_Symbol>(varSymbol);
    }

    void visit(Block_AST_Node &node) override {
        if (node.statements.size() != 0)
            for (auto &n: node.statements) n->accept(*this);
    }

    void visit(FunctionCall_AST_Node &node) override {
        std::shared_ptr<Symbol> functionSymbol = currentScope->lookup(node.funcName, false);
        if (functionSymbol == nullptr) {
            fprintf(stderr, "something wrong!!!\n");
            exit(1);
        }
        node.b_type = functionSymbol->type;
        // 目的是为了确定每一个实参node的base_type
        for (auto &eachArg: node.arguments) eachArg->accept(*this);
    }

    void visit(Parameter_AST_Node &node) override {
        // 类型
        node.parameterType->accept(*this);
        node.b_type = node.parameterType->b_type;
        std::shared_ptr<Type_AST_Node> paramTypeNode = std::dynamic_pointer_cast<Type_AST_Node>(node.parameterType);

        std::shared_ptr<Variable_AST_Node> paramVarNode = std::dynamic_pointer_cast<Variable_AST_Node>(
                node.parameterVar);

        offsetSum += 8;
        auto sym = std::make_shared<Variable_Symbol>(paramVarNode->var_name, "parameter", paramTypeNode->b_type,
                                                     -offsetSum);
        currentScope->insert(sym);
        node.symbol = sym;
        //node.accept(*this);
    }

    void visit(FunctionDeclaration_AST_Node &node) override {
        offsetSum = 0;  // 为每个函数初始化offsetSum

        // visit类型结点
        node.funcType->accept(*this);
        // 构建function symbol
        auto sym = std::make_shared<Function_Symbol>(node.funcName, "function", node.funcType->b_type);
        // 将function symbol插入gloabal scope符号表
        currentScope->insert(sym);

        // 创建function scope symbol_table
        std::map<std::string, std::shared_ptr<Symbol>> symbols;
        auto scope = std::make_shared<ScopedSymbolTable>(std::move(symbols), currentScope->level + 1, nullptr);
        scope->enclosing_scope = currentScope;
        currentScope = scope;

        // visit形参列表
        for (auto &eachParam: node.formalParams)
            eachParam->accept(*this);

        // visit函数体
        node.funcBlock->accept(*this);

        node.offset = offsetSum; // 此值即是代码生成阶段的stack_size
    }

    void visit(Program_AST_Node &node) override {
        // 创建global scope (program) symbol_table
        std::map<std::string, std::shared_ptr<Symbol>> symbols;
        auto globalScope = std::make_shared<ScopedSymbolTable>(std::move(symbols), 0, nullptr);
        currentScope = std::move(globalScope);

        if (node.varDeclarationsList.size() != 0)
            for (auto &var: node.varDeclarationsList) var->accept(*this);
        if (node.funcDeclarationList.size() != 0)
            for (auto &func: node.funcDeclarationList) func->accept(*this);
    }
};

//===----------------------------------------------------------------------===//
// Code generator derived from Visitor
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

class CodeGenerator : public Visitor {
private:
    // Round up `n` to the nearest multiple of `align`. For instance,
    // align_to(5, 8) returns 8 and align_to(11, 8) returns 16.
    int align_to(int n, int align) {
        return int((n + align - 1) / align) * align;
    };

public:
    void code_generate(AST_Node &tree);

    void visit(IntLiteral_AST_Node &node) override {
        std::cout << "  mov $" << node.literal << ", %rax\n";
    }

    void visit(FloatLiteral_AST_Node &node) override {
        union {
            double f64;
            uint64_t u64;
        } u{};
        u.f64 = strtod(node.literal.c_str(), nullptr);
        std::cout << "  mov $" << u.u64 << ", %rax   # float " << u.f64 << "\n";
        std::cout << "  movq %rax, %xmm0\n";
    }

    void visit(BoolLiteral_AST_Node &node) override {
        // like c, 1 stands for true, and 0 stands for false
        if (node.literal == "true") std::cout << "  mov $1, %rax\n";
        else std::cout << "  mov $0, %rax\n";
    }

    void visit(Return_AST_Node &node) override {
        if (node.expression != nullptr) node.expression->accept(*this);
        std::cout << "  jmp L." << node.functionName << ".return\n";
    }

    void visit(Assignment_AST_Node &node) override {
        // the left-side of "=" should be a left-value
        // so get its address in memory
        int var_offset = std::dynamic_pointer_cast<Variable_AST_Node>(node.left)->symbol->offset;
        std::cout << "  lea " << var_offset << "(%rbp), %rax\n";
        // put its address to the top of stack
        std::cout << "  push %rax\n";

        node.right->accept(*this);
        std::cout << "  pop %rdi\n";
        // 若right-side是int类型的结果，则位于%rax; 若是float类型，则位于%xxm0
        if (node.b_type == base_type_float) {
            // 若%rax参与float类型的运算，则需将之转换成float类型，并放入%xmm0
            if (node.right->b_type < base_type_float)
                std::cout << "  cvtsi2sd %rax, %xmm0\n";
            std::cout << "  movsd %xmm0, (%rdi)\n";
        } else std::cout << "  mov %rax, (%rdi)\n"; // int or bool
    }

    void visit(BinaryOperator_AST_Node &node) override {
        // float类型
        if (node.b_type == base_type_float) {
            node.right->accept(*this);
            if (node.right->b_type < base_type_float) { //int to float
                std::cout << "  cvtsi2sd %rax, %xmm0" << std::endl;
            }
            // push float
            std::cout << "  sub $8, %rsp" << std::endl;
            std::cout << "  movsd %xmm0, (%rsp)" << std::endl;

            node.left->accept(*this);
            if (node.left->b_type < base_type_float) { //int to float
                std::cout << "  cvtsi2sd %rax, %xmm0" << std::endl;
            }
            // pop float
            std::cout << "  movsd (%rsp), %xmm1" << std::endl;
            std::cout << "  add $8, %rsp" << std::endl;

            if (node.operation == "+") std::cout << "  addsd %xmm1, %xmm0" << std::endl;
            else if (node.operation == "-") std::cout << "  subsd %xmm1, %xmm0" << std::endl;
            else if (node.operation == "*") std::cout << "  mulsd %xmm1, %xmm0" << std::endl;
            else if (node.operation == "/") std::cout << "  divsd %xmm1, %xmm0" << std::endl;
            return;
        }

        // int类型
        // first right,
        node.right->accept(*this);
        std::cout << "  push %rax" << std::endl;
        // then left, otherwise, wrong
        node.left->accept(*this);
        std::cout << "  pop %rdi" << std::endl;
        if (node.operation == "+") std::cout << "  add %rdi, %rax" << std::endl;
        else if (node.operation == "-") std::cout << "  sub %rdi, %rax" << std::endl;
        else if (node.operation == "*") std::cout << "  imul %rdi, %rax" << std::endl;
        else if (node.operation == "/") std::cout << "  div %rdi, %rax" << std::endl;
    }

    void visit(UnaryOperator_AST_Node &node) override {
        node.right->accept(*this);
        if (node.operation == "-") {
            if (node.b_type == base_type_int)
                std::cout << "  neg %rax" << std::endl;
            else {
                // 下面四步实现将一个float取负数，即实现 %xmm0 = (-1) * %xmm0.
                // 有点啰嗦. chibicc有另一个解决方案，但是也需要四步，同样啰嗦.
                std::cout << "  mov $1, %rax" << std::endl;
                std::cout << "  neg %rax" << std::endl;
                std::cout << "  cvtsi2sd %rax, %xmm1" << std::endl;
                std::cout << "  mulsd %xmm1, %xmm0" << std::endl;
            }
        }
    }

    void visit(Type_AST_Node &node) override {}

    void visit(SingleVariableDeclaration_AST_Node &node) override {
        if (node.init != nullptr) node.init->accept(*this);
    }

    void visit(VariableDeclarations_AST_Node &node) override {
        if (node.varDeclarations.size() != 0)
            for (auto &n: node.varDeclarations) n->accept(*this);
    }

    void visit(Variable_AST_Node &node) override {
        // when variable is right-value
        // get its address in memory
        int var_offset = node.symbol->offset;
        std::cout << "  lea " << var_offset << "(%rbp), %rax\n";
        // put its value to %rax
        if (node.b_type == base_type_float) std::cout << "  movsd (%rax), %xmm0\n";
        else if (node.b_type == base_type_int || node.b_type == base_type_bool)
            std::cout << "  mov (%rax), %rax\n";
    }

    void visit(Block_AST_Node &node) override {
        if (node.statements.size() != 0)
            for (auto &n: node.statements) n->accept(*this);
    }

    void visit(FunctionCall_AST_Node &node) override {
        int j, float_num = 0, others_num = 0;
        // 实参入栈
        for (j = 0; j < node.arguments.size(); j++) {
            node.arguments[j]->accept(*this);
            if (node.arguments[j]->b_type == base_type_float) {
                // push float
                std::cout << "  sub $8, %rsp" << std::endl;
                std::cout << "  movsd %xmm0, (%rsp)" << std::endl;
                float_num += 1;
            } else {
                std::cout << "  push %rax" << std::endl;
                others_num += 1;
            }
        }
        // 实参分发转入寄存器
        for (j = node.arguments.size() - 1; j >= 0; j--) {
            if (node.arguments[j]->b_type == base_type_float) {
                float_num = float_num - 1;
                // pop float
                std::cout << "  movsd (%rsp), %" << parameter_registers_float[float_num] << std::endl;
                std::cout << "  add $8, %rsp" << std::endl;
            } else {
                others_num = others_num - 1;
                std::cout << "  pop %" << parameter_registers[others_num] << std::endl;
            }
        }

        std::cout << "  mov $0, %rax\n";
        std::cout << "  call " << node.funcName << std::endl;
    }

    void visit(Parameter_AST_Node &node) override {
        node.accept(*this);
    }

    void visit(FunctionDeclaration_AST_Node &node) override {
        // Prologue
        std::cout << "\n" << "  .text\n";
        std::cout << "  .global " << node.funcName << std::endl;
        std::cout << node.funcName << ":\n";
        std::cout << "  push %rbp\n";
        std::cout << "  mov %rsp, %rbp\n";
        int stack_size = align_to(node.offset, 16);
        if (stack_size != 0)
            std::cout << "  sub $" << stack_size << ", %rsp\n";

        // 从左往右，将参数由寄存器压入栈
        int i = 0, f = 0;
        for (auto &param: node.formalParams) {
            std::shared_ptr<Parameter_AST_Node> p = std::dynamic_pointer_cast<Parameter_AST_Node>(param);
            if (param->b_type == base_type_float)
                std::cout << "  movq %" << parameter_registers_float[f++] << ", " << p->symbol->offset << "(%rbp)\n";
            else std::cout << "  mov %" << parameter_registers[i++] << ", " << p->symbol->offset << "(%rbp)\n";
        }

        node.funcBlock->accept(*this);

        std::cout << "L." << node.funcName << ".return:\n";
        // Epilogue
        std::cout << "  mov %rbp, %rsp\n";
        std::cout << "  pop %rbp\n";
        std::cout << "  ret\n";
    }

    void visit(Program_AST_Node &node) override {
        if (node.varDeclarationsList.size() != 0)
            for (auto &n: node.varDeclarationsList) n->accept(*this);
        if (node.funcDeclarationList.size() != 0)
            for (auto &n: node.funcDeclarationList) n->accept(*this);
    }
};

void CodeGenerator::code_generate(AST_Node &tree) {
    tree.accept(*this);
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
        std::ostringstream out;  // this var is auxiliary
        if (strcmp(argv[1], "-") == 0) {  // "-" is exactly the string in argv[1]
            out << std::cin.rdbuf();
        } else {   // argv[1] is the name of test file
            std::ifstream fin(argv[1]);
            out << fin.rdbuf();
            fin.close();
        }
        input_string = out.str(); // with the help of out, source code is read into a string
    } else {
        std::cout << "Usage: ./code input_source_code missing.\n";
        return 1;
    }

    // 词法分析
    Lexer lexer;
//    // 打印所有的tokens
//    for (auto &currentToken: lexer.constructTokenStream()) {
//        fprintf(stderr, "%s : type %d\n", currentToken.lexeme.c_str(),
//                currentToken.type);
//    }

    // 语法分析 
    Parser parser(lexer);
    std::shared_ptr<AST_Node> tree = parser.parse();

    // 语义分析
    SemanticAnalyzer semantic_analyzer;
    semantic_analyzer.analyze(*tree);

    // 代码生成
    CodeGenerator code_generator;
    code_generator.code_generate(*tree);
}


