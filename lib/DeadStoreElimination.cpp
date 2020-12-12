

#include "DeadStoreElimination.h"
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

PreservedAnalyses DeadStoreElimination::run(Function& fn,
                                           FunctionAnalysisManager&) {
  
  bool changed = false;
  for (auto& bb : fn){
    
    DenseMap<Value*, Instruction*> val2Store;
    std::vector<Value*> deadInstrVec;

    for (auto& inst : bb){
      if (isa<StoreInst>(inst)){
        Value* changedValue = inst.getOperand(1);
        if (val2Store.find(changedValue) != val2Store.end()){
          // last stored value was never used
          // mark the instruction as dead
          deadInstrVec.push_back(val2Store[changedValue]);
        }
        val2Store[changedValue] = &inst;
      }
      else{
        for (auto& op : inst.operands()){
          if (!isa<Constant>(op))
            val2Store[op] = nullptr;
        }
      }
    }

    // remove all dead stores
    for (auto& instr : deadInstrVec){
      errs() << "removing " << *instr << '\n';
      dyn_cast<Instruction>(instr)->eraseFromParent();
      changed = true;
    }

  }

  return !changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
}

// --------------------------------------------------------------------------------
// Pass Manager registration
// --------------------------------------------------------------------------------

PassPluginLibraryInfo getDSEPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, PASS_NAME, LLVM_VERSION_STRING,
          [](PassBuilder& PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager& FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "dead-store") {
                    FPM.addPass(DeadStoreElimination());
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return getDSEPluginInfo();
}
