#pragma once
#include "jdcTextCtrl.h"
#include "colorsMenu.h"
#include "../file-handler/fileHandler.h"

#define NUM_OF_DATA_TEXT_CTRL_TYPES 7

class DataTextCtrl : public JdcTextCtrl
{
public:
	DataTextCtrl(wxWindow* parent, const wxSize& size, ColorsMenu* colorMenu, wxStaticText* statusText);

	ColorsMenu* colorsMenu = nullptr;

	const unsigned int bytesPerLine = 8;
	int numOfLines = 0;

	unsigned char* fileBytes = nullptr;
	unsigned long long numOfFileBytes = 0;
	unsigned long long imageBase = 0;
	struct FileSection* sections = nullptr;
	int numOfSections = 0;

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

	void Initialize(unsigned long long baseOfImage, struct FileSection* fileSections, int amountOfSections, unsigned char* bytes, unsigned long long numOfBytes);

	void ResetTextCtrl();

	void ClearData();

	void OnUpdateDataUI(wxStyledTextEvent& e);

	void UpdateTextCtrl();

	void ApplyDataHighlighting();

	void HighlightInstruction(unsigned long long address, int numOfBytes);
};
