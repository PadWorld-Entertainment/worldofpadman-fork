/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Quake III Arena source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

/*****************************************************************************
 * name:		l_struct.c
 *
 * desc:		structure reading / writing
 *
 * $Archive: /MissionPack/CODE/botlib/l_struct.c $
 *
 *****************************************************************************/

#ifdef BOTLIB
#include "../qcommon/q_shared.h"
#include "botlib.h" //for the include of be_interface.h
#include "l_script.h"
#include "l_precomp.h"
#include "l_struct.h"
#include "l_utils.h"
#include "be_interface.h"
#endif // BOTLIB

#ifdef BSPC
// include files for usage in the BSP Converter
#include "../bspc/qbsp.h"
#include "../bspc/l_log.h"
#include "../bspc/l_mem.h"
#include "l_precomp.h"
#include "l_struct.h"

#define qtrue true
#define qfalse false
#endif // BSPC

static const fielddef_t *FindField(const fielddef_t *defs, const char *name) {
	int i;

	for (i = 0; defs[i].name; i++) {
		if (!strcmp(defs[i].name, name))
			return &defs[i];
	}
	return NULL;
}

static qboolean ReadNumber(source_t *source, const fielddef_t *fd, void *p) {
	token_t token;
	int negative = qfalse;
	long int intval, intmin = 0, intmax = 0;
	double floatval;

	if (!PC_ExpectAnyToken(source, &token))
		return 0;

	// check for minus sign
	if (token.type == TT_PUNCTUATION) {
		if (fd->type & FT_UNSIGNED) {
			SourceError(source, "expected unsigned value, found %s", token.string);
			return 0;
		}
		// if not a minus sign
		if (strcmp(token.string, "-") != 0) {
			SourceError(source, "unexpected punctuation %s", token.string);
			return 0;
		}
		negative = qtrue;
		// read the number
		if (!PC_ExpectAnyToken(source, &token))
			return 0;
	}
	// check if it is a number
	if (token.type != TT_NUMBER) {
		SourceError(source, "expected number, found %s", token.string);
		return 0;
	}
	// check for a float value
	if (token.subtype & TT_FLOAT) {
		if ((fd->type & FT_TYPE) != FT_FLOAT) {
			SourceError(source, "unexpected float");
			return 0;
		}
		floatval = token.floatvalue;
		if (negative)
			floatval = -floatval;
		if (fd->type & FT_BOUNDED) {
			if (floatval < fd->floatmin || floatval > fd->floatmax) {
				SourceError(source, "float out of range [%f, %f]", fd->floatmin, fd->floatmax);
				return 0;
			}
		}
		*(float *)p = (float)floatval;
		return 1;
	}

	intval = token.intvalue;
	if (negative)
		intval = -intval;
	// check bounds
	if ((fd->type & FT_TYPE) == FT_CHAR) {
		if (fd->type & FT_UNSIGNED) {
			intmin = 0;
			intmax = 255;
		} else {
			intmin = -128;
			intmax = 127;
		}
	}
	if ((fd->type & FT_TYPE) == FT_INT) {
		if (fd->type & FT_UNSIGNED) {
			intmin = 0;
			intmax = 65535;
		} else {
			intmin = -32768;
			intmax = 32767;
		}
	}
	if ((fd->type & FT_TYPE) == FT_CHAR || (fd->type & FT_TYPE) == FT_INT) {
		if (fd->type & FT_BOUNDED) {
			intmin = Maximum(intmin, fd->floatmin);
			intmax = Minimum(intmax, fd->floatmax);
		}
		if (intval < intmin || intval > intmax) {
			SourceError(source, "value %ld out of range [%ld, %ld]", intval, intmin, intmax);
			return 0;
		}
	} else if ((fd->type & FT_TYPE) == FT_FLOAT) {
		if (fd->type & FT_BOUNDED) {
			if (intval < fd->floatmin || intval > fd->floatmax) {
				SourceError(source, "value %ld out of range [%f, %f]", intval, fd->floatmin, fd->floatmax);
				return 0;
			}
		}
	}
	// store the value
	if ((fd->type & FT_TYPE) == FT_CHAR) {
		if (fd->type & FT_UNSIGNED)
			*(unsigned char *)p = (unsigned char)intval;
		else
			*(char *)p = (char)intval;
	} else if ((fd->type & FT_TYPE) == FT_INT) {
		if (fd->type & FT_UNSIGNED)
			*(unsigned int *)p = (unsigned int)intval;
		else
			*(int *)p = (int)intval;
	} else if ((fd->type & FT_TYPE) == FT_FLOAT) {
		*(float *)p = (float)intval;
	}
	return 1;
}

static qboolean ReadChar(source_t *source, const fielddef_t *fd, void *p) {
	token_t token;

	if (!PC_ExpectAnyToken(source, &token))
		return 0;

	// take literals into account
	if (token.type == TT_LITERAL) {
		StripSingleQuotes(token.string);
		*(char *)p = token.string[0];
	} else {
		PC_UnreadLastToken(source);
		if (!ReadNumber(source, fd, p))
			return 0;
	}
	return 1;
}

static int ReadString(source_t *source, const fielddef_t *fd, void *p) {
	token_t token;

	if (!PC_ExpectTokenType(source, TT_STRING, 0, &token))
		return 0;
	// remove the double quotes
	StripDoubleQuotes(token.string);
	// copy the string and make sure it is closed with a zero
	Q_strncpyz((char *)p, token.string, MAX_STRINGFIELD);
	return 1;
}

int ReadStructure(source_t *source, const structdef_t *def, char *structure) {
	token_t token;
	const fielddef_t *fd;
	void *p;
	int num;

	if (!PC_ExpectTokenString(source, "{"))
		return 0;
	while (1) {
		if (!PC_ExpectAnyToken(source, &token))
			return qfalse;
		// if end of structure
		if (!strcmp(token.string, "}"))
			break;
		// find the field with the name
		fd = FindField(def->fields, token.string);
		if (!fd) {
			SourceError(source, "unknown structure field %s", token.string);
			return qfalse;
		}
		if (fd->type & FT_ARRAY) {
			num = fd->maxarray;
			if (!PC_ExpectTokenString(source, "{"))
				return qfalse;
		} else {
			num = 1;
		}
		p = (void *)(structure + fd->offset);
		while (num-- > 0) {
			if (fd->type & FT_ARRAY) {
				if (PC_CheckTokenString(source, "}"))
					break;
			}
			switch (fd->type & FT_TYPE) {
			case FT_CHAR: {
				if (!ReadChar(source, fd, p))
					return qfalse;
				p = (char *)p + sizeof(char);
				break;
			}
			case FT_INT: {
				if (!ReadNumber(source, fd, p))
					return qfalse;
				p = (char *)p + sizeof(int);
				break;
			}
			case FT_FLOAT: {
				if (!ReadNumber(source, fd, p))
					return qfalse;
				p = (char *)p + sizeof(float);
				break;
			}
			case FT_STRING: {
				if (!ReadString(source, fd, p))
					return qfalse;
				p = (char *)p + MAX_STRINGFIELD;
				break;
			}
			case FT_STRUCT: {
				if (!fd->substruct) {
					SourceError(source, "BUG: no sub structure defined");
					return qfalse;
				}
				ReadStructure(source, fd->substruct, (char *)p);
				p = (char *)p + fd->substruct->size;
				break;
			}
			}
			if (fd->type & FT_ARRAY) {
				if (!PC_ExpectAnyToken(source, &token))
					return qfalse;
				if (!strcmp(token.string, "}"))
					break;
				if (strcmp(token.string, ",") != 0) {
					SourceError(source, "expected a comma, found %s", token.string);
					return qfalse;
				}
			}
		}
	}
	return qtrue;
}

int WriteIndent(FILE *fp, int indent) {
	while (indent-- > 0) {
		if (fprintf(fp, "\t") < 0)
			return qfalse;
	}
	return qtrue;
}

int WriteFloat(FILE *fp, float value) {
	char buf[128];
	int l;

	Com_sprintf(buf, sizeof(buf), "%f", value);
	l = strlen(buf);
	// strip any trailing zeros
	while (l-- > 1) {
		if (buf[l] != '0' && buf[l] != '.')
			break;
		if (buf[l] == '.') {
			buf[l] = 0;
			break;
		}
		buf[l] = 0;
	}
	// write the float to file
	if (fprintf(fp, "%s", buf) < 0)
		return 0;
	return 1;
}

static int WriteStructWithIndent(FILE *fp, const structdef_t *def, const char *structure, int indent) {
	int i, num;
	void *p;
	const fielddef_t *fd;

	if (!WriteIndent(fp, indent))
		return qfalse;
	if (fprintf(fp, "{\r\n") < 0)
		return qfalse;

	indent++;
	for (i = 0; def->fields[i].name; i++) {
		fd = &def->fields[i];
		if (!WriteIndent(fp, indent))
			return qfalse;
		if (fprintf(fp, "%s\t", fd->name) < 0)
			return qfalse;
		p = (void *)(structure + fd->offset);
		if (fd->type & FT_ARRAY) {
			num = fd->maxarray;
			if (fprintf(fp, "{") < 0)
				return qfalse;
		} else {
			num = 1;
		}
		while (num-- > 0) {
			switch (fd->type & FT_TYPE) {
			case FT_CHAR: {
				if (fprintf(fp, "%d", *(char *)p) < 0)
					return qfalse;
				p = (char *)p + sizeof(char);
				break;
			}
			case FT_INT: {
				if (fprintf(fp, "%d", *(int *)p) < 0)
					return qfalse;
				p = (char *)p + sizeof(int);
				break;
			}
			case FT_FLOAT: {
				if (!WriteFloat(fp, *(float *)p))
					return qfalse;
				p = (char *)p + sizeof(float);
				break;
			}
			case FT_STRING: {
				if (fprintf(fp, "\"%s\"", (char *)p) < 0)
					return qfalse;
				p = (char *)p + MAX_STRINGFIELD;
				break;
			}
			case FT_STRUCT: {
				if (!WriteStructWithIndent(fp, fd->substruct, structure, indent))
					return qfalse;
				p = (char *)p + fd->substruct->size;
				break;
			}
			}
			if (fd->type & FT_ARRAY) {
				if (num > 0) {
					if (fprintf(fp, ",") < 0)
						return qfalse;
				} else {
					if (fprintf(fp, "}") < 0)
						return qfalse;
				}
			}
		}
		if (fprintf(fp, "\r\n") < 0)
			return qfalse;
	}
	indent--;

	if (!WriteIndent(fp, indent))
		return qfalse;
	if (fprintf(fp, "}\r\n") < 0)
		return qfalse;
	return qtrue;
}

int WriteStructure(FILE *fp, const structdef_t *def, const char *structure) {
	return WriteStructWithIndent(fp, def, structure, 0);
}
