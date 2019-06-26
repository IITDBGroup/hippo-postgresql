# contrib/binary_search/Makefile

MODULES = binary_search

EXTENSION = binary_search
DATA = binary_search--1.0.sql
PGFILEDESC = "binary_search - binary search for integer arrays"

REGRESS = binary_search

ifdef USE_PGXS
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/binary_search
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif
