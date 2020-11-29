//
// Created by anshil on 2020-11-26.
//

#include "LivenessAnalysis.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

#define PASS_NAME "liveness"
#define DEBUG_TYPE "liveness"

llvm::PreservedAnalyses
LivenessAnalysis::run(llvm::Function& fn, llvm::FunctionAnalysisManager&){

  // compute gen and kill sets for each basic block
  for (BasicBlock& bb : fn){
    computeGenKillVariables(&bb);
  }

  // compute liveIn and liveOut sets for each basic block
//  bool changed;
//  do{
//    // track whether this iteration changed the liveIn set or not
//    changed = false;
//
//    // iterate over each basic block
//    for (auto& bb : fn){
//
//      // compute the liveOut set
//      livenessAtInstrExit(&bb);
//
//      // compute the liveIn set
//      livenessAtInstrEntry(&bb, changed);
//
//    }
//  } while (changed);
//
//  for (auto& bb : fn){
//    LLVM_DEBUG(dbgs() << "Live in set for basic block" << bb.getName() << '\n');
//    debugPrintVarSet(liveIn[&bb]);
//    LLVM_DEBUG(dbgs() << '\n');
//
//    LLVM_DEBUG(dbgs() << "\nLive out set for basic block" << bb.getName() << '\n');
//    debugPrintVarSet(liveOut[&bb]);
//    LLVM_DEBUG(dbgs() << '\n');
//  }

  return llvm::PreservedAnalyses::all();
}

void LivenessAnalysis::livenessAtInstrExit(BasicBlock* bb){
  for (BasicBlock* nextBB : successors(bb)){
    for (auto& liveVar : liveIn[nextBB]){
      liveOut[bb].insert(liveVar);
    }
  }
}

void LivenessAnalysis::livenessAtInstrEntry(BasicBlock* bb, bool& changed){
  liveIn[bb] = gen[bb];
  for (auto& liveVar : liveOut[bb]){
    if (kill[bb].find(liveVar) == std::end(kill[bb])){
      liveIn[bb].insert(liveVar);
      changed = true;
    }
  }
}

void LivenessAnalysis::computeGenKillVariables(BasicBlock *bb){

  for (auto& inst : *bb){
    const Instruction* cInst = &inst;

    // store killed variable from cInst
    if (isa<StoreInst>(*cInst)){
      // if store instruction, the second operand is killed
      auto* storeInst = dyn_cast<StoreInst>(cInst);
      kill[bb].insert(storeInst->getOperand(1));
    }
    else{
      // otherwise the def of the instruction is killed
      if (!isa<Constant>(cInst->getOperand(0)))
        kill[bb].insert(dyn_cast<Value>(cInst->getOperand(0)));
    }

    // store generated variable from cInst
    if (isa<StoreInst>(*cInst)){
      // record the operand into the gen set iff the operand is not a constant
      auto* var = cInst->getOperand(0);
      const Constant* constValue = dyn_cast<Constant>(var);
      if (!constValue){
        gen[bb].insert(var);
      }
    }
    else{
      for (int i = 0; i < cInst->getNumOperands(); ++i){
        if (!isa<Constant>(cInst->getOperand(i))){
          const Value* var = dyn_cast<Value>(cInst->getOperand(i));
          gen[bb].insert(var);
        }
      }
    }
  }

  LLVM_DEBUG(dbgs() << "Kill set for basic block" << bb->getName() << '\n');
  debugPrintVarSet(kill[bb]);
  LLVM_DEBUG(dbgs() << "\n");

  LLVM_DEBUG(dbgs() << "\nGen set for basic block" << bb->getName() << '\n');
  debugPrintVarSet(gen[bb]);
  LLVM_DEBUG(dbgs() << '\n');

}

void LivenessAnalysis::debugPrintVarSet(LivenessAnalysis::VarSet& s){
  for (auto& var : s){
    LLVM_DEBUG(dbgs() << *var << "\n");
  }
}

// --------------------------------------------------------------------------------
// Pass Manager registration
// --------------------------------------------------------------------------------

llvm::PassPluginLibraryInfo getLivenessPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, PASS_NAME, LLVM_VERSION_STRING,
          [](llvm::PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef Name, llvm::FunctionPassManager &FPM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) {
                  if (Name == "liveness") {
                    FPM.addPass(LivenessAnalysis());
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getLivenessPluginInfo();
}