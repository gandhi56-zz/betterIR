//
// Created by anshil on 2020-12-14.
//

#ifndef BETTERIR_LIB_REACHINGDEFINITIONS_H
#define BETTERIR_LIB_REACHINGDEFINITIONS_H

#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"

using namespace llvm;

namespace {
struct ReachingDefinitions : public PassInfoMixin<ReachingDefinitions> {
  using VarSet = DenseSet<const Value*>;
  DenseMap<BasicBlock*, VarSet> gen, kill;
  DenseMap<BasicBlock*, VarSet> in, out;

  PreservedAnalyses run(Function &fn, FunctionAnalysisManager &);
  void computeGenKillVariables(Function *fn);
  static void debugPrintVarSet(VarSet& s);
  void computeLiveOut(BasicBlock* bb);
  void computeLiveIn(BasicBlock* bb);
};
}

#endif//BETTERIR_LIB_REACHINGDEFINITIONS_H
