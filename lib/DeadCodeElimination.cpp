//
// Created by anshil on 2020-11-14.
//

/**
 *
 * Usage:
 *
 * New PM
 *    opt -load-pass-plugin cmake-build-debug/lib/libDeadCodeElimination.so
 *    -passes="dead-code" tests/DeadCodeElimination/input/foo00.ll
 * -disable-output
 */

#include "DeadCodeElimination.h"
#include <vector>
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/Local.h"

#define DEBUG_TYPE "dead-code"

#define PASS_NAME "dead-code"

PreservedAnalyses DeadCodeElimination::run(Function& fn,
                                           FunctionAnalysisManager&) {
  bool changed;
  errs() << "\nrunning dead code elimination...\n";

  std::vector<Value*> deadInstrVec;
  // DenseSet<Value*> usedValues;
  for (auto& bb : reverse(fn)){
    for (auto& inst : reverse(bb)){
      if (inst.isTerminator() or isa<CallInst>(inst))  continue;
        
      /// instruction with a non-void return value
      // if the current instruction has no use following it
      // declare it as a dead instruction
      if (inst.getType() != Type::getVoidTy(fn.getContext()) and inst.getNumUses() == 0){
        deadInstrVec.push_back(&inst);
      }

    }
  }

  errs() << "\n# dead instructions = " << deadInstrVec.size() << '\n';
  for (auto* inst : deadInstrVec) errs() << "removing " << *inst << '\n';

  changed = !deadInstrVec.empty();

  for (auto* inst : deadInstrVec){
    auto* deadInst = dyn_cast<Instruction>(inst);
    deadInst->eraseFromParent();
  }


  return changed ? PreservedAnalyses::all() : PreservedAnalyses::none();
}

// --------------------------------------------------------------------------------
// Pass Manager registration
// --------------------------------------------------------------------------------

PassPluginLibraryInfo getDCEPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, PASS_NAME, LLVM_VERSION_STRING,
          [](PassBuilder& PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager& FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "dead-code") {
                    FPM.addPass(DeadCodeElimination());
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return getDCEPluginInfo();
}
