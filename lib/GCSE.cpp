//
// Created by anshil on 2021-01-12.
//

#include "GCSE.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include <vector>
#define PASS_NAME "GCSE"

PreservedAnalyses GCSE::run(Function &fn, FunctionAnalysisManager &) {

  /// populate definedExprSet and killedExprSet of each basic block
  /// in this function, iterate over the instructions in reverse order
  /// in the basic block
  for (auto& bb : fn){
    DenseSet<Value*> killedVars;
    for (auto& instr : reverse(bb)){
      if (!instr.isBinaryOp())  continue;
      killedVars.insert(&instr);
      auto op1 = instr.getOperand(0);
      auto op2 = instr.getOperand(1);

      if (isa<Constant>(op1) or isa<Constant>(op2)) continue;

      bool killedOp1 = killedVars.count(op1) == 0;
      bool killedOp2 = killedVars.count(op2) == 0;
      if (killedOp1 or killedOp2)
        killedExprSet[&bb].insert(&instr);
      if (killedOp1 and killedOp2){
        definedExprSet[&bb].insert(&instr);
      }

    }
  }

  /// compute available expressions :-
  // create a worklist of basic blocks to compute available expressions
  std::set<BasicBlock*> workList;
  for (auto& bb : fn)
    workList.insert(&bb);

  // lambda for computing available expressions
  // avail[x] = defExpr[x] U (availExpr[x] - killExpr[x])
  auto computeAvailExpr =
      [](BasicBlock* x, VarSet& defExpr, VarSet& availExpr, VarSet& killExpr){
        std::set<Value*> res = defExpr[x];
        for (Value* expr : availExpr[x]){
          if (killExpr[x].find(expr) == killExpr[x].end()){
            res.insert(expr);
          }
        }
        return res;
  };

  // execute fixed point iteration method to compute all available expressions
  // Note: solution is unique so the order of basic blocks in the worklist
  //  does not matter
  while (!workList.empty()){
    auto bb = *workList.begin();
    workList.erase(bb);

    availExprSet[bb].clear();
    BasicBlock* pred = *predecessors(bb).begin();
    availExprSet[bb] = computeAvailExpr(pred, definedExprSet, availExprSet, killedExprSet);
    ++pred;
    while (pred != *predecessors(bb).end()){
      auto tmp = availExprSet[bb];
      auto res = computeAvailExpr(pred, definedExprSet, availExprSet, killedExprSet);
      for (auto it = availExprSet[bb].begin(); it != availExprSet[bb].end(); ++it){
        if (res.find(*it) == res.end()){
          availExprSet[bb].erase(*it);
        }
      }
      if (tmp != availExprSet[bb]){
        for (auto nextBB : successors(bb))
          workList.insert(nextBB);
      }
    }
  }

  return PreservedAnalyses();
}

// --------------------------------------------------------------------------------
// Pass Manager registration
// --------------------------------------------------------------------------------

PassPluginLibraryInfo getGCSEPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, PASS_NAME, LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == PASS_NAME) {
                    FPM.addPass(GCSE());
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getGCSEPluginInfo();
}