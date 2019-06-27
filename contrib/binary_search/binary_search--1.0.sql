/* contrib/binary_search/binary_search--1.0.sql */

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION binary_search" to load this file. \quit

-- Example:
--   SELECT binary_search_array('{1,15,27,50}', 27);
--   SELECT binary_search_array_pos('{1,15,27,50}', 23);
--

--
--	search functions
--
---------------------------------------------------
CREATE FUNCTION binary_search_array_pos(int8[], int8)
	RETURNS int4
	AS 'MODULE_PATHNAME'
	LANGUAGE C
	IMMUTABLE STRICT
	PARALLEL SAFE;

CREATE FUNCTION binary_search_in_bucket(int8[], int8)
	RETURNS boolean
	AS 'MODULE_PATHNAME'
	LANGUAGE C
	IMMUTABLE STRICT
	PARALLEL SAFE;

-- CREATE FUNCTION sql_bit_on(varbit, integer) RETURNS varbit
--     AS 'select set_bit($1,$2,1);'
--     LANGUAGE SQL
--     IMMUTABLE STRICT;

CREATE FUNCTION set_bit_on(varbit, int8)
	RETURNS varbit
	AS 'MODULE_PATHNAME'
	LANGUAGE C
	IMMUTABLE STRICT
	PARALLEL SAFE;

CREATE AGGREGATE set_bits(int8)
(
	sfunc = set_bit_on,
	stype = varbit,
	initcond = '0'
);

CREATE FUNCTION fast_no_copy_bit_or(varbit, varbit)
	RETURNS varbit
	AS 'MODULE_PATHNAME'
	LANGUAGE C
	IMMUTABLE STRICT
	PARALLEL SAFE;

CREATE AGGREGATE fast_bit_or(varbit)
(
	sfunc = fast_no_copy_bit_or,
	stype = varbit
);
