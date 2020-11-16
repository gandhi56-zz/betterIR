//
// Created by anshil on 2020-11-14.
//

#ifndef BETTERIR_LIB_DEADCODEELIMINATION_H
#define BETTERIR_LIB_DEADCODEELIMINATION_H

#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"

/*
 * FIXME New pass manager version, not working
struct DeadCodeElimination: public llvm::PassInfoMixin<DeadCodeElimination>{
  llvm::PreservedAnalyses
  run(llvm::Function& fn, llvm::FunctionAnalysisManager&);
};
*/

struct DeadCodeElimination : public llvm::FunctionPass {
  static char ID;
  DeadCodeElimination() : FunctionPass(ID) {}
  bool runOnFunction(llvm::Function &fn) override;
};


#endif //BETTERIR_LIB_DEADCODEELIMINATION_H
