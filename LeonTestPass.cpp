//
// Created by leon on 23-3-6.
//
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

class MyTestPass : public llvm::FunctionPass {
public:
    static char ID;
    MyTestPass() : FunctionPass(ID) {}
    bool runOnFunction(llvm::Function &F) override {
        llvm::errs() << "Hello, I am MyPass!\n";
        return false;
    }
};