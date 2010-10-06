#
# ibus-freearray - FreeArray for Test for The Input Bus
#
# Copyright (c) 2010
# 	Yong-Siang Shih (Shaform) <shaform at gmail.com>
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

m4_define([package_name], [ibus-freearray])

# if not 1, append datestamp to the version number.m4_define([ibus_released], [0])
m4_define([ibus_major_version], [0])
m4_define([ibus_minor_version], [0])
m4_define([ibus_micro_version], [0])
m4_define(ibus_maybe_datestamp,
    m4_esyscmd([if test x]ibus_released[ != x1; then date +.%Y%m%d | tr -d '\n\r'; fi]))

m4_define([ibus_version],
    ibus_major_version.ibus_minor_version.ibus_micro_version[]ibus_maybe_datestamp)

AC_INIT([package_name], [ibus_version], [http://code.google.com/p/ibus/issues/entry], [package_name])
AM_INIT_AUTOMAKE([1.10])
AC_GNU_SOURCE


AC_CONFIG_MACRO_DIR([m4])

# define PACKAGE_VERSION_* variables
AS_VERSION
AS_NANO
AC_PROG_CC
AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_RANLIB

# check ibus
PKG_CHECK_MODULES(IBUS, [
    ibus-1.0 >= 1.2
])


AM_PATH_PYTHON([2.5])


# check sqlite3
PKG_CHECK_MODULES(SQLITE3, [
    sqlite3
])

# check freearray
PKG_CHECK_MODULES(FREEARRAY, [
    freearray
])


AC_CONFIG_HEADERS([config.h])
# OUTPUT files
AC_CONFIG_FILES(Makefile
ibus-freearray.spec
src/Makefile
src/freearray.xml.in
setup/Makefile
setup/ibus-setup-freearray
setup/config.py
data/Makefile
icons/Makefile
m4/Makefile
)

AC_OUTPUT
