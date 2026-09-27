#pragma once
#include "decompilationUtils.h"

enum JdcStatus getAllLocalRegVars(struct DecompilationParameters* params);

static enum JdcStatus getLocalRegVarsFromConditionalInstructions(struct DecompilationParameters* params);

static enum JdcStatus getLocalRegVarsFromConditions(struct DecompilationParameters* params);

static enum JdcStatus getTempLocalRegVars(struct DecompilationParameters* params);

static void getLocalRegVarScope(struct DecompilationParameters* params, int32_t upperStart, int32_t lowerStart, struct RegisterVariable* regVar);

bool isRegisterAccessedBeforeInit(struct DecompilationParameters* params, int32_t startInstructionIndex, int32_t lastInstructionIndex, enum Register reg, bool ignoreInitialization, int32_t callNum);