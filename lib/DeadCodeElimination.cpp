//
// Created by anshil on 2020-11-14.
//

/**
 *
 * Usage:
 *
 * New PM
 *    opt -load-pass-plugin cmake-build-debug/lib/libDeadCodeElimination.so
 *    -passes="dead-code" tests/DeadCodeElimination/input/foo00.ll -disable-output
 */


#include "DeadCodeElimination.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include <stack>
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/Local.h"
#include "llvm/ADT/Statistic.h"

#define DEBUG_TYPE "dead-code"

#define PASS_NAME "dce"

STATISTIC(numInstrDeleted, "Number of Instructions Eliminated");
STATISTIC(numBasicBlocksDeleted, "Number of Basic Blocks Eliminated");

PreservedAnalyses
DeadCodeElimination::run(Function& fn, FunctionAnalysisManager&){
  bool changed = false;
  for (auto& bb : fn){
    changed |= runOnBasicBlock(bb);
  }
  changed |= constantFolding(fn);
  return changed? PreservedAnalyses::all() : PreservedAnalyses::none();
}

bool DeadCodeElimination::runOnBasicBlock(BasicBlock& bb){
  bool changed = false;
  changed |= removeTriviallyDeadInstr(bb);
  changed |= removeUnusedInstr(bb);
  return changed;
}

/// FIXME
bool DeadCodeElimination::removeUnusedInstr(BasicBlock& bb){
  bool changed;
  std::stack<Instruction*> deadInstrStack;
  do{
    changed = false;
    for (auto& inst : bb){
      if (inst.getNumUses() == 0){
        deadInstrStack.push(&inst);
      }
    }

    if (!deadInstrStack.empty()){
      changed = true;
      while (!deadInstrStack.empty()){
        deadInstrStack.top()->eraseFromParent();
        deadInstrStack.pop();
      }
    }

  }while (changed);
  return true;
}

bool DeadCodeElimination::removeTriviallyDeadInstr(BasicBlock& bb){
  bool changed = false;
  std::stack<Instruction*> deadInstrStack;
  for (auto& inst : bb) {
    if (isInstructionTriviallyDead(&inst)){
      deadInstrStack.push(&inst);
      changed = true;
    }
  }
  while (!deadInstrStack.empty()){
    deadInstrStack.top()->eraseFromParent();
    deadInstrStack.pop();
  }
  return changed;
}

bool DeadCodeElimination::constantFolding(Function &fn){
  for (auto& bb : fn){
    ConstantFoldTerminator(&bb);
  }
  return true;
}

// --------------------------------------------------------------------------------
// Pass Manager registration
// --------------------------------------------------------------------------------

PassPluginLibraryInfo getDCEPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, PASS_NAME, LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "dead-code") {
                    FPM.addPass(DeadCodeElimination());
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getDCEPluginInfo();
}
