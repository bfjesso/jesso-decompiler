#pragma once
#include "decompilationStructs.h"

unsigned char decompileOperation(struct DecompilationParameters* params, enum Register targetReg, unsigned char getAssignment, int dstOperandIndex, struct JdcStr* result);

static unsigned char decompileBinaryOperation(struct DecompilationParameters* params, unsigned char getAssignment, const char* regularOperator, const char* assignmentOperator, struct JdcStr* result);

static unsigned char decompileLEA(struct DecompilationParameters* params, unsigned char getAssignment, struct JdcStr* result);

static unsigned char decompileInc(struct DecompilationParameters* params, unsigned char getAssignment, struct JdcStr* result);

static unsigned char decompileDec(struct DecompilationParameters* params, unsigned char getAssignment, struct JdcStr* result);

static unsigned char decompileNeg(struct DecompilationParameters* params, unsigned char getAssignment, struct JdcStr* result);

static unsigned char decompileXor(struct DecompilationParameters* params, unsigned char getAssignment, struct JdcStr* result);

static unsigned char decompileFLD(struct DecompilationParameters* params, unsigned char getAssignment, struct JdcStr* result);

static unsigned char decompileIDIV(struct DecompilationParameters* params, unsigned char getAssignment, struct JdcStr* result);

static unsigned char decompileIMUL(struct DecompilationParameters* params, enum Register targetReg, unsigned char getAssignment, struct JdcStr* result);

static unsigned char decompileCMOVcc(struct DecompilationParameters* params, unsigned char getAssignment, struct JdcStr* result);

static unsigned char decompileSETcc(struct DecompilationParameters* params, unsigned char getAssignment, struct JdcStr* result);

static unsigned char decompilePop(struct DecompilationParameters* params, unsigned char getAssignment, struct JdcStr* result);

static unsigned char decompileXCHG(struct DecompilationParameters* params, enum Register targetReg, unsigned char getAssignment, int dstOperandIndex, struct JdcStr* result);