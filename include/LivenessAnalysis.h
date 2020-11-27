//
// Created by anshil on 2020-11-26.
//

#ifndef BETTERIR_LIB_LIVENESSANALYSIS_H
#define BETTERIR_LIB_LIVENESSANALYSIS_H

#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"

using namespace llvm;

namespace {
struct LivenessAnalysis : public PassInfoMixin<LivenessAnalysis> {
  DenseMap<const Instruction*, DenseSet<const Value*>> liveIn, liveOut;

  PreservedAnalyses run(Function &fn, FunctionAnalysisManager &);

  void getTermSuccInstr(const BasicBlock* bb);
  void computeUsesDefs(const BasicBlock* bb);
  void printLiveVariables(Function& fn);

};
}

#endif //BETTERIR_LIB_LIVENESSANALYSIS_H
