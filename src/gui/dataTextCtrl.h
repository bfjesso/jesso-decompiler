#pragma once
#include "jdcTextCtrl.h"

#define NUM_OF_DATA_TEXT_CTRL_TYPES 7

class DataTextCtrl : public JdcTextCtrl
{
public:
	DataTextCtrl(wxWindow* parent, MainGui* mainGuiRef, wxString name);

	const uint32_t bytesPerLine = 8;
	int32_t numOfLines = 0;

	const char* dataTypeStrs[NUM_OF_DATA_TEXT_CTRL_TYPES] =
	{
		"1-byte int32_t",
		"2-byte int32_t",
		"4-byte int32_t",
		"8-byte int32_t",
		"float",
		"double",
		"ASCII character"
	};
	const int32_t typeSizes[NUM_OF_DATA_TEXT_CTRL_TYPES] =
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
	bool isHex = true;
	bool isSigned = false;

	void Initialize();

	void ResetTextCtrl();

	void ShowGoToVirtualAddressDialog();

	void ShowGoToFileOffsetDialog();

	void DataRightClickOptions(wxContextMenuEvent& e);

	void OnDataKeyDown(wxKeyEvent& e);

	void OnUpdateDataUI(wxStyledTextEvent& e);

	void UpdateTextCtrl();

	void ApplyDataHighlighting();

	void HighlightBytes(uint64_t address, uint32_t numOfBytes, enum IndicatorColor color);
};
