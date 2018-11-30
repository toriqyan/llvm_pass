//===- HW1.cpp - Example code from "Writing an LLVM Pass" ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements two versions of the LLVM "HW1" pass described
// in docs/WritingAnLLVMPass.html
//
//===----------------------------------------------------------------------===//
#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/Support/raw_ostream.h"
// #include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
// #include "llvm/IR/Verifier.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/TypeBuilder.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/Analysis/LoopInfo.h"
// #include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/Twine.h"
// #include "llvm/ADT/StringRef.h"
#include <cstdlib>
#include <cmath>
#include <vector>
using namespace llvm;

#define DEBUG_TYPE "proj"

namespace {
    // HW1 - The first implementation, without getAnalysisUsage.
    struct PROJ : public ModulePass {
        static char ID; // Pass identification, replacement for typeid
        PROJ() : ModulePass(ID) {}

        // bool runOnFunction(Function &F) override {
        //     if(F.isDeclaration())
        //         return false;
        //     BranchProbabilityInfo *BPI = &getAnalysis<BranchProbabilityInfoWrapperPass>().getBPI();
        //     BlockFrequencyInfo *BFI = &getAnalysis<BlockFrequencyInfoWrapperPass>().getBFI();
        //     double totalCount = 0;
        //     double biasedCount = 0;
        //     double unbiasedCount = 0;
        //     double aluCount = 0;
        //     double floatPCount = 0;
        //     double memoryCount = 0;
        //     double otherCount = 0;
        //     for (Function::iterator b = F.begin(); b != F.end(); b++) {
        //         BasicBlock *bb = &(*b);
        //         for(BasicBlock::iterator i_iter = bb->begin(); i_iter != bb->end(); ++i_iter) {
        //             Instruction *I = &(*i_iter);
        //             int count = (BFI->getBlockProfileCount(bb).getValue() < 0) ? 0 : BFI->getBlockProfileCount(bb).getValue();
        //             totalCount += count;

        //             switch(I->getOpcode()) {
        //               // branch
        //               case Instruction::Br:
        //               case Instruction::Switch:
        //               case Instruction::IndirectBr:
        //                 if(BPI->getHotSucc(bb)) {
        //                     biasedCount += count;
        //                 } else {
        //                     unbiasedCount += count;
        //                 }
        //                 break;

        //               // ALU
        //               case Instruction::Add:
        //               case Instruction::Sub:
        //               case Instruction::Mul:
        //               case Instruction::UDiv:
        //               case Instruction::SDiv:
        //               case Instruction::URem:
        //               case Instruction::Shl:
        //               case Instruction::LShr:
        //               case Instruction::AShr:
        //               case Instruction::And:
        //               case Instruction::Or:
        //               case Instruction::Xor:
        //               case Instruction::ICmp:
        //               case Instruction::SRem:
        //                 aluCount += count;
        //                 break;

        //               // floating points
        //               case Instruction::FAdd:
        //               case Instruction::FSub:
        //               case Instruction::FMul:
        //               case Instruction::FDiv:
        //               case Instruction::FRem:
        //               case Instruction::FCmp:
        //                 floatPCount += count;
        //                 break;

        //               // memory
        //               case Instruction::Alloca:
        //               case Instruction::Load:
        //               case Instruction::Store:
        //               case Instruction::GetElementPtr:
        //               case Instruction::Fence:
        //               case Instruction::AtomicCmpXchg:
        //               case Instruction::AtomicRMW:
        //                 memoryCount += count;
        //                 break;

        //               default:
        //                 otherCount += count;
        //                 break;
        //             }
        //         }
        //     }

        //     errs().write_escaped(F.getName()) << ",";
        //     errs() << totalCount << ",";
        //     if (totalCount == 0.0) {
        //       errs() << "0,0,0,0,0,0\n";
        //     } else { 
        //         errs() << (aluCount/totalCount) << ",";
        //         errs() << (floatPCount/totalCount) << ",";
        //         errs() << (memoryCount/totalCount) << ",";
        //         errs() << (biasedCount/totalCount) << ",";
        //         errs() << (unbiasedCount/totalCount) << ",";
        //         errs() << (otherCount/totalCount) << "\n"; 

        //     }
        //     return true;
        // }

        void getAnalysisUsage(AnalysisUsage &AU) const {
            AU.setPreservesCFG();
            AU.addRequired<LoopInfoWrapperPass>();
        }

        bool runOnModule(Module& M) override {
          LLVMContext &c = M.getContext();
          Type *intTy = TypeBuilder<int,false>::get(c);
          

          for (Module::iterator F = M.begin(), m_end = M.end(); F!=m_end; ++F) {
            // Function *FF = 
            for (auto b = F->begin(), be = F->end(); b != be; ++b) {
              Module *MM = b->getModule();
              BasicBlock* BB = &(*b);
              for (BasicBlock::iterator II = BB->end(); II != BB->begin();) {
                Instruction &I = *--II;
                if (I.getOpcode() == Instruction::Load || I.getOpcode() == Instruction::Store) {
                  IRBuilder<> builder(BB, II);
                  // builder.SetInsertPoint();
                  std::vector<llvm::Type *> args;
                  args.push_back(llvm::Type::getInt8PtrTy(c));
                  llvm::FunctionType *printfType =
                      llvm::FunctionType::get(builder.getInt32Ty(), args, true);
                  llvm::Constant *printfFunc =
                      MM->getOrInsertFunction("printf", printfType);
                  std::vector<llvm::Value *> values;
                  llvm::Value *formatStr = builder.CreateGlobalStringPtr("\n");
                  values.clear();
                  values.push_back(formatStr);
                  builder.CreateCall(printfFunc, values);
                }
              }
            }
          }
        }
    };

}

char PROJ::ID = 0;
static RegisterPass<PROJ> X("proj", "proj Pass");
