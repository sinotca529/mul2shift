#include <llvm/IR/PassManager.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Passes/PassBuilder.h>
#include <unordered_set>

using namespace llvm;

struct Mul2Shift: public PassInfoMixin<Mul2Shift> {
    static bool isRequired() { return true; }

    PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM) {
        std::unordered_set<Instruction*> WorkList;

        for (BasicBlock &BB: F) {
            for (Instruction &I: BB) {
                if (I.getOpcode() != Instruction::Mul) continue;
                auto const *CI = dyn_cast<ConstantInt>(I.getOperand(1));
                if (!CI) continue;
                auto const V = CI->getValue();
                if (V.exactLogBase2() == -1) continue;
                WorkList.insert(&I);
            }
        }

        for (auto *Mul: WorkList) {
            auto Log2Multiplier = cast<ConstantInt>(Mul->getOperand(1))->getValue().exactLogBase2();
            IRBuilder<> Builder(Mul);
            auto *Shl = Builder.CreateShl(Mul->getOperand(0), Log2Multiplier);
            Mul->replaceAllUsesWith(Shl);
            Mul->eraseFromParent();
        }

        return PreservedAnalyses::none();
    }
};

extern "C" LLVM_ATTRIBUTE_WEAK
PassPluginLibraryInfo llvmGetPassPluginInfo() {
    return {
        LLVM_PLUGIN_API_VERSION, "Mul2Shift", "v0.1",
        [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](
                    StringRef Name,
                    FunctionPassManager &FPM,
                    ArrayRef<PassBuilder::PipelineElement>
                ) {
                    if (Name == "mul2shift") {
                        FPM.addPass(Mul2Shift());
                        return true;
                    }
                    return false;
                }
            );
        }
    };
}
