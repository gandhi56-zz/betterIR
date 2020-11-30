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
  void computeGenKillVariables(Function *fn);
  bool killedInstr(const Instruction* cInst);
  static void debugPrintVarSet(LivenessAnalysis::VarSet& s);

};
}

#endif //BETTERIR_LIB_LIVENESSANALYSIS_H
