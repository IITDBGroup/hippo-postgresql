/*-------------------------------------------------------------------------
 *
 * binary_search.c
 *	  PostgreSQL type definitions for BINARY_SEARCHs (ISBN, ISMN, ISSN, EAN13, UPC)
 *
 * Author:	Boris Glavic
 * Portions Copyright (c) 1996-2016, PostgreSQL Global Development Group
 *
 * IDENTIFICATION
 *	  contrib/binary_search/binary_search.c
 *
 *-------------------------------------------------------------------------
 */

#include "postgres.h"

#include "fmgr.h"
#include "utils/builtins.h"
#include "utils/array.h"
#include "utils/varbit.h"
#include "utils/lsyscache.h"
#include "binary_search.h"


PG_MODULE_MAGIC;

#define MIN(A, B) ((B)<(A)?(B):(A))
#define MAX(A, B) ((B)>(A)?(B):(A))


//TODO use later
enum binary_search_type
{
	BINARY_SEARCH_EXACT,
	BINARY_SEARCH_BINS
};

static Datum binary_search_array_position(FunctionCallInfo fcinfo);

PG_FUNCTION_INFO_V1(binary_search_array_pos);
Datum
binary_search_array_pos(PG_FUNCTION_ARGS)
{
	return binary_search_array_position(fcinfo);
}

PG_FUNCTION_INFO_V1(binary_search_array_contains);
Datum
binary_search_array_contains(PG_FUNCTION_ARGS)
{
	int32 pos;
	Datum ret;
	bool result;

	ret = binary_search_array_position(fcinfo);
	pos = DatumGetInt16(ret);
	result = (pos >= 0);

	PG_RETURN_BOOL(result);
}


PG_FUNCTION_INFO_V1(binary_search_in_bucket);
Datum
binary_search_in_bucket(PG_FUNCTION_ARGS)
{
	bool isThere = FALSE;
	Datum ret;
	int32 pos;

	ret = binary_search_array_position(fcinfo);
	pos = DatumGetInt16(ret);
	isThere = (pos % 2);

	PG_RETURN_BOOL(isThere);
}

#define SET_BIT_MIN_RESULT_BITS 100
#define SET_BIT_MAX_GROWTH_STEP 1000

PG_FUNCTION_INFO_V1(set_bit_on);
Datum
set_bit_on(PG_FUNCTION_ARGS)
{
	VarBit *v = PG_GETARG_VARBIT_P(0);
	VarBit *result = v;
	int32 pos = PG_GETARG_INT64(1);
	int bitlen,
		len;
	bits8 *r,
		  *p;
	int	byteNo,
		bitNo;

	bitlen = VARBITLEN(v);
	// ignore smaller than 0 positions
	if (pos < 0)
		PG_RETURN_VARBIT_P(result);
	// grow varbit if input not of sufficient length
	if (pos >= bitlen)
	{
		// double in size until we go beyond 1000 bits. Also make sure the result is at least 100 bits to avoid repeated copying
		bitlen = MAX(SET_BIT_MIN_RESULT_BITS, MIN(2 * bitlen, pos + 1 + SET_BIT_MAX_GROWTH_STEP));
		len = VARBITTOTALLEN(bitlen);
		result = (VarBit *) palloc0(len);
		SET_VARSIZE(result, len);
		VARBITLEN(result) = bitlen;

		p = VARBITS(v);
		r = VARBITS(result);

		memcpy(r, p, VARBITBYTES(v));
	}
	else
		r = VARBITS(result);

	byteNo = pos / BITS_PER_BYTE;
	bitNo = BITS_PER_BYTE - 1 - (pos % BITS_PER_BYTE);

	r[byteNo] |= (1 << bitNo);

	PG_RETURN_VARBIT_P(result);
}



static Datum
binary_search_array_position(FunctionCallInfo fcinfo)
{
	ArrayType  *array;
	Oid			element_type;
		        /* array_type; */
	Datum		searched_element,
				value;
	int64       search_element_int,
		        value_int = -1;
	int			position,
		        low,
		        middle,
		        high;
	int16       array_typlen = -1;
//	TypeCacheEntry *typentry;
	ArrayMetaState *my_extra;
	int *subscript = palloc0(sizeof(int));
	bool isNull;

	/*
	 * do not attempt to search in NULL array or for a NULL element
	 */
	if (PG_ARGISNULL(0) || PG_ARGISNULL(1))
		PG_RETURN_NULL();

	array = PG_GETARG_ARRAYTYPE_P(0);
	element_type = ARR_ELEMTYPE(array);
	searched_element = PG_GETARG_DATUM(1);
	search_element_int = DatumGetInt64(searched_element);

	/*
	 * get element type information (currently ignored since we assume int16.
	 */
	my_extra = (ArrayMetaState *) MemoryContextAlloc(fcinfo->flinfo->fn_mcxt,
													  sizeof(ArrayMetaState));

	/* if (my_extra->element_type != element_type) */
	/* { */
//		get_typlen(array_type);
		get_typlenbyvalalign(element_type,
							 &my_extra->typlen,
							 &my_extra->typbyval,
							 &my_extra->typalign);
	/* } */

	/*
	 * We refuse to search for elements in multi-dimensional arrays, since we
	 * have no good way to report the element's location in the array.
	 */
	if (ARR_NDIM(array) > 1)
		ereport(ERROR,
				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
				 errmsg("searching for elements in multidimensional arrays is not supported")));

	position = (ARR_LBOUND(array))[0] - 1;
	low = position;
	high = (ARR_DIMS(array))[0] + position; // position of last element of the array

	// the binary search
	while(high > low + 1)
	{
		middle = low + ((high - low) / 2);
		*subscript = middle;
		value = array_get_element(PointerGetDatum(array),
								  1,
								  subscript,
								  array_typlen,
								  my_extra->typlen,
								  my_extra->typbyval,
								  my_extra->typalign,
								  &isNull);
		value_int = DatumGetInt64(value);

		if (search_element_int >= value_int)
			low = middle;
		else
			high = middle;
	}

	pfree(subscript);

	PG_RETURN_INT32(low);
}
