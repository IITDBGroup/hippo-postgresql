/*-------------------------------------------------------------------------
 *
 * binary_search.h
 *	  PostgreSQL functions for searching in an array using binary search.
 *
 * Author:	Boris Glavic
 * Portions Copyright (c) 1996-2016, PostgreSQL Global Development Group
 *
 * IDENTIFICATION
 *	  contrib/binary_search/binary_search.h
 *
 *-------------------------------------------------------------------------
 */

#ifndef BINARY_SEARCH_H
#define BINARY_SEARCH_H

#include "fmgr.h"

extern Datum binary_search_array_pos(PG_FUNCTION_ARGS);
extern Datum binary_search_array_contains(PG_FUNCTION_ARGS);
extern Datum binary_search_in_bucket(PG_FUNCTION_ARGS);
extern Datum set_bit_on(PG_FUNCTION_ARGS);
extern Datum fast_no_copy_bit_or(PG_FUNCTION_ARGS);

#endif   /* ISN_H */
