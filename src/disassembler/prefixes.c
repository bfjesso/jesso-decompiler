#include "prefixes.h"

unsigned char handleLegacyPrefixes(struct DisassemblyParameters* params)
{
	params->legPrefixes.group1 = NO_PREFIX;
	params->legPrefixes.group2 = NO_PREFIX;
	params->legPrefixes.group3 = NO_PREFIX;
	params->legPrefixes.group4 = NO_PREFIX;

	while (1)
	{
		if (params->bytes > params->maxBytesAddr) { return 0; }

		switch (params->bytes[0])
		{
		case 0xF0:
			params->legPrefixes.group1 = LOCK;
			break;
		case 0xF2:
			params->legPrefixes.group1 = REPNZ;
			break;
		case 0xF3:
			params->legPrefixes.group1 = REPZ;
			break;
		case 0x2E:
			params->legPrefixes.group2 = CSO_BNT;
			break;
		case 0x36:
			params->legPrefixes.group2 = SSO;
			break;
		case 0x3E:
			params->legPrefixes.group2 = DSO_BT;
			break;
		case 0x26:
			params->legPrefixes.group2 = ESO;
			break;
		case 0x64:
			params->legPrefixes.group2 = FSO;
			break;
		case 0x65:
			params->legPrefixes.group2 = GSO;
			break;
		case 0x66:
			params->legPrefixes.group3 = OSO;
			break;
		case 0x67:
			params->legPrefixes.group4 = ASO;
			break;
		default:
			return 1;
		}

		params->bytes++;
	}

	return 1;
}

unsigned char handleREXPrefix(struct DisassemblyParameters* params)
{
	if (params->bytes > params->maxBytesAddr) { return 0; }

	unsigned char rexByte = params->bytes[0];

	if (rexByte < 0x40 || rexByte > 0x4F) { return 1; }

	params->rexPrefix.isValidREX = 1;
	params->rexPrefix.W = (rexByte >> 3) & 0x01;
	params->rexPrefix.R = (rexByte >> 2) & 0x01;
	params->rexPrefix.X = (rexByte >> 1) & 0x01;
	params->rexPrefix.B = (rexByte >> 0) & 0x01;

	params->bytes++;

	return 1;
}

unsigned char handleVEXPrefix(struct DisassemblyParameters* params)
{
	if (params->bytes > params->maxBytesAddr) { return 0; }

	unsigned char byte0 = params->bytes[0];
	unsigned char byte1 = params->bytes[1];
	unsigned char byte2 = params->bytes[2];

	if (byte0 == 0xC5) // two-byte form
	{
		params->vexPrefix.isValidVEX = 1;
		params->vexPrefix.R = (byte1 >> 7) & 0x01;
		params->vexPrefix.vvvv = (((byte1 >> 6) & 0x01) * 8) + (((byte1 >> 5) & 0x01) * 4) + (((byte1 >> 4) & 0x01) * 2) + ((byte1 >> 3) & 0x01);
		params->vexPrefix.L = (byte1 >> 2) & 0x01;
		params->vexPrefix.pp = (((byte1 >> 1) & 0x01) * 2) + ((byte1 >> 0) & 0x01);

		params->vexPrefix.m_mmmm = 0b00001;

		params->bytes += 2;
	}
	else if (byte0 == 0xC4) // three-byte form
	{
		params->vexPrefix.isValidVEX = 1;
		params->vexPrefix.R = (byte1 >> 7) & 0x01;
		params->vexPrefix.X = (byte1 >> 6) & 0x01;
		params->vexPrefix.B = (byte1 >> 5) & 0x01;
		params->vexPrefix.m_mmmm = (((byte1 >> 4) & 0x01) * 16) + (((byte1 >> 3) & 0x01) * 8) + (((byte1 >> 2) & 0x01) * 4) + (((byte1 >> 1) & 0x01) * 2) + ((byte1 >> 0) & 0x01);

		params->vexPrefix.W = (byte2 >> 7) & 0x01;
		params->vexPrefix.vvvv = (((byte2 >> 6) & 0x01) * 8) + (((byte2 >> 5) & 0x01) * 4) + (((byte2 >> 4) & 0x01) * 2) + ((byte2 >> 3) & 0x01);
		params->vexPrefix.L = (byte2 >> 2) & 0x01;
		params->vexPrefix.pp = (((byte2 >> 1) & 0x01) * 2) + ((byte2 >> 0) & 0x01);

		params->bytes += 3;
	}

	if(params->vexPrefix.isValidVEX)
	{
		switch(params->vexPrefix.pp)
		{
		case 0b01:
			params->legPrefixes.group3 = OSO; // 0x66
			break;
		case 0b10:
			params->legPrefixes.group1 = REPZ; // 0xF3
			break;
		case 0b11:
			params->legPrefixes.group1 = REPNZ; // 0xF2
			break;
		}
	}

	return 1;
}

unsigned char handleEVEXPrefix(struct DisassemblyParameters* params)
{
	if (params->bytes > params->maxBytesAddr) { return 0; }

	unsigned char firstByte = params->bytes[0];
	unsigned char p0 = params->bytes[1];
	unsigned char p1 = params->bytes[2];
	unsigned char p2 = params->bytes[3];

	if (firstByte == 0x62)
	{
		params->evexPrefix.isValidEVEX = 1;

		params->evexPrefix.R = ((p0 >> 7) & 0x01);
		params->evexPrefix.X = ((p0 >> 6) & 0x01);
		params->evexPrefix.B = ((p0 >> 5) & 0x01);
		params->evexPrefix.R_prime = ((p0 >> 4) & 0x01);
		params->evexPrefix.mmm = (((p0 >> 2) & 0x01) * 4) + (((p0 >> 1) & 0x01) * 2) + ((p0 >> 0) & 0x01);

		params->evexPrefix.W = ((p1 >> 7) & 0x01);
		params->evexPrefix.vvvv = (((p1 >> 6) & 0x01) * 8) + (((p1 >> 5) & 0x01) * 4) + (((p1 >> 4) & 0x01) * 2) + ((p1 >> 3) & 0x01);
		params->evexPrefix.pp = (((p1 >> 1) & 0x01) * 2) + ((p1 >> 0) & 0x01);

		params->evexPrefix.z = ((p2 >> 7) & 0x01);
		params->evexPrefix.LL = (((p2 >> 6) & 0x01) * 2) + ((p2 >> 5) & 0x01);
		params->evexPrefix.b = ((p2 >> 4) & 0x01);
		params->evexPrefix.V_prime = ((p2 >> 3) & 0x01);
		params->evexPrefix.aaa = (((p2 >> 2) & 0x01) * 4) + (((p2 >> 1) & 0x01) * 2) + ((p2 >> 0) & 0x01);

		switch (params->evexPrefix.pp)
		{
		case 0b01:
			params->legPrefixes.group3 = OSO; // 0x66
			break;
		case 0b10:
			params->legPrefixes.group1 = REPZ; // 0xF3
			break;
		case 0b11:
			params->legPrefixes.group1 = REPNZ; // 0xF2
			break;
		}

		params->bytes += 4;
	}

	return 1;
}

unsigned char checkFlagB(struct DisassemblyParameters* params)
{
	return params->rexPrefix.B || (params->vexPrefix.isValidVEX && !params->vexPrefix.B) || (params->evexPrefix.isValidEVEX && !params->evexPrefix.B);
}