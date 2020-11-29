//
// Created by anshil on 2020-11-26.
//

#define LIN errs() << __LINE__ << '\n';

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

  // determine the dfs order of basic blocks in this function
  std::vector<const BasicBlock*> workVec;
  for (const BasicBlock* bb : depth_first(&fn.getEntryBlock()))
    workVec.push_back(bb);

  while (!workVec.empty()){
    const BasicBlock* bb = workVec.back(); workVec.pop_back();
    getTermSuccInstr(bb);
    computeUsesDefs(bb);
    for (const BasicBlock* pred : predecessors(bb))
      workVec.push_back(pred);
  }

  // output code and live variables
  printLiveVariables(fn);

  return llvm::PreservedAnalyses::all();
}

void LivenessAnalysis::printLiveVariables(Function& fn){
  for (auto & b : fn){
    const BasicBlock* bb = &b;
    for (auto i = bb->begin(); i != bb->end(); ++i){
      const Instruction* instr = &*i;
      if (i == bb->begin()){
        errs() << "Live : ";
        for (auto var : liveIn[instr])
          errs() << *var << ' ';
        errs() << '\n';
      }
      errs() << '\n' << *instr << "\n\n";
      errs() << "Live : ";
      for (auto var : liveOut[instr])
        errs() << *var << ' ';
      errs() << '\n';
      if (instr->isTerminator())  errs() << '\n';
    }
  }
}

void LivenessAnalysis::getTermSuccInstr(const BasicBlock* bb){
  // set liveOut for the terminator instruction of this basic block
  for (const BasicBlock* succ : successors(bb))
    for (const Value* v : liveIn[&(*(succ->begin()))])
      liveOut[bb->getTerminator()].insert(v);
}

void LivenessAnalysis::computeUsesDefs(const BasicBlock* bb){
  // iterate over each instruction to compute uses and defs
  for (auto i = bb->rbegin(); i != bb->rend(); ++i){
    const Instruction* instr = &(*i);
    DenseSet<const Value*> uses, defs;

    errs() << *instr << '\n';

    // liveOut[terminator instruction] is the liveIn of the following instruction
    /// Is this working properly?
//    if (i != bb->rbegin()){
//      auto tmp  = i;
//      liveOut[instr] = liveIn[&*(--tmp)];
//    }

    // if store instruction, the second operand is defined,
    //  store the operand in the defs set
    // otherwise store the instruction into the set
    const Value* lhsValue;
    if (isa<StoreInst>(*instr)){
      lhsValue = instr->getOperand(1);
      errs() << "STORE DEF " << *lhsValue << '\n';
    }
    else{
      lhsValue = dyn_cast<Value>(instr);
    }
    defs.insert(lhsValue);

    // if store instruction, the first operand is used,
    //  store the operand in the uses set
    // otherwise store the operands of the instruction into the set
    //  if they are valid definitions of an instruction
    if (isa<StoreInst>(*instr)){
      auto* storeInstr = dyn_cast<StoreInst>(instr);
      if (!isa<Constant>(storeInstr->getOperand(0))){
        uses.insert(storeInstr->getOperand(0));
        errs() << "STORE USE " << *storeInstr->getOperand(0) << '\n';
      }
    }
    else{
      /// FIXME: global variables, other instructions
      for (auto& op : instr->operands()) {
        errs() << "operands = " << *op << ' ';
        if (!isa<ConstantInt>(op)) {
          uses.insert(op);
          errs() << "STORE USE " << *op << '\n';
        }
        errs() << '\n';
      }
    }

    // if a variable is live at the exit point of the instruction,
    //  then it is potentially live at the entry point of the instruction
    liveIn[instr] = liveOut[instr];

    for (auto var : uses){
      errs() << "inserting " << *var << '\n';
      liveIn[instr].insert(var);
    }

    // if this instruction kills variables that are live at the exit
    //  point of the instruction, then remove it from the liveIn set
    for (auto var : defs){
      errs() << "erasing " << *var << '\n';
      liveIn[instr].erase(var);
    }

    // clear sets
    defs.clear();
    uses.clear();
  }
  errs() << "============================\n";
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