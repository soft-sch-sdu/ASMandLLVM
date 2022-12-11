#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

//input_string = source code
static std::string input_string;

static long getNumber() {
    std::string NumberString;
    int pos = 0;
    static char CurrentChar = input_string[pos++];
    while (isdigit(CurrentChar)) {
        NumberString += CurrentChar;
        CurrentChar = input_string[pos++];
    }
    return std::strtol(NumberString.c_str(), nullptr, 10);
}


//===----------------------------------------------------------------------===//
// Main driver code.
//===----------------------------------------------------------------------===//

int main(int argc, char* argv[]) {
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
        } else {
            std::ifstream fin(argv[1]);
            out << fin.rdbuf();
            fin.close();
        }
        input_string = out.str(); // with the help of out, source code is read into a string
    } else {
        std::cout << "Usage: ./code input_source_code here\n";
        return 1;
    }

    std::cout << "  .globl main\n";
    std::cout << "main:\n";
    std::cout << "  mov $" << getNumber() << ", %rax\n";
    std::cout << "  ret\n";
    return 0;
}
