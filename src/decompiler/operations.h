#pragma once
#include "decompilationStructs.h"

enum JdcStatus decompileOperation(struct DecompilationParameters* params, int instructionIndex, enum Register targetReg, unsigned char getAssignment, unsigned char notStatusFlag, struct JdcStr* result, unsigned char* placeOperatorInfront);

static enum JdcStatus decompileBinaryOperation(struct DecompilationParameters* params, int instructionIndex, unsigned char getAssignment, const char* regularOperator, const char* assignmentOperator, struct JdcStr* result);

static enum JdcStatus decompileSumStatusFlags(struct DecompilationParameters* params, int instructionIndex, enum Register targetReg, unsigned char getAssignment, unsigned char notStatusFlag, struct JdcStr* result);

static enum JdcStatus decompileIncDec(struct DecompilationParameters* params, int instructionIndex, enum Register targetReg, unsigned char getAssignment, unsigned char notStatusFlag, struct JdcStr* result);

static enum JdcStatus decompileNeg(struct DecompilationParameters* params, int instructionIndex, enum Register targetReg, unsigned char getAssignment, unsigned char notStatusFlag, struct JdcStr* result, unsigned char* placeOperatorInfront);

static enum JdcStatus decompileNot(struct DecompilationParameters* params, int instructionIndex, unsigned char getAssignment, struct JdcStr* result, unsigned char* placeOperatorInfront);

static enum JdcStatus decompileOr(struct DecompilationParameters* params, int instructionIndex, unsigned char getAssignment, struct JdcStr* result);

static enum JdcStatus decompileXor(struct DecompilationParameters* params, int instructionIndex, unsigned char getAssignment, struct JdcStr* result);

static enum JdcStatus decompileBitTest(struct DecompilationParameters* params, int instructionIndex, unsigned char getAssignment, unsigned char notStatusFlag, struct JdcStr* result);

static enum JdcStatus decompileBitSet(struct DecompilationParameters* params, int instructionIndex, enum Register targetReg, unsigned char getAssignment, unsigned char notStatusFlag, struct JdcStr* result);

static enum JdcStatus decompileBitReset(struct DecompilationParameters* params, int instructionIndex, enum Register targetReg, unsigned char getAssignment, unsigned char notStatusFlag, struct JdcStr* result);

static enum JdcStatus decompileADC(struct DecompilationParameters* params, int instructionIndex, unsigned char getAssignment, struct JdcStr* result);

static enum JdcStatus decompileSBB(struct DecompilationParameters* params, int instructionIndex, unsigned char getAssignment, struct JdcStr* result);

static enum JdcStatus decompileFLD(struct DecompilationParameters* params, int instructionIndex, unsigned char getAssignment, struct JdcStr* result);

static enum JdcStatus decompileIDIV(struct DecompilationParameters* params, int instructionIndex, enum Register targetReg, unsigned char getAssignment, struct JdcStr* result);

static enum JdcStatus decompileIMUL(struct DecompilationParameters* params, int instructionIndex, enum Register targetReg, unsigned char getAssignment, struct JdcStr* result);

static enum JdcStatus decompileCMOVcc(struct DecompilationParameters* params, int instructionIndex, unsigned char getAssignment, struct JdcStr* result);

static enum JdcStatus decompileSETcc(struct DecompilationParameters* params, int instructionIndex, unsigned char getAssignment, struct JdcStr* result);

static enum JdcStatus decompilePop(struct DecompilationParameters* params, int instructionIndex, unsigned char getAssignment, struct JdcStr* result);

static enum JdcStatus decompileXCHG(struct DecompilationParameters* params, int instructionIndex, enum Register targetReg, unsigned char getAssignment, struct JdcStr* result);

static enum JdcStatus decompileCMPXCHG(struct DecompilationParameters* params, int instructionIndex, unsigned char getAssignment, struct JdcStr* result);
