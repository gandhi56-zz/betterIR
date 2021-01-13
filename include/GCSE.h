//
// Created by anshil on 2021-01-12.
//

#ifndef BETTERIR_GCSE_H
#define BETTERIR_GCSE_H

#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"
#include <set>
using namespace llvm;
typedef DenseMap<BasicBlock*, std::set<Value*>> VarSet;

struct GCSE: public PassInfoMixin<GCSE>{

  VarSet definedExprSet;
  VarSet killedExprSet;
  VarSet availExprSet;

  PreservedAnalyses run(Function& fn,
                              FunctionAnalysisManager&);
};

#endif//BETTERIR_GCSE_H
