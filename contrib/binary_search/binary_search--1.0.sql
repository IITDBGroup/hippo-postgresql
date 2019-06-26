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
