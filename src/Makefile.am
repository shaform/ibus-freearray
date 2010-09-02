#
# ibus-faft - FreeArray for Test for The Input Bus
#
# Copyright (c) 2010
# 	Yong-Siang Shih (Shaform) <shaform@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# See the file "COPYING" for information on usage and redistribution
# of this file.

chewingdir = chewing
freearraydir = freearray
faftdir = faft
mixeddir = mixed

INCLUDES = \
	-I. \
	-I$(mixeddir) \
	$(NULL)

chewing_source = \
	$(chewingdir)/global.h \
	$(chewingdir)/global-private.h \
	$(chewingdir)/private.h \
	$(chewingdir)/chewing-private.h \
	$(chewingdir)/chewing-definition.h \
	$(chewingdir)/chewing-utf8-util.h \
	$(chewingdir)/chewing-utf8-util.c \
	$(chewingdir)/plat_mmap.h \
	$(chewingdir)/plat_posix.h \
	$(chewingdir)/plat_types.h \
	$(chewingdir)/plat_mmap_posix.c \
	$(chewingdir)/choice-private.h \
	$(chewingdir)/choice.c \
	$(chewingdir)/dict-private.h \
	$(chewingdir)/dict.c \
	$(chewingdir)/tree-private.h \
	$(chewingdir)/tree.c \
	$(chewingdir)/chewingutil.h \
	$(chewingdir)/chewingutil.c \
	$(chewingdir)/chewingio.h \
	$(chewingdir)/chewingio.c \
	$(chewingdir)/mod_aux.h \
	$(chewingdir)/mod_aux.c \
	$(NULL)
freearray_source = \
	$(freearraydir)/code.h \
	$(freearraydir)/code.c \
	$(freearraydir)/kbtype.h \
	$(freearraydir)/kbtype.c \
	$(freearraydir)/tables.h \
	$(freearraydir)/tables.c \
	$(NULL)
faft_source = \
	$(faftdir)/acdata.h \
	$(faftdir)/acdata.c \
	$(faftdir)/faft-util.h \
	$(faftdir)/faft-util.c \
	$(faftdir)/faftio.h \
	$(faftdir)/faftio.c \
	$(faftdir)/faft-def.h \
	$(faftdir)/faft.h \
	$(NULL)
mixed_source = \
	$(mixeddir)/char-private.h \
	$(NULL)
noinst_LIBRARIES = libfaft.a
libfaft_a_SOURCES = \
	$(freearray_source) \
	$(chewing_source) \
	$(faft_source) \
	$(mixed_source) \
	$(NULL)
libfaft_a_CFLAGS = \
	-DFAFT_CHEWING \
	$(NULL)

libexec_PROGRAMS = ibus-engine-faft
ibus_engine_faft_SOURCES = \
	main.c \
	ibus-faft-engine.c \
	ibus-faft-engine.h \
	properties.c \
	properties.h \
	$(NULL)
ibus_engine_faft_CFLAGS = \
	@IBUS_CFLAGS@ \
	@SQLITE3_CFLAGS@ \
	-DPKGDATADIR=\"$(pkgdatadir)\" \
	-DLIBEXECDIR=\"$(libexecdir)\" \
	-DFAFT_CHEWING \
	$(NULL)
ibus_engine_faft_LDFLAGS = \
	@IBUS_LIBS@ \
	@SQLITE3_LIBS@ \
	$(NULL)
ibus_engine_faft_LDADD = \
	libfaft.a \
	$(NULL)

component_DATA = \
	faft.xml \
	$(NULL)
componentdir = @datadir@/ibus/component


EXTRA_DIST = \
	faft.xml.in \
	$(NULL)

CLEANFILES = \
	faft.xml \
	$(NULL)

faft.xml: faft.xml.in
	( \
		libexecdir=${libexecdir}; \
		pkgdatadir=${pkgdatadir}; \
		s=`cat $<`; \
		eval "echo \"$${s}\""; \
	) > $@

test: ibus-engine-faft
	$(builddir)/ibus-engine-faft







noinst_PROGRAMS = faft-test
faft_test_SOURCES = \
	test.c \
	$(NULL)
faft_test_CFLAGS = \
	@IBUS_CFLAGS@ \
	@SQLITE3_CFLAGS@ \
	-DPKGDATADIR=\"$(pkgdatadir)\" \
	-DLIBEXECDIR=\"$(libexecdir)\" \
	-DFAFT_CHEWING \
	$(NULL)
faft_test_LDFLAGS = \
	@IBUS_LIBS@ \
	@SQLITE3_LIBS@ \
	$(NULL)
faft_test_LDADD = \
	libfaft.a \
	$(NULL)