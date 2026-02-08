#pragma once
#include "disassemblyStructs.h"

unsigned char handleOpcode(struct DisassemblyParameters* params);

static void handleAlternateMnemonics(struct DisassemblyParameters* params);