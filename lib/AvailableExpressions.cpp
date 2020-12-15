
#define LIN errs() << __LINE__ << '\n';

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

  for (auto& bb : fn){
    computeKillExpressions(&bb, fn);
  }

  return PreservedAnalyses::none();
}


/**
 * @brief compute all expressions killed by the basic block bb,
 *  store them in kill[bb]
 */
void AvailableExpressions::computeKillExpressions(BasicBlock* bb, Function& fn){
  kill[bb].clear();

  // find all variables killed in this basic block
  VarSet killedVariables;
  for (auto& inst : *bb){
    if (isa<StoreInst>(inst)){
      killedVariables.insert(inst.getOperand(1));
    }
  }

  // loop over all binary expressions in the program
  for (auto& otherBB : fn){
    for (auto& E : otherBB){
      if (!E.isBinaryOp())  continue;
      auto* binOp = dyn_cast<BinaryOperator>(&E);

      // if bb does not generate binOp and one of its operands
      // is killed in this basic block, then
      // insert binOp into the kill set of expressions
      if (gen[bb].find(binOp) == gen[bb].end()){
        for (auto& op : binOp->operands()){
          if (killedVariables.find(op) != killedVariables.end()){
            kill[bb].insert(binOp);
          }
        }
      }

    }
  }


  for (auto& expr : kill[bb]){
    errs() << "killed expr : " << *expr << '\n';
  }
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
    }
  }

  for (auto& inst : *bb){
//    errs() << inst << '\n';
    if (isa<StoreInst>(inst)){
      
      // get the variable whose value is killed by this
      // store instruction
      auto* storeVariable = dyn_cast<StoreInst>(&inst)->getOperand(1);
//      errs() << "STORE VARIABLE: " << *storeVariable << '\n';

      // loop over each generated expression of this basic block
      for (auto& expr : gen[bb]){
        auto* exprInst = dyn_cast<Instruction>(expr);

        // check if any operand of the generated expression
        // is the killing variable, then the expression is
        // not generated anymore
        for (auto& op : exprInst->operands()){
          auto* opInst = dyn_cast<Instruction>(op);
          if (isa<LoadInst>(opInst)){
            opInst = dyn_cast<Instruction>(opInst->getOperand(0));
//            errs() << "108: " << *opInst << '\n';
//            errs() << "109: " << *storeVariable << '\n';
            if (opInst == storeVariable){
              errs() << "removing operand " << *opInst << '\n';
              gen[bb].erase(exprInst);
            }
          }
        }
      }
    
    }
  }

  for (auto& expr : gen[bb]){
    errs() << "generated expression : " << *expr << '\n';
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
