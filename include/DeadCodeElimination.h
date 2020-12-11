//
// Created by anshil on 2020-11-14.
//

#ifndef BETTERIR_LIB_DEADCODEELIMINATION_H
#define BETTERIR_LIB_DEADCODEELIMINATION_H

#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"

using namespace llvm;

namespace {
struct DeadCodeElimination : public PassInfoMixin<DeadCodeElimination> {
  using VarSet = DenseSet<const Value*>;
  DenseMap<BasicBlock*, VarSet>  use, def;
  DenseMap<BasicBlock*, VarSet> liveIn, liveOut;
  
  PreservedAnalyses run(Function &fn, FunctionAnalysisManager &);
  
  void computeLiveness(Function& fn);
  void computeGenKillVariables(Function *fn);
  static void debugPrintVarSet(VarSet& s);
  void computeLiveOut(BasicBlock* bb);
  void computeLiveIn(BasicBlock* bb);
};
}


#endif //BETTERIR_LIB_DEADCODEELIMINATION_H
