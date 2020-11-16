#ifndef _OPCC_PASS_
#define _OPCC_PASS_

#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"

using ResultOpcodeCounter = llvm::StringMap<unsigned>;

struct OpCodeCounter: public llvm::PassInfoMixin<OpCodeCounter>{
  using Result = ResultOpcodeCounter;
  llvm::PreservedAnalyses run(llvm::Function& fn,
                              llvm::FunctionAnalysisManager&);
  static OpCodeCounter::Result generateOpcodeMap(llvm::Function& f);
};

static void printResult(const ResultOpcodeCounter& counter,
                        llvm::StringRef funName);

#endif // _OPCC_PASS_