#pragma once
#include "decompilationParams.h"

unsigned char doesInstructionModifyReturnRegister(struct DecompilationParameters params);

unsigned char checkForReturnStatement(struct DecompilationParameters params);

unsigned char decompileReturnStatement(struct DecompilationParameters params, struct JdcStr* result);
