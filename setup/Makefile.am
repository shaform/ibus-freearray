#
# ibus-freearray - FreeArray for The Input Bus
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

ibus_setup_freearray_PYTHON = \
	main.py \
	$(NULL)

nodist_ibus_setup_freearray_PYTHON = config.py

ibus_setup_freearraydir = $(datadir)/ibus-freearray/setup

libexec_SCRIPTS = ibus-setup-freearray

CLEANFILES = \
	*.pyc \
	$(NULL)

EXTRA_DIST = \
	ibus-setup-freearray.in \
	$(NULL)

test:
	$(ENV) DBUS_DEBUG=true \
		   LANG=en_US \
		   PYTHONPATH=$(abs_top_srcdir):$(pyexecdir) \
		   $(PYTHON) $(srcdir)/main.py

