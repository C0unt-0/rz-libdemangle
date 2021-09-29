// SPDX-FileCopyrightText: 2012-2019 pancake <pancake@nopcode.org>
// SPDX-License-Identifier: LGPL-3.0-only
#include <rz_libdemangle.h>
#include "demangler_util.h"

char *libdemangle_handler_objc(const char *sym) {
	char *ret = NULL;
	char *clas = NULL;
	char *name = NULL;
	char *args = NULL;
	int i, nargs = 0;
	const char *type = NULL;

	/* classes */
	if (!strncmp(sym, "_OBJC_Class_", 12)) {
		const char *className = sym + 12;
		ret = dem_str_newf("class %s", className);
		return ret;
	}
	if (!strncmp(sym, "_OBJC_CLASS_$_", 14)) {
		const char *className = sym + 14;
		ret = dem_str_newf("class %s", className);
		return ret;
	}
	if (!strncmp(sym, "_OBJC_METACLASS_$_", 18)) {
		const char *className = sym + 18;
		ret = dem_str_newf("class %s", className);
		return ret;
	}
	/* fields */
	if (!strncmp(sym, "_OBJC_IVAR_$_", 13)) {
		clas = strdup(sym + 13);
		char *p = strchr(clas, '.');
		type = "field";
		if (p) {
			*p = 0;
			name = strdup(p + 1);
		} else {
			name = NULL;
		}
	}
	/* methods */
	if (sym && sym[0] && sym[1] == '[') { // apple style
		if (sym[0] == '+') {
			type = "static";
		} else if (sym[0] == '-') {
			type = "public";
		}
		if (type) {
			free(clas);
			clas = strdup(sym + 2);
			name = strchr(clas, ' ');
			if (name) {
				*name++ = 0;
				name = strdup(name);
				if (!name) {
					free(clas);
					return NULL;
				}
				for (i = 0; name[i]; i++) {
					if (name[i] == ']') {
						name[i] = 0;
					}
					if (name[i] == ':') {
						nargs++;
						name[i] = 0;
					}
				}
			}
		}
	}
	if (sym[0] == '_' && sym[1] && sym[2] == '_') { // gnu style
		free(clas);
		clas = strdup(sym + 3);
		args = strstr(clas, "__");
		if (!args) {
			free(clas);
			if (name != clas) {
				free(name);
			}
			return NULL;
		}
		*args = 0;
		free(name);
		name = strdup(args + 2);
		if (!name) {
			free(clas);
			return NULL;
		}
		args = NULL;
		for (i = 0; name[i]; i++) {
			if (name[i] == '_') {
				name[i] = 0;
				nargs++;
			}
		}
		if (sym[1] == 'i') {
			type = "public";
		} else if (sym[1] == 'c') {
			type = "static";
		}
	}
	if (type) {
		if (!strcmp(type, "field")) {
			ret = dem_str_newf("field int %s::%s", clas, name);
		} else {
			if (nargs) {
				const char *arg = "int";
				args = malloc(((strlen(arg) + 4) * nargs) + 1);
				args[0] = 0;
				for (i = 0; i < nargs; i++) {
					strcat(args, arg);
					if (i + 1 < nargs) {
						strcat(args, ", ");
					}
				}
			} else {
				args = strdup("");
			}
			if (type && name && *name) {
				ret = dem_str_newf("%s int %s::%s(%s)", type, clas, name, args);
			}
		}
	}
	free(clas);
	free(args);
	free(name);
	return ret;
}
