#pragma once
#include "decompilationUtils.h"

unsigned char getAllLocalRegVars(struct DecompilationParameters* params);

static unsigned char getLocalRegVarsFromConditionalInstructions(struct DecompilationParameters* params);

static unsigned char getLocalRegVarsFromConditions(struct DecompilationParameters* params);

static unsigned char getTempLocalRegVars(struct DecompilationParameters* params);

static void getLocalRegVarScope(struct DecompilationParameters* params, int upperStart, int lowerStart, struct RegisterVariable* regVar);

unsigned char isRegisterAccessedBeforeInit(struct DecompilationParameters* params, int startInstructionIndex, int lastInstructionIndex, enum Register reg, unsigned char ignoreInitialization, int callNum);