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

#define debug_kill_sets
#define debug_gen_sets

PreservedAnalyses
LivenessAnalysis::run(llvm::Function& fn, llvm::FunctionAnalysisManager&){

  // compute gen and kill sets for each basic block
  for (BasicBlock& bb : fn){
    computeGenKillVariables(&bb);
  }

  errs() << "// ======================================== //\n";

  // compute liveIn and liveOut sets for each basic block
  bool changed = true;
  while (changed) {
    // track whether this iteration changed the liveIn set or not
    changed = false;

    // iterate over each basic block
    for (BasicBlock& bb : fn){

      // compute liveOut set for the current basic block
      for (BasicBlock* nextBB : successors(&bb)){
        for (auto& liveVar : liveIn[nextBB]){
          liveOut[&bb].insert(liveVar);
        }
      }

      // compute liveIn set for the current basic block
      VarSet tmpSet = liveIn[&bb];
      liveIn[&bb] = gen[&bb];
      debugPrintVarSet(gen[&bb]);
      for (auto& var : liveOut[&bb]){
        if (kill[&bb].find(var) == std::end(kill[&bb])){
          liveIn[&bb].insert(var);
        }
      }

      // check if the liveIn set has reached a fixed point
      if (liveIn[&bb] == tmpSet){
        changed = false;
      }
    }
  }

  for (auto& bb : fn){
    errs() << "Live in set for basic block " << bb.getName() << '\n';
    debugPrintVarSet(liveIn[&bb]);
    errs() << '\n';

//    for (auto& instr : bb)
//      errs() << instr << '\n';

    LLVM_DEBUG(dbgs() << "\nLive out set for basic block " << bb.getName() << '\n');
    debugPrintVarSet(liveOut[&bb]);
    LLVM_DEBUG(dbgs() << '\n');
    errs() << "---------------------------------\n";
  }
  return llvm::PreservedAnalyses::all();
}

void LivenessAnalysis::computeGenKillVariables(BasicBlock *bb){

  for (auto& inst : *bb){
    const Instruction* cInst = &inst;

    // insert all users of the current instruction into the gen set
    if (isa<StoreInst>(cInst)){
      auto* storeInst = dyn_cast<StoreInst>(cInst);
      const Value* var = storeInst->getOperand(0);
      if (!isa<Constant>(var)){
        gen[bb].insert(var);
      }
    }
    else{
      for (const Use& use : cInst->operands()){
        const Value* var =  use.get();
        if (!isa<Constant>(var))
          gen[bb].insert(var);
      }

    }

    // store killed variable from cInst
    if (isa<StoreInst>(*cInst)){
      // if store instruction, the second operand is killed
      auto* storeInst = dyn_cast<StoreInst>(cInst);
      kill[bb].insert(storeInst->getOperand(1));
    }
    else{
      if (cInst->hasValueHandle())
        kill[bb].insert(cInst);
    }
  }

#ifdef debug_kill_sets
  LLVM_DEBUG(dbgs() << "Kill set for basic block " << bb->getName() << '\n');
  debugPrintVarSet(kill[bb]);
  LLVM_DEBUG(dbgs() << "\n");
#endif

#ifdef debug_gen_sets
  LLVM_DEBUG(dbgs() << "Gen set for basic block " << bb->getName() << '\n');
  debugPrintVarSet(gen[bb]);
  LLVM_DEBUG(dbgs() << "--------------------------------------\n");
#endif
}

void LivenessAnalysis::debugPrintVarSet(LivenessAnalysis::VarSet& s){
  for (auto& var : s){
    errs() << *var << "\n";
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