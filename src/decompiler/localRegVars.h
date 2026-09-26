#pragma once
#include "decompilationUtils.h"

enum JdcStatus getAllLocalRegVars(struct DecompilationParameters* params);

static enum JdcStatus getLocalRegVarsFromConditionalInstructions(struct DecompilationParameters* params);

static enum JdcStatus getLocalRegVarsFromConditions(struct DecompilationParameters* params);

static enum JdcStatus getTempLocalRegVars(struct DecompilationParameters* params);

static void getLocalRegVarScope(struct DecompilationParameters* params, int upperStart, int lowerStart, struct RegisterVariable* regVar);

unsigned char isRegisterAccessedBeforeInit(struct DecompilationParameters* params, int startInstructionIndex, int lastInstructionIndex, enum Register reg, unsigned char ignoreInitialization, int callNum);