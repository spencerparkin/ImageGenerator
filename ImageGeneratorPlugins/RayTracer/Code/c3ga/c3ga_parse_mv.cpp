/*
Gaigen 2.5
*/
/*

The code in this file was generated by Gaigen 2.5 which was written by Daniel Fontijne.
He grants that code generated by Gaigen 2.5 be provided under any license, such as the one to follow.

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall
be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

*/

#include <stdio.h>
#include <ctype.h>
#include "c3ga.h"

#ifdef WIN32
#define _snprintf __snprintf
#pragma warning(disable:4996) /* quit your whining already */
#endif /* WIN32 */

namespace c3ga {

static void c3gaParseWedgeBasisVector(unsigned int bvIdx, double *scale, unsigned int *bitmap) {
	unsigned b;
	
	b = 1 << bvIdx;
	if ((*bitmap) & b) { /* bv ^ bv = 0 */
		*scale = 0.0;
		return;
	}

	/* add basis vector to bitmap */
	(*bitmap) |= b;

	bvIdx++;
	for (; bvIdx < 5; bvIdx++) /* compute sign flips due to anti commuting basis vectors */
		if ((*bitmap) & (1 << bvIdx)) (*scale) = -(*scale);
}

static void c3gaParseSum(double coord[32], double scale, unsigned int bitmap) {
	int idx = c3ga_basisElementIndexByBitmap[bitmap];
	coord[idx] += scale / (double)c3ga_basisElementSignByIndex[idx];
}

static void c3gaParseDataInit(struct c3gaParseMultivectorData *PD) {
	PD->message[0] = 0;
	PD->value.set();
}

/** returns true when 'buf' starts with 'pattern' */
static int c3gaCompareId(const char *pattern, const char *buf, int len) {
	int i = 0;
	while (pattern[i] != 0) {
		if (buf[i] != pattern[i]) return 0;
		else i++;
	}
	if (i != len) return 0; /* must match full pattern */
	else return 1;
}


/** TOKEN IDs: (for internal use only) */
#define T_BAD_IDENTIFIER -100
#define T_BAD_NUMBER -10
#define T_BAD_CHARACTER -1
#define T_END_OF_STRING 0
#define T_WEDGE 1
#define T_MUL 2
#define T_PLUS 3
#define T_MINUS 4
#define T_NUMBER 10
#define T_FIRST_BASIS_VECTOR 100
#define T_LAST_BASIS_VECTOR 104
#define T_NO 100
#define T_E1 101
#define T_E2 102
#define T_E3 103
#define T_NI 104

/**
Returns the next token for the multivector parser.
Skips whitespace.

Sets startIdx and endIdx to start and end (inclusive) of token, except when end of string.

If the next token is a number, anything that atof() can accept should be accepted.

Increments lineIdx when new line char (0x0A) is found, and sets currentLineStart to
the index of that char.
*/
int c3gaParseGetNextToken(const char *buf, int *startIdx, int *endIdx, int *lineIdx, int *currentLineStart) {
	int i, pointFound = 0;

	/* skip all whitespace and other empty stuff, keep track of line index */
	while ((buf[(*startIdx)] > 0) && (buf[(*startIdx)] <= ' ')) {
		if (buf[(*startIdx)] == 0x0A) { /* start of new line */
			(*lineIdx)++;
			*currentLineStart = (*startIdx)+1;
		}
		(*startIdx)++;
	}

	/* detect end of string */
	if (buf[(*startIdx)] == 0) {
		*startIdx = *endIdx;
		return T_END_OF_STRING; /* EOS */
	}

	/* operators */
	if (buf[(*startIdx)] == '^') {*endIdx = *startIdx; return T_WEDGE;} /*   ^   */
	else if (buf[(*startIdx)] == '*') {*endIdx = *startIdx; return T_MUL;} /*   *   */
	else if (buf[(*startIdx)] == '+') {*endIdx = *startIdx; return T_PLUS;} /*   +   */
	else if (buf[(*startIdx)] == '-') {*endIdx = *startIdx; return T_MINUS;} /*   -   */

	else if (isdigit((unsigned char)buf[(*startIdx)]) || (buf[(*startIdx)] == '.')) { /* parse number? */
		*endIdx = (*startIdx);

		/* eat up all digits and at most one point */
		while (isdigit((unsigned char)buf[(*endIdx)]) || (buf[(*endIdx)] == '.')) {
			(*endIdx)++;
			if (buf[(*endIdx)] == '.') {
				(*endIdx)++;
				pointFound = 1;
				break;
			}
		}
		
		if (pointFound) { /* if point found, eat up all digits */
			while (isdigit((unsigned char)buf[(*endIdx)])) {
				(*endIdx)++;
			}
		}

		/* see if there is a 'e' or 'E' */
		if  ((buf[(*endIdx)] == 'e') || (buf[(*endIdx)] == 'E')) {
			(*endIdx)++;
			/* accept at most one +- */
			if  ((buf[(*endIdx)] == '-') || (buf[(*endIdx)] == '+')) {
				(*endIdx)++;
			}

			/* if there is an 'e', there must be some digit */
			if (!isdigit((unsigned char)buf[(*endIdx)])) return T_BAD_NUMBER; /* bad number */

			/* eat up all digits */
			while (isdigit((unsigned char)buf[(*endIdx)])) {
				(*endIdx)++;
			}
		}
		(*endIdx)--; /* end index is inclusive */
		return T_NUMBER;
	}

	else if (isalpha((unsigned char)buf[(*startIdx)]) || (buf[(*startIdx)] == '_')) { /* parse identifier? */
		/* find end of chain of numbers, letters and '_' */
		*endIdx = (*startIdx) + 1;

		while (isalnum((unsigned char)buf[(*endIdx)]) || (buf[(*endIdx)] == '_')) (*endIdx)++;
		(*endIdx)--;  /* end index is inclusive */

		/* see which basis vector it is */
		for (i = 0; i < 5; i++)
			if (c3gaCompareId(c3ga_basisVectorNames[i], buf + (*startIdx), (*endIdx) - (*startIdx) + 1)) return T_FIRST_BASIS_VECTOR + i; /* basis vector */
		return T_BAD_IDENTIFIER; /* bad identifier */
	}

	else return T_BAD_CHARACTER;
} /* end of c3gaParseGetNextToken */

bool parseEx(struct c3gaParseMultivectorData *PD, const std::string &_str, const std::string &_strSourceName)
{
	double coord[32];
	char buf[256]; /* used to copy number tokens */
	double scale;
	unsigned int bitmap;
	int startIdx = 0, endIdx;
	int lineIdx = 0, currentLineStart = 0;
	int token, i, beDone, cnt, firstLoop = 1;
	const char *str = _str.c_str();
	const char *strSourceName = _strSourceName.c_str();

	c3gaParseDataInit(PD); /* should reset all */
	c3ga::zero_N(coord, 32);

	/* get the first token */
	token = c3gaParseGetNextToken(str, &startIdx, &endIdx, &lineIdx, &currentLineStart);

	while (1) {
		/* reset for next basis blade */
		bitmap = 0;
		scale = 1.0;
		beDone = 0;

		if (token == T_END_OF_STRING) break;


		cnt = 0;
		while ((token == T_PLUS) || (token == T_MINUS)) { /* accept all +- */
			cnt++;
			startIdx = endIdx+1;
			if (token == T_MINUS) scale *= -1.0; /*   -   */
			token = c3gaParseGetNextToken(str, &startIdx, &endIdx, &lineIdx, &currentLineStart);
		}
		
		/* require at least one +- if this is not the first term: */
		if ((!firstLoop) && (cnt == 0)) {
			_snprintf(PD->message, 256, "Expected '+' or '-' at %s, line %d, column %d", strSourceName, lineIdx+1, startIdx - currentLineStart +1);
			return false;
		}

		if ((token == T_NUMBER) || ((token >= T_FIRST_BASIS_VECTOR) && (token <= T_LAST_BASIS_VECTOR))) { /* must be number or basis vector */
			if (token == T_NUMBER) {
				{ /* copy token to buf, multiply scale with value of number */
					for (i = 0; i <= (endIdx-startIdx); i++)
						buf[i] = str[startIdx+i];
					buf[(endIdx-startIdx)+1] = 0;
					scale *= (double)atof((const char*)buf);
				}
				startIdx = endIdx+1;

				/*   * or ^ ?   */
				token = c3gaParseGetNextToken(str, &startIdx, &endIdx, &lineIdx, &currentLineStart);
				if ((token == T_WEDGE) || (token == T_MUL)) {
					startIdx = endIdx+1;

					/* must find basis vector */
					token = c3gaParseGetNextToken(str, &startIdx, &endIdx, &lineIdx, &currentLineStart);
				}
				else { /* just a single scalar is OK */
					startIdx = endIdx+1;
					beDone = 1;
				}

			}

			if (!beDone) {
				if ((token >= T_FIRST_BASIS_VECTOR) && (token <= T_LAST_BASIS_VECTOR)) {
					c3gaParseWedgeBasisVector((unsigned int)token - T_FIRST_BASIS_VECTOR, &scale, &bitmap);
					startIdx = endIdx+1;
				}
				else {
					_snprintf(PD->message, 256, "Expected basis vector at %s, line %d, column %d", strSourceName, lineIdx+1, startIdx - currentLineStart +1);
					return false;
				}
			}

			if (!beDone) {
				/* accept ^ basis vector as many times as it takes */
				while (1) {
					/* ^ */
					token = c3gaParseGetNextToken(str, &startIdx, &endIdx, &lineIdx, &currentLineStart);
					if (token != T_WEDGE) break;
					startIdx = endIdx+1;

					/* basis vector */
					token = c3gaParseGetNextToken(str, &startIdx, &endIdx, &lineIdx, &currentLineStart);
					if ((token >= T_FIRST_BASIS_VECTOR) && (token <= T_LAST_BASIS_VECTOR)) {
						c3gaParseWedgeBasisVector((unsigned int)token - T_FIRST_BASIS_VECTOR, &scale, &bitmap);
						startIdx = endIdx+1;
					}
					else {
						_snprintf(PD->message, 256, "Expected basis vector at %s, line %d, column %d", strSourceName, lineIdx+1, startIdx - currentLineStart +1);
						return false;
					}


				}

			}
		} /* end of 'if number or bv' */
		else if (token == T_BAD_CHARACTER) {
			_snprintf(PD->message, 256, "Bad character at %s, line %d, column %d", strSourceName, lineIdx+1, startIdx - currentLineStart +1);
			return false;
		}
		else if (token == T_BAD_NUMBER) {
			_snprintf(PD->message, 256, "Bad number at %s, line %d, column %d", strSourceName, lineIdx+1, startIdx - currentLineStart +1);
			return false;
		}
		else if (token == T_BAD_IDENTIFIER) {
			_snprintf(PD->message, 256, "Bad identifier at %s, line %d, column %d", strSourceName, lineIdx+1, startIdx - currentLineStart +1);
			return false;
		}
		else {
			_snprintf(PD->message, 256, "Unexpected token at %s, line %d, column %d", strSourceName, lineIdx+1, startIdx - currentLineStart +1);
			return false;
		}

		/* add */
		c3gaParseSum(coord, scale, bitmap);
		
		/* remember that the first loop is done */
		firstLoop = 0;
	}

	/* compress */
	PD->value = mv_compress(coord, 0.0, 63);

	return true; /* success */
} /* end of parse_mvEx */

mv parse(const std::string &str)
{
	struct c3gaParseMultivectorData PD;
	bool result = parseEx(&PD, str, std::string(""));
	if (result) return PD.value;
	throw std::string(PD.message);
}

} /* end of namespace c3ga */

