#ifndef FILEREADER_H
#define FILEREADER_H

#include <Windows.h>
#include <tchar.h>
#include <iostream>

#import "CFGReader.tlb" raw_interfaces_only

namespace CFGReader {

	BSTR get_BSTR(std::wstring w_string) {
		return SysAllocStringLen(w_string.data(), w_string.size());
	}

	IReaderPtr CFG_initialize() {
		HRESULT hr = CoInitialize(NULL);
		IReaderPtr reader_p = NULL;
		try {
			reader_p = IReaderPtr(__uuidof(IReader));
		}
		catch (const _com_error& e) {
			std::cerr << "CFG reader initialisation error: " << e.ErrorMessage() << std::endl;
		}
		return reader_p;
	}

	void exit_reader() {
		CoUninitialize();
	}

	VARIANT get_value(IDataStructure* ds, std::wstring value_name, unsigned char type, VARIANT* def = NULL, bool test = false, bool quiet = false) {
		VARIANT default_;
		if (def == NULL) {
			default_.intVal = 0;
		} else {
			default_ = *def;
		}
		VARIANT* value = NULL;
		ds->Get(get_BSTR(value_name), type, default_, test, quiet, value);
		return *value;
	}

}

// Define type identifyers
#define   CFG_INT_32 0x00
#define CFG_U_INT_16 0x01
#define CFG_U_INT_64 0x02
#define CFG_FLOAT_32 0x03
#define CFG_FLOAT_64 0x04
#define   CFG_VECTOR 0x05
#define   CFG_STRING 0x06
#define  CFG_BOOLEAN 0x07

#define   CFG_INT_32_ARR 0x09
#define CFG_U_INT_16_ARR 0x0a
#define CFG_U_INT_64_ARR 0x0b
#define CFG_FLOAT_32_ARR 0x0c
#define CFG_FLOAT_64_ARR 0x0d
#define   CFG_VECTOR_ARR 0x0e
#define   CFG_STRING_ARR 0x0f
//#define  CFG_BOOLEAN_ARR 0x10

// Define flags
#define CFG_PATH 0x10
#define CFG_LOGFILE 0x11


#endif // !FILEREADER_H
