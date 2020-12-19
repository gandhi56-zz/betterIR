//
// Created by anshil on 2020-11-26.
//

#include "ReachingDefinitions.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

#define PASS_NAME "reach-defs"
#define DEBUG_TYPE "reach-defs"

// #define debug_def_sets
// #define debug_use_sets

PreservedAnalyses
ReachingDefinitions::run(llvm::Function& fn, llvm::FunctionAnalysisManager&){
  computeGenKillVariables(&fn);

  for (BasicBlock& bb : fn){
    in[&bb].clear();
  }

  while (true) {
    bool changed = false;
    for (BasicBlock& bb : fn){
      const BasicBlock* currBB = &bb;

      // no computation at exit basic blocks
      if (successors(currBB).empty()) continue;

      // backup the previously computed in set for this block
      VarSet prevIn = in[&bb];

      computeLiveOut(&bb);
      if (currBB != &fn.getEntryBlock())
        computeLiveIn(&bb);
      changed |= (in[&bb] != prevIn);
    }
    if (!changed) break;
  }

  for (auto& bb : fn){
    errs() << "Live in set for basic block " << bb.getName() << " with #instructions = " << bb.getInstList().size() << '\n';
    debugPrintVarSet(in[&bb]);
    errs() << '\n';

    errs() << "\nLive out set for basic block " << bb.getName() << '\n';
    debugPrintVarSet(out[&bb]);
    errs() << "\n---------------------------------\n";
  }
  return llvm::PreservedAnalyses::all();
}

void ReachingDefinitions::computeLiveOut(BasicBlock* bb){
  out[bb].clear();
  for (BasicBlock* nextBB : successors(bb)){
    for (auto& liveVar : in[nextBB]){
      out[bb].insert(liveVar);
    }
  }
}

void ReachingDefinitions::computeLiveIn(BasicBlock* bb){
  in[bb].clear();
  in[bb] = gen[bb];
  for (auto& var : out[bb]){
    if (kill[bb].find(var) == kill[bb].end()){
      in[bb].insert(var);
    }
  }
}

void ReachingDefinitions::computeGenKillVariables(Function *fn) {
  for (auto& bb : *fn) {
    for (auto& inst : bb) {
      const Instruction *cInst = &inst;
      if (cInst->isTerminator())  continue;

      // insert each operand into the gen set of this basic block
      for (auto &op : cInst->operands()) {
        if (!isa<Constant>(op))
          gen[&bb].insert(op);
      }

      if (cInst->getType() != Type::getVoidTy(fn->getContext())){
        if (!isa<Constant>(cInst))
          gen[&bb].insert(cInst);
      }
    }

#ifdef debug_def_sets
    errs() << "Def set for basic block " << bb.getName() << '\n';
    debugPrintVarSet(def[&bb]);
    errs() << "\n";
#endif

#ifdef debug_use_sets
    errs() << "Use set for basic block " << bb.getName() << '\n';
    debugPrintVarSet(gen[&bb]);
    errs() << "--------------------------------------\n";
    errs() << "// ======================================== //\n";
#endif
  }
}

void ReachingDefinitions::debugPrintVarSet(ReachingDefinitions::VarSet& s){
  for (auto& var : s){
    errs() << *var << "\n";
  }
}

// --------------------------------------------------------------------------------
// Pass Manager registration
// --------------------------------------------------------------------------------

llvm::PassPluginLibraryInfo getReachingDefinitionsPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, PASS_NAME, LLVM_VERSION_STRING,
          [](llvm::PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef Name, llvm::FunctionPassManager &FPM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) {
                  if (Name == "reach-defs") {
                    FPM.addPass(ReachingDefinitions());
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getReachingDefinitionsPluginInfo();
}