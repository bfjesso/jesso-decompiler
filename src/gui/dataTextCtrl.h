#pragma once
#include "jdcTextCtrl.h"
#include "colorsMenu.h"

#include "../decompiler/decompilationStructs.h"

#define NUM_OF_DATA_TEXT_CTRL_TYPES 7

class DataTextCtrl : public JdcTextCtrl
{
public:
	DataTextCtrl(wxWindow* parent, wxString name, struct DecompilationParameters* decompParams, ColorsMenu* colorMenu);

	ColorsMenu* colorsMenu = nullptr;

	const unsigned int bytesPerLine = 8;
	int numOfLines = 0;

	struct DecompilationParameters* params = nullptr;

	const char* dataTypeStrs[NUM_OF_DATA_TEXT_CTRL_TYPES] =
	{
		"1-byte int",
		"2-byte int",
		"4-byte int",
		"8-byte int",
		"float",
		"double",
		"ASCII character"
	};
	const int typeSizes[NUM_OF_DATA_TEXT_CTRL_TYPES] =
	{
		1,
		2,
		4,
		8,
		4,
		8,
		1,
	};
	enum DataTextCtrlTypes
	{
		ONE_BYTE_INT_TYPE,
		TWO_BYTE_INT_TYPE,
		FOUR_BYTE_INT_TYPE,
		EIGHT_BYTE_INT_TYPE,
		FLOAT_TYPE,
		DOUBLE_TYPE,
		ASCII_CHAR_TYPE
	};

	enum DataTextCtrlTypes selectedType = ONE_BYTE_INT_TYPE;
	unsigned char isHex = 1;

	void Initialize();

	void ResetTextCtrl();

	void ShowGoToAddressDialog();

	void DataRightClickOptions(wxContextMenuEvent& e);

	void OnDataKeyDown(wxKeyEvent& e);

	void OnUpdateDataUI(wxStyledTextEvent& e);

	void UpdateTextCtrl();

	void ApplyDataHighlighting();

	void HighlightBytes(unsigned long long address, int numOfBytes, enum IndicatorColor color);
};
