#pragma once
#include "disassemblyStructs.h"

unsigned char handleLegacyPrefixes(struct DisassemblyParameters* params);

enum Segment segmentOverrideToSegment(enum LegacyPrefix group2Prefix);

unsigned char handleREXPrefix(struct DisassemblyParameters* params);

unsigned char handleVEXPrefix(struct DisassemblyParameters* params);

unsigned char handleEVEXPrefix(struct DisassemblyParameters* params);

unsigned char checkFlagB(struct DisassemblyParameters* params);