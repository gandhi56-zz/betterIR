

#include "AvailableExpressions.h"
#include <vector>
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/Local.h"

#define DEBUG_TYPE "avail-expr"

#define PASS_NAME "avail-expr"

PreservedAnalyses AvailableExpressions::run(Function& fn,
                                           FunctionAnalysisManager&) {
  errs() << "finding available expressions...\n";

  for (auto& bb : fn){
    computeGenExpressions(&bb);
  }

  return PreservedAnalyses::none();
}


/**
 * @brief compute all expressions killed by the basic block bb,
 *  store them in kill[bb]
 */
void AvailableExpressions::computeKillExpressions(BasicBlock* bb){

}

/**
 * @brief compute all expressions generated by the basic block bb,
 *  store them in gen[bb]
 */
void AvailableExpressions::computeGenExpressions(BasicBlock* bb){
  gen[bb].clear();
  for (auto& inst : *bb){
    if (inst.isBinaryOp()){
      auto* binOp = dyn_cast<BinaryOperator>(&inst);
      gen[bb].insert(binOp);
      
      // for each expr in gen[bb], remove expr if one of its
      // operands contains the value of inst
      for (auto& expr : gen[bb]){
        auto* exprInst = dyn_cast<Instruction>(expr);
        for (auto& op : exprInst->operands()){
          auto* opInst = dyn_cast<Instruction>(op);
          if (opInst == &inst){
            errs() << "removing operand " << opInst << '\n';
            gen[bb].erase(opInst);
          }
        }
      }
    }
  }

  for (auto& expr : gen[bb]){
    errs() << *expr << '\n';
  }
}

// --------------------------------------------------------------------------------
// Pass Manager registration
// --------------------------------------------------------------------------------

PassPluginLibraryInfo getAEPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, PASS_NAME, LLVM_VERSION_STRING,
          [](PassBuilder& PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager& FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "avail-expr") {
                    FPM.addPass(AvailableExpressions());
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return getAEPluginInfo();
}
