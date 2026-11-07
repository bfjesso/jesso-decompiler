#include "operands.h"
#include "prefixes.h"
#include "modRM.h"
#include "registers.h"

#include <stdlib.h>

enum JdcStatus handleOperands(struct DisassemblyParameters* params, struct DisassembledInstruction* result)
{
	struct Operand operands[4] = { 0 };

	if (params->evexPrefix.isValidEVEX && params->evexPrefix.aaa != 0)
	{
		result->opmask = (params->evexPrefix.aaa + K0_X86_REG);
		result->zeroMasking = params->evexPrefix.z;
	}

	bool is64BitOperandSize = false;
	if (params->is64BitMode)
	{
		if ((params->opcode.opcodeSuperscript == d64 && params->legPrefixes.group3 != OSO) ||
			params->opcode.opcodeSuperscript == f64 ||
			checkFlagW(params))
		{
			is64BitOperandSize = true;
		}
	}

	if (is64BitOperandSize)
	{
		result->operandSizeAttribute = 8;
	}
	else if (params->legPrefixes.group3 != OSO)
	{
		result->operandSizeAttribute = 4;
	}
	else
	{
		result->operandSizeAttribute = 2;
	}

	uint8_t vectorLength = getVectorLength(params);

	int32_t operandIndex = 0;
	for (int32_t i = 0; i < 4; i++)
	{
		enum OperandCode currentOperandCode = params->opcode.operands[i];
		struct Operand* currentOperand = &(operands[operandIndex]);

		if (currentOperandCode == NONE_OC)
		{
			break;
		}

		int64_t immediateTmp = 0; // used for Lx and Ap
		switch (currentOperandCode)
		{
		case ONE_OC:
			currentOperand->type = IMMEDIATE;
			currentOperand->immediate.value = 1;
			currentOperand->immediate.size = 1;
			break;
		case AL_X86_OC:
			currentOperand->type = REGISTER;
			currentOperand->reg = AL_X86_REG;
			break;
		case CL_X86_OC:
			currentOperand->type = REGISTER;
			currentOperand->reg = CL_X86_REG;
			break;
		case AX_X86_OC:
			currentOperand->type = REGISTER;
			currentOperand->reg = AX_X86_REG;
			break;
		case DX_X86_OC:
			currentOperand->type = REGISTER;
			currentOperand->reg = DX_X86_REG;
			break;
		case ES_X86_OC:
			currentOperand->type = REGISTER;
			currentOperand->reg = ES_X86_REG;
			break;
		case CS_X86_OC:
			currentOperand->type = REGISTER;
			currentOperand->reg = CS_X86_REG;
			break;
		case SS_X86_OC:
			currentOperand->type = REGISTER;
			currentOperand->reg = SS_X86_REG;
			break;
		case DS_X86_OC:
			currentOperand->type = REGISTER;
			currentOperand->reg = DS_X86_REG;
			break;
		case FS_X86_OC:
			currentOperand->type = REGISTER;
			currentOperand->reg = FS_X86_REG;
			break;
		case GS_X86_OC:
			currentOperand->type = REGISTER;
			currentOperand->reg = GS_X86_REG;
			break;
		case AX_WDQ_X86_OC:
			currentOperand->type = REGISTER;
			currentOperand->reg = is64BitOperandSize ? RAX_X86_REG : (params->legPrefixes.group3 == OSO ? AX_X86_REG : EAX_X86_REG);
			break;
		case CX_WDQ_X86_OC:
			currentOperand->type = REGISTER;
			currentOperand->reg = is64BitOperandSize ? RCX_X86_REG : (params->legPrefixes.group3 == OSO ? CX_X86_REG : ECX_X86_REG);
			break;
		case DX_WDQ_X86_OC:
			currentOperand->type = REGISTER;
			currentOperand->reg = is64BitOperandSize ? RDX_X86_REG : (params->legPrefixes.group3 == OSO ? DX_X86_REG : EDX_X86_REG);
			break;
		case BX_WDQ_X86_OC:
			currentOperand->type = REGISTER;
			currentOperand->reg = is64BitOperandSize ? RBX_X86_REG : (params->legPrefixes.group3 == OSO ? BX_X86_REG : EBX_X86_REG);
			break;
		case SP_WDQ_X86_OC:
			currentOperand->type = REGISTER;
			currentOperand->reg = is64BitOperandSize ? RSP_X86_REG : (params->legPrefixes.group3 == OSO ? SP_X86_REG : ESP_X86_REG);
			break;
		case BP_WDQ_X86_OC:
			currentOperand->type = REGISTER;
			currentOperand->reg = is64BitOperandSize ? RBP_X86_REG : (params->legPrefixes.group3 == OSO ? BP_X86_REG : EBP_X86_REG);
			break;
		case SI_WDQ_X86_OC:
			currentOperand->type = REGISTER;
			currentOperand->reg = is64BitOperandSize ? RSI_X86_REG : (params->legPrefixes.group3 == OSO ? SI_X86_REG : ESI_X86_REG);
			break;
		case DI_WDQ_X86_OC:
			currentOperand->type = REGISTER;
			currentOperand->reg = is64BitOperandSize ? RDI_X86_REG : (params->legPrefixes.group3 == OSO ? DI_X86_REG : EDI_X86_REG);
			break;
		case AX_R8_WDQ_X86_OC:
			currentOperand->type = REGISTER;
			currentOperand->reg = params->rexPrefix.B ? (is64BitOperandSize ? R8_X86_REG : R8D_X86_REG) : (is64BitOperandSize ? RAX_X86_REG : (params->legPrefixes.group3 != OSO ? EAX_X86_REG : AX_X86_REG));
			break;
		case CX_R9_WDQ_X86_OC:
			currentOperand->type = REGISTER;
			currentOperand->reg = params->rexPrefix.B ? (is64BitOperandSize ? R9_X86_REG : R9D_X86_REG) : (is64BitOperandSize ? RCX_X86_REG : (params->legPrefixes.group3 != OSO ? ECX_X86_REG : CX_X86_REG));
			break;
		case DX_R10_WDQ_X86_OC:
			currentOperand->type = REGISTER;
			currentOperand->reg = params->rexPrefix.B ? (is64BitOperandSize ? R10_X86_REG : R10D_X86_REG) : (is64BitOperandSize ? RDX_X86_REG : (params->legPrefixes.group3 != OSO ? EDX_X86_REG : DX_X86_REG));
			break;
		case BX_R11_WDQ_X86_OC:
			currentOperand->type = REGISTER;
			currentOperand->reg = params->rexPrefix.B ? (is64BitOperandSize ? R11_X86_REG : R11D_X86_REG) : (is64BitOperandSize ? RBX_X86_REG : (params->legPrefixes.group3 != OSO ? EBX_X86_REG : BX_X86_REG));
			break;
		case SP_R12_WDQ_X86_OC:
			currentOperand->type = REGISTER;
			currentOperand->reg = params->rexPrefix.B ? (is64BitOperandSize ? R12_X86_REG : R12D_X86_REG) : (is64BitOperandSize ? RSP_X86_REG : (params->legPrefixes.group3 != OSO ? ESP_X86_REG : SP_X86_REG));
			break;
		case BP_R13_WDQ_X86_OC:
			currentOperand->type = REGISTER;
			currentOperand->reg = params->rexPrefix.B ? (is64BitOperandSize ? R13_X86_REG : R13D_X86_REG) : (is64BitOperandSize ? RBP_X86_REG : (params->legPrefixes.group3 != OSO ? EBP_X86_REG : BP_X86_REG));
			break;
		case SI_R14_WDQ_X86_OC:
			currentOperand->type = REGISTER;
			currentOperand->reg = params->rexPrefix.B ? (is64BitOperandSize ? R14_X86_REG : R14D_X86_REG) : (is64BitOperandSize ? RSI_X86_REG : (params->legPrefixes.group3 != OSO ? ESI_X86_REG : SI_X86_REG));
			break;
		case DI_R15_WDQ_X86_OC:
			currentOperand->type = REGISTER;
			currentOperand->reg = params->rexPrefix.B ? (is64BitOperandSize ? R15_X86_REG : R15D_X86_REG) : (is64BitOperandSize ? RDI_X86_REG : (params->legPrefixes.group3 != OSO ? EDI_X86_REG : DI_X86_REG));
			break;
		case AL_R8B_X86_OC:
			currentOperand->type = REGISTER;
			currentOperand->reg = params->rexPrefix.B ? R8B_X86_REG : AL_X86_REG;
			break;
		case CL_R9B_X86_OC:
			currentOperand->type = REGISTER;
			currentOperand->reg = params->rexPrefix.B ? R9B_X86_REG : CL_X86_REG;
			break;
		case DL_R10B_X86_OC:
			currentOperand->type = REGISTER;
			currentOperand->reg = params->rexPrefix.B ? R10B_X86_REG : DL_X86_REG;
			break;
		case BL_R11B_X86_OC:
			currentOperand->type = REGISTER;
			currentOperand->reg = params->rexPrefix.B ? R11B_X86_REG : BL_X86_REG;
			break;
		case AH_R12B_X86_OC:
			currentOperand->type = REGISTER;
			currentOperand->reg = params->rexPrefix.B ? R12B_X86_REG : AH_X86_REG;
			break;
		case CH_R13B_X86_OC:
			currentOperand->type = REGISTER;
			currentOperand->reg = params->rexPrefix.B ? R13B_X86_REG : CH_X86_REG;
			break;
		case DH_R14B_X86_OC:
			currentOperand->type = REGISTER;
			currentOperand->reg = params->rexPrefix.B ? R14B_X86_REG : DH_X86_REG;
			break;
		case BH_R15B_X86_OC:
			currentOperand->type = REGISTER;
			currentOperand->reg = params->rexPrefix.B ? R15B_X86_REG : BH_X86_REG;
			break;
		case ST0_X86_OC:
			currentOperand->type = REGISTER;
			currentOperand->reg = ST0_X86_REG;
			break;
		case ST1_X86_OC:
			currentOperand->type = REGISTER;
			currentOperand->reg = ST1_X86_REG;
			break;
		case ST2_X86_OC:
			currentOperand->type = REGISTER;
			currentOperand->reg = ST2_X86_REG;
			break;
		case ST3_X86_OC:
			currentOperand->type = REGISTER;
			currentOperand->reg = ST3_X86_REG;
			break;
		case ST4_X86_OC:
			currentOperand->type = REGISTER;
			currentOperand->reg = ST4_X86_REG;
			break;
		case ST5_X86_OC:
			currentOperand->type = REGISTER;
			currentOperand->reg = ST5_X86_REG;
			break;
		case ST6_X86_OC:
			currentOperand->type = REGISTER;
			currentOperand->reg = ST6_X86_REG;
			break;
		case ST7_X86_OC:
			currentOperand->type = REGISTER;
			currentOperand->reg = ST7_X86_REG;
			break;
		case RM_B_X86_OC:
			if (ERROR_JDC == handleModRM(params, RM_MODRM, 1, currentOperand)) { return ERROR_JDC; }
			break;
		case RM_D_X86_OC:
			if (ERROR_JDC == handleModRM(params, RM_MODRM, 4, currentOperand)) { return ERROR_JDC; }
			break;
		case RM_WDQ_X86_OC:
			if (ERROR_JDC == handleModRM(params, RM_MODRM, is64BitOperandSize ? 8 : params->legPrefixes.group3 != OSO ? 4 : 2, currentOperand)) { return ERROR_JDC; }
			break;
		case RM_W_X86_OC:
			if (ERROR_JDC == handleModRM(params, RM_MODRM, 2, currentOperand)) { return ERROR_JDC; }
			break;
		case RM_DF_X86_OC: // check
			if (ERROR_JDC == handleModRM(params, RM_MODRM, params->legPrefixes.group3 == OSO ? 4 : 6, currentOperand)) { return ERROR_JDC; }
			break;
		case RM_DQ_X86_OC:
			if (ERROR_JDC == handleModRM(params, RM_MODRM, is64BitOperandSize ? 8 : 4, currentOperand)) { return ERROR_JDC; }
			break;
		case RM_64MODE_X86_OC:
			if (ERROR_JDC == handleModRM(params, RM_MODRM, params->is64BitMode ? 8 : 4, currentOperand)) { return ERROR_JDC; }
			break;
		case REG_B_X86_OC:
			if (ERROR_JDC == handleModRM(params, REG_MODRM, 1, currentOperand)) { return ERROR_JDC; }
			break;
		case REG_D_X86_OC:
			if (ERROR_JDC == handleModRM(params, REG_MODRM, 4, currentOperand)) { return ERROR_JDC; }
			break;
		case REG_WDQ_X86_OC:
			if (ERROR_JDC == handleModRM(params, REG_MODRM, is64BitOperandSize ? 8 : params->legPrefixes.group3 != OSO ? 4 : 2, currentOperand)) { return ERROR_JDC; }
			break;
		case REG_WD_X86_OC:
			if (ERROR_JDC == handleModRM(params, REG_MODRM, params->legPrefixes.group3 == OSO ? 2 : 4, currentOperand)) { return ERROR_JDC; }
			break;
		case REG_W_X86_OC:
			if (ERROR_JDC == handleModRM(params, REG_MODRM, 2, currentOperand)) { return ERROR_JDC; }
			break;
		case REG_DQ_X86_OC:
			if (ERROR_JDC == handleModRM(params, REG_MODRM, is64BitOperandSize ? 8 : 4, currentOperand)) { return ERROR_JDC; }
			break;
		case REG_64MODE_X86_OC:
			if (ERROR_JDC == handleModRM(params, REG_MODRM, params->is64BitMode ? 8 : 4, currentOperand)) { return ERROR_JDC; }
			break;
		case VEX_VVVV_DQ_X86_OC:
			if (!params->vexPrefix.isValidVEX) { return ERROR_JDC; }
			if (ERROR_JDC == getRegFromVVVV(params, is64BitOperandSize ? 8 : 4, currentOperand)) { return ERROR_JDC; }
			break;
		case M_B_X86_OC:
			if (ERROR_JDC == handleModRM(params, M_MODRM, 1, currentOperand)) { return ERROR_JDC; }
			break;
		case M_W_X86_OC:
			if (ERROR_JDC == handleModRM(params, M_MODRM, 2, currentOperand)) { return ERROR_JDC; }
			break;
		case M_D_X86_OC:
			if (ERROR_JDC == handleModRM(params, M_MODRM, 4, currentOperand)) { return ERROR_JDC; }
			break;
		case M_WDQ_X86_OC:
			if (ERROR_JDC == handleModRM(params, M_MODRM, is64BitOperandSize ? 8 : params->legPrefixes.group3 != OSO ? 4 : 2, currentOperand)) { return ERROR_JDC; }
			break;
		case M_OSO_DQ_X86_OC:
			if (ERROR_JDC == handleModRM(params, M_MODRM, params->legPrefixes.group3 == OSO ? 4 : 8, currentOperand)) { return ERROR_JDC; }
			break;
		case M_DF_X86_OC:
			if (ERROR_JDC == handleModRM(params, M_MODRM, params->legPrefixes.group3 == OSO ? 4 : 6, currentOperand)) { return ERROR_JDC; }
			break;
		case M_Q_X86_OC:
			if (ERROR_JDC == handleModRM(params, M_MODRM, 8, currentOperand)) { return ERROR_JDC; }
			break;
		case M_T_X86_OC:
			if (ERROR_JDC == handleModRM(params, M_MODRM, 10, currentOperand)) { return ERROR_JDC; }
			break;
		case M_XYZ_X86_OC:
			if (ERROR_JDC == handleModRM(params, M_MODRM, vectorLength, currentOperand)) { return ERROR_JDC; }
			break;
		case M_X_X86_OC:
			if (ERROR_JDC == handleModRM(params, M_MODRM, 16, currentOperand)) { return ERROR_JDC; }
			break;
		case M_DQ_X86_OC:
			if (ERROR_JDC == handleModRM(params, M_MODRM, is64BitOperandSize ? 8 : 4, currentOperand)) { return ERROR_JDC; }
			break;
		case IMM_B_X86_OC:
			if (ERROR_JDC == getImmediate(params, 1, currentOperand)) { return ERROR_JDC; }
			break;
		case IMM_WDQ_X86_OC:
			if (ERROR_JDC == getImmediate(params, is64BitOperandSize ? 8 : params->legPrefixes.group3 != OSO ? 4 : 2, currentOperand)) { return ERROR_JDC; }
			break;
		case IMM_WD_X86_OC:
			if (ERROR_JDC == getImmediate(params, params->legPrefixes.group3 == OSO ? 2 : 4, currentOperand)) { return ERROR_JDC; }
			break;
		case IMM_W_X86_OC:
			if (ERROR_JDC == getImmediate(params, 2, currentOperand)) { return ERROR_JDC; }
			break;
		case ES_MEM_DI_B_X86_OC:
			currentOperand->type = MEM_ADDRESS;
			currentOperand->memoryAddress.ptrSize = 1;
			currentOperand->memoryAddress.scale = 1;
			currentOperand->memoryAddress.regSegment = ES_X86_REG;
			currentOperand->memoryAddress.reg = params->legPrefixes.group4 != ASO ? (params->is64BitMode ? RDI_X86_REG : EDI_X86_REG) : (params->is64BitMode ? EDI_X86_REG : DI_X86_REG);
			break;
		case ES_MEM_DI_WDQ_X86_OC:
			currentOperand->type = MEM_ADDRESS;
			currentOperand->memoryAddress.ptrSize = is64BitOperandSize ? 8 : params->legPrefixes.group3 != OSO ? 4 : 2;
			currentOperand->memoryAddress.scale = 1;
			currentOperand->memoryAddress.regSegment = ES_X86_REG;
			currentOperand->memoryAddress.reg = params->legPrefixes.group4 != ASO ? (params->is64BitMode ? RDI_X86_REG : EDI_X86_REG) : (params->is64BitMode ? EDI_X86_REG : DI_X86_REG);
			break;
		case ES_MEM_DI_WD_X86_OC:
			currentOperand->type = MEM_ADDRESS;
			currentOperand->memoryAddress.ptrSize = params->legPrefixes.group3 == OSO ? 2 : 4;
			currentOperand->memoryAddress.scale = 1;
			currentOperand->memoryAddress.regSegment = ES_X86_REG;
			currentOperand->memoryAddress.reg = params->legPrefixes.group4 != ASO ? (params->is64BitMode ? RDI_X86_REG : EDI_X86_REG) : (params->is64BitMode ? EDI_X86_REG : DI_X86_REG);
			break;
		case DS_MEM_SI_B_X86_OC:
			currentOperand->type = MEM_ADDRESS;
			currentOperand->memoryAddress.ptrSize = 1;
			currentOperand->memoryAddress.scale = 1;
			currentOperand->memoryAddress.regSegment = DS_X86_REG;
			currentOperand->memoryAddress.reg = params->legPrefixes.group4 != ASO ? (params->is64BitMode ? RSI_X86_REG : ESI_X86_REG) : (params->is64BitMode ? ESI_X86_REG : SI_X86_REG);
			break;
		case DS_MEM_SI_WDQ_X86_OC:
			currentOperand->type = MEM_ADDRESS;
			currentOperand->memoryAddress.ptrSize = is64BitOperandSize ? 8 : params->legPrefixes.group3 != OSO ? 4 : 2;
			currentOperand->memoryAddress.scale = 1;
			currentOperand->memoryAddress.regSegment = DS_X86_REG;
			currentOperand->memoryAddress.reg = params->legPrefixes.group4 != ASO ? (params->is64BitMode ? RSI_X86_REG : ESI_X86_REG) : (params->is64BitMode ? ESI_X86_REG : SI_X86_REG);
			break;
		case DS_MEM_SI_WD_X86_OC:
			currentOperand->type = MEM_ADDRESS;
			currentOperand->memoryAddress.ptrSize = params->legPrefixes.group3 == OSO ? 2 : 4;
			currentOperand->memoryAddress.scale = 1;
			currentOperand->memoryAddress.regSegment = DS_X86_REG;
			currentOperand->memoryAddress.reg = params->legPrefixes.group4 != ASO ? (params->is64BitMode ? RSI_X86_REG : ESI_X86_REG) : (params->is64BitMode ? ESI_X86_REG : SI_X86_REG);
			break;
		case CONST_MEM_B_X86_OC:
			if (ERROR_JDC == readIntFromBytes(params, params->legPrefixes.group4 != ASO ? (params->is64BitMode ? 8 : 4) : (params->is64BitMode ? 4 : 2), &currentOperand->memoryAddress.constDisplacement)) { return ERROR_JDC; }
			currentOperand->type = MEM_ADDRESS;
			currentOperand->memoryAddress.ptrSize = 1;
			currentOperand->memoryAddress.scale = 1;
			currentOperand->memoryAddress.regSegment = params->legPrefixes.group2 == NO_PREFIX ? DS_X86_REG : segmentOverrideToSegment(params->legPrefixes.group2);
			break;
		case CONST_MEM_WDQ_X86_OC:
			if (ERROR_JDC == readIntFromBytes(params, params->legPrefixes.group4 != ASO ? (params->is64BitMode ? 8 : 4) : (params->is64BitMode ? 4 : 2), &currentOperand->memoryAddress.constDisplacement)) { return ERROR_JDC; }
			currentOperand->type = MEM_ADDRESS;
			currentOperand->memoryAddress.ptrSize = is64BitOperandSize ? 8 : params->legPrefixes.group3 != OSO ? 4 : 2;
			currentOperand->memoryAddress.scale = 1;
			currentOperand->memoryAddress.regSegment = params->legPrefixes.group2 == NO_PREFIX ? DS_X86_REG : segmentOverrideToSegment(params->legPrefixes.group2);
			break;
		case RM_REG_64MODE_X86_OC:
			if (ERROR_JDC == handleModRM(params, RM_REG_MODRM, params->is64BitMode ? 8 : 4, currentOperand)) { return ERROR_JDC; }
			break;
		case RM_REG_DQ_X86_OC:
			if (ERROR_JDC == handleModRM(params, RM_REG_MODRM, is64BitOperandSize ? 8 : 4, currentOperand)) { return ERROR_JDC; }
			break;
		case RM_REG_WDQ_X86_OC:
			if (ERROR_JDC == handleModRM(params, RM_REG_MODRM, is64BitOperandSize ? 8 : params->legPrefixes.group3 != OSO ? 4 : 2, currentOperand)) { return ERROR_JDC; }
			break;
		case REG_CTRL_X86_OC:
			if (ERROR_JDC == handleModRM(params, REG_CONTROL_MODRM, params->is64BitMode ? 8 : 4, currentOperand)) { return ERROR_JDC; }
			break;
		case REG_DEBUG_X86_OC:
			if (ERROR_JDC == handleModRM(params, REG_DEBUG_MODRM, 8, currentOperand)) { return ERROR_JDC; }
			break;
		case R_WDQ_M_W_X86_OC:
			if (ERROR_JDC == handleModRM(params, RM_MW_MODRM, is64BitOperandSize ? 8 : params->legPrefixes.group3 != OSO ? 4 : 2, currentOperand)) { return ERROR_JDC; }
			break;
		case REG_SEGMENT_X86_OC:
			if (ERROR_JDC == handleModRM(params, REG_SEG_MODRM, 2, currentOperand)) { return ERROR_JDC; }
			break;
		case CONST_SEG_MEM_X86_OC:
			if (ERROR_JDC == readIntFromBytes(params, params->legPrefixes.group3 != OSO ? 4 : 2, &currentOperand->memoryAddress.constDisplacement)) { return ERROR_JDC; }
			if (ERROR_JDC == readIntFromBytes(params, 2, &immediateTmp)) { return ERROR_JDC; }
			currentOperand->type = MEM_ADDRESS;
			currentOperand->memoryAddress.scale = 1;
			currentOperand->memoryAddress.constSegment = (uint16_t)immediateTmp;
			break;
		case REG_MMX_X86_OC:
			if (ERROR_JDC == handleModRM(params, REG_MMX_MODRM, 8, currentOperand)) { return ERROR_JDC; }
			break;
		case RM_MMX_D_X86_OC:
			if (ERROR_JDC == handleModRM(params, RM_MMX_MODRM, 4, currentOperand)) { return ERROR_JDC; }
			break;
		case RM_MMX_Q_X86_OC:
			if (ERROR_JDC == handleModRM(params, RM_MMX_MODRM, 8, currentOperand)) { return ERROR_JDC; }
			break;
		case RM_MOD11_MMX_X86_OC:
			if (ERROR_JDC == handleModRM(params, RM_MOD11_MMX_MODRM, 8, currentOperand)) { return ERROR_JDC; }
			break;
		case RM_MOD11_XYZ_X86_OC:
			if (ERROR_JDC == handleModRM(params, RM_MOD11_REG_MODRM, vectorLength, currentOperand)) { return ERROR_JDC; }
			break;
		case RM_MOD11_X_X86_OC:
			if (ERROR_JDC == handleModRM(params, RM_MOD11_REG_MODRM, 16, currentOperand)) { return ERROR_JDC; }
			break;
		case RM_MOD11_REQ_X_X86_OC:
			if (vectorLength != 16) { return ERROR_JDC; }
			if (ERROR_JDC == handleModRM(params, RM_MOD11_REG_MODRM, 16, currentOperand)) { return ERROR_JDC; }
			break;
		case REG_XYZ_X86_OC:
			if (ERROR_JDC == handleModRM(params, REG_MODRM, vectorLength, currentOperand)) { return ERROR_JDC; }
			break;
		case REG_REQ_X_X86_OC:
			if (vectorLength != 16) { return ERROR_JDC; }
			if (ERROR_JDC == handleModRM(params, REG_MODRM, 16, currentOperand)) { return ERROR_JDC; }
			break;
		case REG_X_X86_OC:
			if (ERROR_JDC == handleModRM(params, REG_MODRM, 16, currentOperand)) { return ERROR_JDC; }
			break;
		case REG_REQ_YZ_X86_OC:
			if (vectorLength == 16) { return ERROR_JDC; }
			if (ERROR_JDC == handleModRM(params, REG_MODRM, vectorLength, currentOperand)) { return ERROR_JDC; }
			break;
		case RM_XYZ_X86_OC:
			if (ERROR_JDC == handleModRM(params, RM_MODRM, vectorLength, currentOperand)) { return ERROR_JDC; }
			break;
		case RM_REQ_X_X86_OC:
			if (vectorLength != 16) { return ERROR_JDC; }
			if (ERROR_JDC == handleModRM(params, RM_MODRM, 16, currentOperand)) { return ERROR_JDC; }
			break;
		case R_X_M_D_X86_OC:
			if (ERROR_JDC == handleModRM(params, RM_MD_MODRM, 16, currentOperand)) { return ERROR_JDC; }
			break;
		case R_X_M_Q_X86_OC:
			if (ERROR_JDC == handleModRM(params, RM_MQ_MODRM, 16, currentOperand)) { return ERROR_JDC; }
			break;
		case RM_REQ_YZ_X86_OC:
			if (vectorLength == 16) { return ERROR_JDC; }
			if (ERROR_JDC == handleModRM(params, RM_MODRM, vectorLength, currentOperand)) { return ERROR_JDC; }
			break;
		case VVVV_XYZ_X86_OC:
			if (!params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX) { operandIndex--; break; }
			if (ERROR_JDC == getRegFromVVVV(params, vectorLength, currentOperand)) { return ERROR_JDC; }
			break;
		case VVVV_REQ_X_X86_OC:
			if (!params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX) { operandIndex--; break; }
			if (vectorLength != 16) { return ERROR_JDC; }
			if (ERROR_JDC == getRegFromVVVV(params, 16, currentOperand)) { return ERROR_JDC; }
			break;
		case VVVV_X_X86_OC:
			if (!params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX) { operandIndex--; break; }
			if (ERROR_JDC == getRegFromVVVV(params, 16, currentOperand)) { return ERROR_JDC; }
			break;
		case VVVV_REQ_YZ_X86_OC:
			if (!params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX) { operandIndex--; break; }
			if (vectorLength == 16) { return ERROR_JDC; }
			if (ERROR_JDC == getRegFromVVVV(params, vectorLength, currentOperand)) { return ERROR_JDC; }
			break;
		case U4_XY_X86_OC:
			if (ERROR_JDC == readIntFromBytes(params, 1, &immediateTmp)) { return ERROR_JDC; }
			immediateTmp = (immediateTmp >> 4) & 0xF; // upper 4 bits
			currentOperand->type = REGISTER;
			currentOperand->reg = vectorLength == 32 ? (YMM0_X86_REG + immediateTmp) : (XMM0_X86_REG + immediateTmp);
			break;
		case EVEX_VVVV_XYZ_X86_OC:
			if (!params->evexPrefix.isValidEVEX) { return ERROR_JDC; }
			if (ERROR_JDC == getRegFromVVVV(params, vectorLength, currentOperand)) { return ERROR_JDC; }
			break;
		}

		if (currentOperandCode != NONE_OC)
		{
			currentOperand = &(operands[operandIndex]); // operandIndex may have been decremented
			if (currentOperand->type == NO_OPERAND)
			{
				return ERROR_JDC;
			}
			else if (currentOperand->type == REGISTER && (currentOperand->reg < 0 || currentOperand->reg >= NUM_OF_REGISTERS))
			{
				return ERROR_JDC;
			}
			else if (currentOperand->type == MEM_ADDRESS &&
				(currentOperand->memoryAddress.reg < 0 || currentOperand->memoryAddress.reg >= NUM_OF_REGISTERS ||
					currentOperand->memoryAddress.regDisplacement < 0 || currentOperand->memoryAddress.regDisplacement >= NUM_OF_REGISTERS ||
					currentOperand->memoryAddress.regSegment < 0 || currentOperand->memoryAddress.regSegment >= NUM_OF_REGISTERS))
			{
				return ERROR_JDC;
			}
		}

		operandIndex++;
	}

	if (operandIndex > 0)
	{
		result->operands = (struct Operand*)calloc(operandIndex, sizeof(struct Operand));
		if (!result->operands)
		{
			return ERROR_JDC;
		}

		for (int32_t i = 0; i < operandIndex; i++)
		{
			result->operands[i] = operands[i];
		}
	}

	result->numOfOperands = operandIndex;
	return SUCCESS_JDC;
}

static enum JdcStatus getImmediate(struct DisassemblyParameters* params, uint8_t immediateSize, struct Operand* result)
{
	if (ERROR_JDC == readIntFromBytes(params, immediateSize, &result->immediate.value))
	{
		return ERROR_JDC;
	}

	result->type = IMMEDIATE;
	result->immediate.size = immediateSize;
	return SUCCESS_JDC;
}

enum JdcStatus readIntFromBytes(struct DisassemblyParameters* params, uint8_t sizeOfInt, int64_t* result)
{
	if (!result || sizeOfInt > 8 || (params->bytes + (sizeOfInt - 1)) > params->maxBytesAddr)
	{
		return ERROR_JDC;
	}

	*result = 0;
	for (int32_t i = 0; i < sizeOfInt; i++)
	{
		*result += (int64_t)(params->bytes[0] << (8 * i));
		params->bytes++;
	}

	return SUCCESS_JDC;
}

uint8_t getVectorLength(struct DisassemblyParameters* params)
{
	if (params->evexPrefix.LL == 0b10)
	{
		return 64;
	}
	else if (params->vexPrefix.L || params->evexPrefix.LL == 0b01)
	{
		return 32;
	}

	return 16;
}

static enum JdcStatus getRegFromVVVV(struct DisassemblyParameters* params, uint8_t regSize, struct Operand* result)
{
	if (!params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX)
	{
		return ERROR_JDC;
	}

	uint8_t vvvv = params->vexPrefix.isValidVEX ? params->vexPrefix.vvvv : params->evexPrefix.vvvv;
	if (!params->is64BitMode)
	{
		if (params->evexPrefix.isValidEVEX && !params->evexPrefix.V_prime)
		{
			return ERROR_JDC;
		}

		// the extended registers are not accesible outside 64 bit mode
		if (regSize > 8 && params->vexPrefix.isValidVEX && (vvvv & 0b1000) == 0)
		{
			return ERROR_JDC;
		}

		vvvv |= 0b1000; // the most significant bit is ignored if the reg size is 4 or 8, or if EVEX is used
	}

	result->type = REGISTER;
	switch (regSize)
	{
	case 4:
		result->reg = (R15D_X86_REG - vvvv);
		if (result->reg <= EIP_X86_REG) { result->reg--; } // the instruction pointer cannot be encoded by vvvv
		break;
	case 8:
		result->reg = (R15_X86_REG - vvvv);
		if (result->reg <= RIP_X86_REG) { result->reg--; }
		break;
	case 16:
		result->reg = params->evexPrefix.isValidEVEX && !params->evexPrefix.V_prime ? (XMM31_X86_REG - vvvv) : (XMM15_X86_REG - vvvv);
		break;
	case 32:
		result->reg = params->evexPrefix.isValidEVEX && !params->evexPrefix.V_prime ? (YMM31_X86_REG - vvvv) : (YMM15_X86_REG - vvvv);
		break;
	case 64:
		result->reg = params->evexPrefix.isValidEVEX && !params->evexPrefix.V_prime ? (ZMM31_X86_REG - vvvv) : (ZMM15_X86_REG - vvvv);
		break;
	}

	return SUCCESS_JDC;
}