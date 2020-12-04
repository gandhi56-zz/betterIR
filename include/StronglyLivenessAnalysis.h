//
// Created by anshil on 2020-12-01.
//

#ifndef BETTERIR_LIB_STRONGLYLIVENESSANALYSIS_H
#define BETTERIR_LIB_STRONGLYLIVENESSANALYSIS_H

#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"

using namespace llvm;

namespace {
struct StronglyLivenessAnalysis : public PassInfoMixin<StronglyLivenessAnalysis> {
  using VarSet = DenseSet<const Value*>;
  DenseMap<BasicBlock*, VarSet>  gen, kill;
  DenseMap<BasicBlock*, VarSet> liveIn, liveOut;

  PreservedAnalyses run(Function &fn, FunctionAnalysisManager &);
  void computeGenKillVariables(Function *fn);
  static void debugPrintVarSet(VarSet& s);

};
}

#endif //BETTERIR_LIB_STRONGLYLIVENESSANALYSIS_H
