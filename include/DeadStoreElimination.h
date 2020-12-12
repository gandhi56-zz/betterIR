#ifndef BETTERIR_LIB_DEADSTOREELIMINATION_H
#define BETTERIR_LIB_DEADSTOREELIMINATION_H

#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"

using namespace llvm;

namespace{
  struct DeadStoreElimination : public PassInfoMixin<DeadStoreElimination>{
    PreservedAnalyses run(Function& fn, FunctionAnalysisManager&);
  };
}

#endif // BETTERIR_LIB_DEADSTOREELIMINATION_H