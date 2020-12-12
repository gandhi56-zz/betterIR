#ifndef BETTERIR_LIB_AVAILABLEEXPRESSIONS_H
#define BETTERIR_LIB_AVAILABLEEXPRESSIONS_H

#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"

using namespace llvm;

namespace{
  struct AvailableExpressions : public PassInfoMixin<AvailableExpressions>{
    PreservedAnalyses run(Function& fn, FunctionAnalysisManager&);
  };
}

#endif // BETTERIR_LIB_AVAILABLEEXPRESSIONS_H