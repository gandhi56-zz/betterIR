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
private:
  using VarSet = DenseSet<const Value*>;
  DenseMap<BasicBlock*, VarSet>  gen, kill;
  DenseMap<Instruction*, VarSet> liveIn, liveOut;

  PreservedAnalyses run(Function &fn, FunctionAnalysisManager &);
  void computeGenKillVariables(BasicBlock *bb);

};
}

#endif //BETTERIR_LIB_LIVENESSANALYSIS_H
