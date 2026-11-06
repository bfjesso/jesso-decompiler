#pragma once
#include "disassemblyStructs.h"

enum JdcStatus handleOpcode(struct DisassemblyParameters* params);

static void handleAlternateMnemonics(struct DisassemblyParameters* params);