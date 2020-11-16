
/**
 *
 * Usage: opt -load-pass-plugin libOpcodeCounter.so --passes='default<O1>' `\` -disable-output <input-llvm-file>
 */


#include "OpCodeCounter.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include <iostream>

#define PASS_NAME "opcc"

llvm::PreservedAnalyses OpCodeCounter::run(llvm::Function &fn,
                                           llvm::FunctionAnalysisManager &) {
  llvm::errs() << "running opcode counter\n";
  OpCodeCounter::Result opcodeMap = generateOpcodeMap(fn);
  printResult(opcodeMap, fn.getName());
  return llvm::PreservedAnalyses::all();
}

OpCodeCounter::Result OpCodeCounter::generateOpcodeMap(llvm::Function &f) {
  OpCodeCounter::Result opcodeMap;
  for (auto& bb : f){
    for (auto& inst : bb){
      llvm::StringRef name = inst.getOpcodeName();
      if (opcodeMap.find(name) == opcodeMap.end()){
        opcodeMap[inst.getOpcodeName()] = 1;
      }
      else{
        opcodeMap[inst.getOpcodeName()]++;
      }
    }
  }
  return opcodeMap;
}

static void printResult(const ResultOpcodeCounter& counter,
                        llvm::StringRef funName){
  llvm::errs() << "================================================="
               << "\n";
  llvm::errs() << "BetterIR: OpcodeCounter results for `" << funName
               << "`\n";
  llvm::errs() << "=================================================\n";
  const char *str1 = "OPCODE";
  const char *str2 = "#N TIMES USED";
  llvm::errs() << llvm::format("%-20s %-10s\n", str1, str2);
  llvm::errs() << "-------------------------------------------------"
               << "\n";
  for (auto &op : counter) {
    llvm::errs() << llvm::format("%-20s - %-10lu\n", op.first().str().c_str(),
                                 op.second);
  }

  llvm::errs() << "-------------------------------------------------"
               << "\n\n";
}

// --------------------------------------------------------------------------------
// Pass Manager registration
// --------------------------------------------------------------------------------

llvm::PassPluginLibraryInfo getMBAAddPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, PASS_NAME, LLVM_VERSION_STRING,
          [](llvm::PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef Name, llvm::FunctionPassManager &FPM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) {
                    if (Name == PASS_NAME) {
                      FPM.addPass(OpCodeCounter());
                      return true;
                    }
                    return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getMBAAddPluginInfo();
}