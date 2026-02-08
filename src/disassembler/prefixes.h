#pragma once
#include "disassemblyStructs.h"

unsigned char handleLegacyPrefixes(struct DisassemblyParameters* params);

unsigned char handleREXPrefix(struct DisassemblyParameters* params);

unsigned char handleVEXPrefix(struct DisassemblyParameters* params);

unsigned char handleEVEXPrefix(struct DisassemblyParameters* params);