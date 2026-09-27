#pragma once
#include "disassemblyStructs.h"

enum JdcStatus handleLegacyPrefixes(struct DisassemblyParameters* params);

enum Segment segmentOverrideToSegment(enum LegacyPrefix group2Prefix);

enum JdcStatus handleREXPrefix(struct DisassemblyParameters* params);

enum JdcStatus handleVEXPrefix(struct DisassemblyParameters* params);

enum JdcStatus handleEVEXPrefix(struct DisassemblyParameters* params);

bool checkFlagR(struct DisassemblyParameters* params);

bool checkFlagB(struct DisassemblyParameters* params);