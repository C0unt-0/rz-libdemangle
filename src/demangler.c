// SPDX-FileCopyrightText: 2015 inisider <inisider@gmail.com>
// SPDX-License-Identifier: LGPL-3.0-only

#include "demangler.h"
#include <stdlib.h>

#include "microsoft_demangle.h"

typedef enum EManglingType {
	eManglingMicrosoft = 0,
	eManglingUnsupported,
	eManglingUnknown,
	eManglingTypeMax,
	eManglingTypeCOFF,
} EManglingType;

///////////////////////////////////////////////////////////////////////////////
static EManglingType get_mangling_type(char *sym) {
	EManglingType mangling_type = eManglingUnsupported;
	if (sym == 0) {
		mangling_type = eManglingUnknown;
		goto get_mangling_type_err;
	}

	switch (*sym) {
	case '.':
	case '?':
		mangling_type = eManglingMicrosoft;
		break;
	case '@':
		mangling_type = eManglingTypeCOFF;
		break;
	default:
		break;
	}

get_mangling_type_err:
	return mangling_type;
}

///////////////////////////////////////////////////////////////////////////////
EDemanglerErr create_demangler(SDemangler **demangler) {
	EDemanglerErr err = eDemanglerErrOK;

	*demangler = (SDemangler *)malloc(sizeof(SDemangler));

	if (!*demangler) {
		err = eDemanglerErrMemoryAllocation;
		goto create_demagler_err;
	}

	(*demangler)->demangle = 0;
	(*demangler)->symbol = 0;

create_demagler_err:
	return err;
}

///////////////////////////////////////////////////////////////////////////////
EDemanglerErr init_demangler(SDemangler *demangler, char *sym) {
	EManglingType mangling_type = eManglingUnsupported;
	EDemanglerErr err = eDemanglerErrOK;

	// !!! sequence in this array need to be same as in EManglingType enum !!!
	demangle_func demangle_funcs[] = {
		microsoft_demangle, // Microsoft demangling function
		0, // Unsupported demangling
		0, // Unknown demangling
		0,
		coff_demangle, // COFF demangling function
	};

	if (demangler == 0) {
		err = eDemanglerErrMemoryAllocation;
		goto init_demangler_err;
	}

	mangling_type = get_mangling_type(sym);
	printf("mangling_type: %d\n", mangling_type);
	switch (mangling_type) {
	case eManglingUnsupported:
		err = eDemanglerErrUnsupportedMangling;
		break;
	case eManglingUnknown:
		err = eDemanglerErrUnkown;
		break;
	default:
		break;
	}

	if (err != eDemanglerErrOK) {
		goto init_demangler_err;
	}

	demangler->symbol = strdup(sym);
	demangler->demangle = demangle_funcs[mangling_type];

init_demangler_err:
	return err;
}

///////////////////////////////////////////////////////////////////////////////
void free_demangler(SDemangler *demangler) {
	RZ_FREE(demangler->symbol);
	RZ_FREE(demangler);
}
