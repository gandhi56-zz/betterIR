#ifndef BETTERIR_LIB_AVAILABLEEXPRESSIONS_H
#define BETTERIR_LIB_AVAILABLEEXPRESSIONS_H

#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"

using namespace llvm;

namespace{
  struct AvailableExpressions : public PassInfoMixin<AvailableExpressions>{
    using VarSet = DenseSet<Value*>;
    DenseMap<BasicBlock*, VarSet> gen, kill;
    
    PreservedAnalyses run(Function& fn, FunctionAnalysisManager&);
    void computeKillExpressions(BasicBlock* bb);
    void computeGenExpressions(BasicBlock* bb);
  };
}

#endif // BETTERIR_LIB_AVAILABLEEXPRESSIONS_H