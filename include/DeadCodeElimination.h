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
  PreservedAnalyses run(Function &fn, FunctionAnalysisManager &);
  bool runOnBasicBlock(BasicBlock &bb);
  bool removeTriviallyDeadInstr(BasicBlock &bb);
  bool constantFolding(Function &fn);
  static bool removeUnusedInstr(BasicBlock& bb);

  int numInstrDeleted = 0;
  int numBasicBlocksDeleted = 0;
};
}


#endif //BETTERIR_LIB_DEADCODEELIMINATION_H
