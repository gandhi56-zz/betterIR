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

#include "LivenessAnalysis.h"
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
  bool changed;
  while (1){
    changed = false;
    std::vector<Value*> deadInstrList;
    for (auto& bb : fn){
      for (auto& inst : bb){
        if (isa<ReturnInst>(inst))  continue;
        if (inst.getNumUses() == 0){
          errs() << "removing " << inst << '\n';
          deadInstrList.push_back(&inst);
          changed = true;
        }
      }
    }

    for (auto* val : deadInstrList){
      auto* deadInstr = dyn_cast<Instruction>(val);
      deadInstr->eraseFromParent();
    }
    if (!changed) break;
  }

  return changed? PreservedAnalyses::all() : PreservedAnalyses::none();
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
