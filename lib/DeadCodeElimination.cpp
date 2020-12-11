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

  computeLiveness(fn);

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
      // FIXME: dead instruction condition needs to be fixed
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

void DeadCodeElimination::computeLiveness(Function& fn){
  computeGenKillVariables(&fn);
  while (true) {
    bool changed = false;
    for (BasicBlock& bb : reverse(fn)){
      VarSet prevOut = liveOut[&bb];
      VarSet prevIn = liveIn[&bb];
      computeLiveOut(&bb);
      computeLiveIn(&bb);
      changed |= (liveOut[&bb] != prevOut) or (liveIn[&bb] != prevIn);
    }
    if (!changed) break;
  }

#ifdef debug_liveness
  for (auto& bb : fn){
    errs() << "Live in set for basic block " << bb.getName() << " with #instructions = " << bb.getInstList().size() << '\n';
    debugPrintVarSet(liveIn[&bb]);
    errs() << '\n';

    errs() << "\nLive out set for basic block " << bb.getName() << '\n';
    debugPrintVarSet(liveOut[&bb]);
    errs() << "\n---------------------------------\n";
  }
#endif
}

void DeadCodeElimination::computeLiveOut(BasicBlock* bb){
  liveOut[bb].clear();
  for (BasicBlock* nextBB : successors(bb)){
    for (auto& liveVar : liveIn[nextBB]){
      liveOut[bb].insert(liveVar);
    }
  }
}

void DeadCodeElimination::computeLiveIn(BasicBlock* bb){
  liveIn[bb].clear();
  liveIn[bb] = use[bb];
  for (auto& var : liveOut[bb]){
    if (def[bb].count(var) == 0){
      liveIn[bb].insert(var);
    }
  }
}

void DeadCodeElimination::computeGenKillVariables(Function *fn) {
  for (auto& bb : *fn) {
    for (auto& inst : bb) {
      const Instruction *cInst = &inst;
      if (cInst->isTerminator())  continue;

      // insert each operand into the use set of this basic block
      for (auto &op : cInst->operands()) {
        if (!isa<Constant>(op))
          use[&bb].insert(op);
      }

      /// a value was killed by 
      if (cInst->getType() != Type::getVoidTy(fn->getContext())){
        if (!isa<Constant>(cInst))
          def[&bb].insert(cInst);
      }
    }

#ifdef debug_def_sets
    errs() << "Def set for basic block " << bb.getName() << '\n';
    debugPrintVarSet(def[&bb]);
    errs() << "\n";
#endif

#ifdef debug_use_sets
    errs() << "Use set for basic block " << bb.getName() << '\n';
    debugPrintVarSet(use[&bb]);
    errs() << "--------------------------------------\n";
    errs() << "// ======================================== //\n";
#endif
  }
}

void DeadCodeElimination::debugPrintVarSet(VarSet& s){
  for (auto& var : s){
    errs() << *var << "\n";
  }
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
