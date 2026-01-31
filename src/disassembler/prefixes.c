#include "prefixes.h"

unsigned char handleLegacyPrefixes(unsigned char** bytesPtr, unsigned char* maxBytesAddr, struct LegacyPrefixes* result)
{
	result->group1 = NO_PREFIX;
	result->group2 = NO_PREFIX;
	result->group3 = NO_PREFIX;
	result->group4 = NO_PREFIX;

	while (1)
	{
		if ((*bytesPtr) > maxBytesAddr) { return 0; }

		unsigned char byte = (*bytesPtr)[0];
		(*bytesPtr)++;

		switch (byte)
		{
		case 0xF0:
			result->group1 = LOCK;
			break;
		case 0xF2:
			result->group1 = REPNZ;
			break;
		case 0xF3:
			result->group1 = REPZ;
			break;
		case 0x2E:
			result->group2 = CSO_BNT;
			break;
		case 0x36:
			result->group2 = SSO;
			break;
		case 0x3E:
			result->group2 = DSO_BT;
			break;
		case 0x26:
			result->group2 = ESO;
			break;
		case 0x64:
			result->group2 = FSO;
			break;
		case 0x65:
			result->group2 = GSO;
			break;
		case 0x66:
			result->group3 = OSO;
			break;
		case 0x67:
			result->group4 = ASO;
			break;
		default:
			(*bytesPtr)--;
			return 1;
		}
	}

	return 1;
}

unsigned char handleREXPrefix(unsigned char** bytesPtr, unsigned char* maxBytesAddr, struct REXPrefix* result)
{
	if ((*bytesPtr) > maxBytesAddr) { return 0; }

	unsigned char rexByte = (*bytesPtr)[0];

	if (rexByte < 0x40 || rexByte > 0x4F) { return 1; }

	result->isValidREX = 1;
	result->w = (rexByte >> 3) & 0x01;
	result->r = (rexByte >> 2) & 0x01;
	result->x = (rexByte >> 1) & 0x01;
	result->b = (rexByte >> 0) & 0x01;

	(*bytesPtr)++;

	return 1;
}

unsigned char handleVEXPrefix(unsigned char** bytesPtr, unsigned char* maxBytesAddr, struct LegacyPrefixes* legPrefixes, struct VEXPrefix* result)
{
	if ((*bytesPtr) > maxBytesAddr) { return 0; }

	unsigned char byte0 = (*bytesPtr)[0];
	unsigned char byte1 = (*bytesPtr)[1];
	unsigned char byte2 = (*bytesPtr)[2];

	if (byte0 == 0xC5) // two-byte form
	{
		result->isValidVEX = 1;
		result->r = (byte1 >> 7) & 0x01;
		result->vvvv = (((byte1 >> 6) & 0x01) * 8) + (((byte1 >> 5) & 0x01) * 4) + (((byte1 >> 4) & 0x01) * 2) + ((byte1 >> 3) & 0x01);
		result->l = (byte1 >> 2) & 0x01;
		result->pp = (((byte1 >> 1) & 0x01) * 2) + ((byte1 >> 0) & 0x01);

		result->mmmmm = 0b00001;

		(*bytesPtr) += 2;
	}
	else if (byte0 == 0xC4) // three-byte form
	{
		result->isValidVEX = 1;
		result->r = (byte1 >> 7) & 0x01;
		result->x = (byte1 >> 6) & 0x01;
		result->b = (byte1 >> 5) & 0x01;
		result->mmmmm = (((byte1 >> 4) & 0x01) * 16) + (((byte1 >> 3) & 0x01) * 8) + (((byte1 >> 2) & 0x01) * 4) + (((byte1 >> 1) & 0x01) * 2) + ((byte1 >> 0) & 0x01);

		result->w = (byte2 >> 7) & 0x01;
		result->vvvv = (((byte2 >> 6) & 0x01) * 8) + (((byte2 >> 5) & 0x01) * 4) + (((byte2 >> 4) & 0x01) * 2) + ((byte2 >> 3) & 0x01);
		result->l = (byte2 >> 2) & 0x01;
		result->pp = (((byte2 >> 1) & 0x01) * 2) + ((byte2 >> 0) & 0x01);

		(*bytesPtr) += 3;
	}

	if(result->isValidVEX)
	{
		switch(result->pp)
		{
		case 0b01:
			legPrefixes->group3 = OSO; // 0x66
			break;
		case 0b10:
			legPrefixes->group1 = REPZ; // 0xF3
			break;
		case 0b11:
			legPrefixes->group1 = REPNZ; // 0xF2
			break;
		}
	}

	return 1;
}

unsigned char handleEVEXPrefix(unsigned char** bytesPtr, unsigned char* maxBytesAddr, struct LegacyPrefixes* legPrefixes, struct EVEXPrefix* result)
{
	if ((*bytesPtr) > maxBytesAddr) { return 0; }

	unsigned char firstByte = (*bytesPtr)[0];
	unsigned char p0 = (*bytesPtr)[1];
	unsigned char p1 = (*bytesPtr)[2];
	unsigned char p2 = (*bytesPtr)[3];

	if (firstByte == 0x62)
	{
		result->isValidEVEX = 1;

		result->rxb = (((p0 >> 7) & 0x01) * 4) + (((p0 >> 6) & 0x01) * 2) + ((p0 >> 5) & 0x01);
		result->r = ((p0 >> 4) & 0x01);
		result->mmm = (((p0 >> 2) & 0x01) * 4) + (((p0 >> 1) & 0x01) * 2) + ((p0 >> 0) & 0x01);

		result->w = ((p1 >> 7) & 0x01);
		result->vvvv = (((p1 >> 6) & 0x01) * 8) + (((p1 >> 5) & 0x01) * 4) + (((p1 >> 4) & 0x01) * 2) + ((p1 >> 3) & 0x01);
		result->pp = (((p1 >> 1) & 0x01) * 2) + ((p1 >> 0) & 0x01);

		result->z = ((p2 >> 7) & 0x01);
		result->ll = (((p2 >> 6) & 0x01) * 2) + ((p2 >> 5) & 0x01);
		result->b = ((p2 >> 4) & 0x01);
		result->v = ((p2 >> 3) & 0x01);
		result->aaa = (((p2 >> 2) & 0x01) * 4) + (((p2 >> 1) & 0x01) * 2) + ((p2 >> 0) & 0x01);

		switch (result->pp)
		{
		case 0b01:
			legPrefixes->group3 = OSO; // 0x66
			break;
		case 0b10:
			legPrefixes->group1 = REPZ; // 0xF3
			break;
		case 0b11:
			legPrefixes->group1 = REPNZ; // 0xF2
			break;
		}

		(*bytesPtr) += 4;
	}

	return 1;
}
