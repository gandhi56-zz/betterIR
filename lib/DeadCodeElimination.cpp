//
// Created by anshil on 2020-11-14.
//

/**
 *
 * Usage:
 *
 * Legacy PM
 *  opt -load cmake-build-debug/lib/libDeadCodeElimination.so `\` --dead-code <bitcode-file>
 *
 * New PM
 *    opt -load-pass-plugin cmake-build-debug/lib/libDeadCodeElimination.so
 *        -passes=-"dce" <bitcode-file>
 */


#include "DeadCodeElimination.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#define PASS_NAME "dce"


llvm::PreservedAnalyses
DeadCodeElimination::run(llvm::Function& fn, llvm::FunctionAnalysisManager&){
  //=== pass implementation goes here ===//
  llvm::errs() << fn.getName() << "\n";
  return llvm::PreservedAnalyses::all();
}

// --------------------------------------------------------------------------------
// Pass Manager registration
// --------------------------------------------------------------------------------

llvm::PassPluginLibraryInfo getDCEPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, PASS_NAME, LLVM_VERSION_STRING,
          [](llvm::PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef Name, llvm::FunctionPassManager &FPM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) {
                  if (Name == "dead-code") {
                    FPM.addPass(DeadCodeElimination());
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getDCEPluginInfo();
}
