#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <queue>   //vector is also Okay, if you like
#include <memory>

//===----------------------------------------------------------------------===//
// Lexer
//===----------------------------------------------------------------------===//

//input_string = source code
static std::string input_string;
static long pos = 0;

static char getChar() {
    return input_string[pos++];
}

static void put_backChar() {
    pos--;
};

enum TOKEN_TYPE {
    TK_INT_LITERAL = -1,            // [0-9]+
    TK_FLOAT_LITERAL = -2,          // [0-9]+.[0-9]+

    // arithmetic operators
    TK_PLUS = int('+'),             // addition or unary plus
    TK_MINUS = int('-'),            // subtraction or unary negative

    // special tokens
    TK_EOF = 0, // signal end of file
};

// TOKEN struct is used to keep track of information about a token
struct TOKEN {
    int type = -100;
    std::string lexeme;
};


class Lexer {
public:
    std::deque<TOKEN> tokenList;
public:
    static TOKEN getNextToken();

    static std::deque<TOKEN> gatherAllTokens();
};

// getNextToken - Return the next token from input_string.
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

// get the list of tokens (used only for debugging)
std::deque<TOKEN> Lexer::gatherAllTokens() {
    std::deque<TOKEN> tokenList;
    TOKEN currentToken;
    do {
        currentToken = getNextToken();
        tokenList.emplace_back(currentToken);
    } while (currentToken.type != TK_EOF);
    pos = 0;   //reset to 0, to make sure that parser will work properly
    return tokenList;
}

//===----------------------------------------------------------------------===//
// AST nodes and abstract visitor
//===----------------------------------------------------------------------===//

// these node classes appear in class Visitor, and have to be declared here first
// their definitions appear shortly after
class IntAST_Node;
class FloatAST_Node;
class BinaryOperator_Node;

class Visitor {
public:
    virtual void visit(IntAST_Node& node) = 0;
    virtual void visit(FloatAST_Node& node) = 0;
    virtual void visit(BinaryOperator_Node& node) = 0;
};

/// AST_Node - Base class for all AST nodes.
class AST_Node {
public:
    virtual ~AST_Node() = default;

    virtual void accept(class Visitor &v) = 0;
};

/// IntAST_Node - Class for integer literals like 1, 2, 10,
class IntAST_Node : public AST_Node {
public:
    TOKEN token;
    explicit IntAST_Node(TOKEN tok) : token(std::move(tok)) {}

    void accept(Visitor &v) override { v.visit(*this); }
};

/// FloatAST_Node - Class for integer literals like 1, 2, 10,
class FloatAST_Node : public AST_Node {
public:
    TOKEN token;
    explicit FloatAST_Node(TOKEN tok) : token(std::move(tok)) {}

    void accept(Visitor &v) override { v.visit(*this); }
};

class BinaryOperator_Node : public AST_Node {
public:
    std::unique_ptr<AST_Node> left;
    TOKEN token;
    std::unique_ptr<AST_Node> right;

    BinaryOperator_Node(std::unique_ptr<AST_Node> LEFT, TOKEN token, std::unique_ptr<AST_Node> RIGHT)
            : left(std::move(LEFT)), token(std::move(token)), right(std::move(RIGHT)) {}

    void accept(Visitor &v) override { v.visit(*this); }
};




//===----------------------------------------------------------------------===//
// Parser
//===----------------------------------------------------------------------===//
/********************
 * expression := term ((+ | -) term)*
 *
********************/

class Parser {
    Lexer lexer;
    TOKEN CurrentToken;
public:
    explicit Parser(Lexer lex) : lexer(std::move(lex)) {
        CurrentToken = lexer.getNextToken();
    }

    std::unique_ptr<AST_Node> parse();

private:
    void eatCurrentToken();

    std::unique_ptr<AST_Node> term();
};

void Parser::eatCurrentToken() {
    CurrentToken = lexer.getNextToken();
}

std::unique_ptr<AST_Node> Parser::term() {
    std::unique_ptr<AST_Node> node = nullptr;
    switch (CurrentToken.type) {
        case TK_INT_LITERAL:   // if int, create an IntAST_Node
            node = std::make_unique<IntAST_Node>(CurrentToken);
            break;
        case TK_FLOAT_LITERAL:  // if float, create a FloatAST_Node
            node = std::make_unique<FloatAST_Node>(CurrentToken);
            break;
        default:                // otherwise, error
            fprintf(stderr, "error:  %s\n", CurrentToken.lexeme.c_str());
            exit(1);
    }
    eatCurrentToken();
    return node;
}

// expression at present
std::unique_ptr<AST_Node> Parser::parse() {
    auto node = term();
    while (true) {
        if (CurrentToken.type == TK_PLUS || CurrentToken.type == TK_MINUS) {
            TOKEN op_token = CurrentToken;
            eatCurrentToken();
            auto left = std::move(node);
            node = std::make_unique<BinaryOperator_Node>(std::move(left), op_token, std::move(term()));
            continue;
        }
        return node;
    }
}

//===----------------------------------------------------------------------===//
// Semantic analyzer derived from Visitor
//===----------------------------------------------------------------------===//
class SemanticAnalyzer : public Visitor {
public:
    void visit(IntAST_Node& node) override { }
    void visit(FloatAST_Node& node) override { }
    void visit(BinaryOperator_Node& node) override { }
};

//===----------------------------------------------------------------------===//
// Code generator derived from Visitor
//===----------------------------------------------------------------------===//
class CodeGenerator : public Visitor {
public:
    void code_generate(AST_Node& tree);

    void visit(IntAST_Node& node) override {
        std::cout << "  mov $" << strtod(node.token.lexeme.c_str(), nullptr) << ", %rax\n";
    }

    void visit(FloatAST_Node& node) override {
        fprintf(stderr, "%s : type %d\n", node.token.lexeme.c_str(),
                node.token.type);
    }

    void visit(BinaryOperator_Node& node) override {
        node.right->accept(*this);
        std::cout << "  push %rax" << std::endl;
        node.left->accept(*this);
        std::cout << "  pop %rdi" << std::endl;
        switch (node.token.type) {
            case TK_PLUS:
                std::cout << "  add %rdi, %rax" << std::endl;
                break;
            case TK_MINUS:
                std::cout << "  sub %rdi, %rax" << std::endl;
                break;
            default:
                break;
        }
    }
};

void CodeGenerator::code_generate(AST_Node& tree) {
    // Prologue
    std::cout << "  .globl main\n";
    std::cout << "main:\n";
    std::cout << "  push %rbp\n";
    std::cout << "  mov %rsp, %rbp\n";

    tree.accept(*this);

    // Epilogue
    std::cout << "  mov %rbp, %rsp\n";
    std::cout << "  pop %rbp\n";
    std::cout << "  ret\n";
}


//===----------------------------------------------------------------------===//
// Main driver code.
//===----------------------------------------------------------------------===//

int main(int argc, char *argv[]) {
    if (argc == 2) {
        /* source code can be read from one of two places:
         * 1. standard input terminal. As in test.sh, it's represented by "-".
         *    This case is used when testing by running "make test" command.
         * 2. a file. This case is used when debugging.
         * */
        // for convenience, source code will be read into a string
        // ref: https://www.zhihu.com/question/426117879/answer/2618969836?utm_source=qq&utm_medium=social&utm_oi=867698515231522816
        std::ostringstream out;  // this var is auxiliary
        if (strcmp(argv[1], "-") == 0) {  // "-" is exactly the string in argv[1]
            out << std::cin.rdbuf();
        } else {   // the string in argv[1] is the name of test file
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
//    for (auto &curToken: lexer.gatherAllTokens()) {
//        fprintf(stderr, "%s : type %d\n", curToken.lexeme.c_str(),
//                curToken.type);
//    }

    // 语法分析
    Parser parser(lexer);
    std::unique_ptr<AST_Node> tree = parser.parse();

    // 语义分析


    // 代码生成
    CodeGenerator code_generator;
    code_generator.code_generate(*tree);
}
