#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# ibus-faft setup dialog
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

import sys
import os
import gobject
import gtk
import ibus
import config

class Setup:
	def __init__(self):
		"""Constructor for the ibus-faft Setup dialog."""

		# Set up settings
		self.setting_types = ('General', 'Select', 'Modules')

		settings = list()
		settings.append((('AutoInput','Auto input mode', 1),
				('AutoClear','Auto clear mode', 1),
				('EscClear','ESC clear all buffer', 0)))

		settings.append((('SpaceSelect', 'Space to select', 1),
				('AnyRowSelect', 'Select by any row of keys', 1),
				('ChooseBackward', 'Choose phrase from backward', 1)))

		settings.append((('ShortCode', 'Short Code Input', 0),
				('PreInput', 'Pre-Input', 1),
				('PhraseInput', 'Phrase Input', 1),
				('ArraySymbol', 'Array Symbol Input', 1),
				('EasySymbol', 'Easy Symbol Input', 1)))

		self.all_settings_check = dict()
		self.__current_settings = {'KbType' : ['Keyboard layout', 0],
				'MaxSize' : ['Preedit buffer length', 20]}

		for i in range(len(self.setting_types)):

			self.all_settings_check[self.setting_types[i]] = list()
			for j in range(len(settings[i])):
				self.all_settings_check[self.setting_types[i]].append(settings[i][j][0])
				self.__current_settings[settings[i][j][0]] = list(settings[i][j][1:])

		# Connect to ibus
		self.__bus = ibus.Bus()
		self.__config = self.__bus.get_config()

		# Sync the settings
		self.__config.connect("value-changed", self.on_value_changed, None)

		# Produce Settings dialog
		self.__create_ui()

	def __create_ui(self):
		# Setup Dialog
		self.__window = gtk.Dialog('ibus-faft Setup', None, gtk.DIALOG_MODAL)
		icon_file = os.path.join(config.datadir, 'ibus-faft', 'icons', "ibus-faft.svg")
		self.__window.set_icon_from_file(icon_file)

		# Tabs
		notebook = gtk.Notebook()

		self.__labels = dict()
		self.__vboxs = dict()
		self.__buttons = dict()

		# Load Settings
		self.__load()

		for type in self.setting_types:
			self.__labels[type] = gtk.Label(type)
			self.__vboxs[type] = gtk.VBox()
			for item in self.all_settings_check[type]:
				self.__buttons[item] = gtk.CheckButton(self.__current_settings[item][0])

				if self.__current_settings[item][1] == 1:
					self.__buttons[item].set_active(True)
				else:
					self.__buttons[item].set_active(False)

				self.__vboxs[type].pack_end(self.__buttons[item])

			notebook.append_page(self.__vboxs[type], self.__labels[type])

		# Keyboard Type
		kbt_l = gtk.Label(self.__current_settings['KbType'][0])

		kbt_m = gtk.ListStore(str)
		kbt_m.set(kbt_m.append(), 0, 'default')
		kbt_m.set(kbt_m.append(), 0, 'dvorak')

		kbt_cell = gtk.CellRendererText()
		self.__kbt_cb = gtk.ComboBox(kbt_m)
		self.__kbt_cb.pack_start(kbt_cell, True)
		self.__kbt_cb.add_attribute(kbt_cell, 'text', 0)
		self.__kbt_cb.set_active(self.__current_settings['KbType'][1])


		kbt_hb = gtk.HBox()
		kbt_hb.pack_start(kbt_l)
		kbt_hb.pack_start(self.__kbt_cb)


		# Maximum Chinese characters
		buf_l = gtk.Label(self.__current_settings['MaxSize'][0])
		buf_adj = gtk.Adjustment(self.__current_settings['MaxSize'][1], 15, 35, 1, 5, 0)
		self.__buf_spin = gtk.SpinButton(buf_adj, 1, 0)

		buf_hb = gtk.HBox()
		buf_hb.pack_start(buf_l)
		buf_hb.pack_start(self.__buf_spin)

		self.__vboxs[self.setting_types[0]].pack_start(buf_hb)
		self.__vboxs[self.setting_types[0]].pack_start(kbt_hb)


		self.__window.vbox.add(notebook)
		self.__window.add_button(gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL)
		self.__window.add_button(gtk.STOCK_OK, gtk.RESPONSE_OK)

		self.__window.show_all()


	def run(self):
		ret = self.__window.run()
		if ret == gtk.RESPONSE_OK:
			self.apply()
			self.__window.destroy()

	def apply(self):
		for key in self.__current_settings:
			if key == 'MaxSize':
				self.__current_settings[key][1] = int(self.__buf_spin.get_value())
			elif key == 'KbType':
				self.__current_settings[key][1] = int(self.__kbt_cb.get_active())
			else:
				self.__current_settings[key][1] = int(self.__buttons[key].get_active())

			self.__write(key, self.__current_settings[key][1])


	def on_value_changed(self, config, section, name, value, data):
		if section == 'engine/FAFT' and name in self.__current_settings:
			if name == 'KbType':
				self.__kbt_cb.set_active(int(value))

			elif name == 'MaxSize':
				self.__buf_spin.set_value(int(value))

			elif name in self.__current_settings:
					self.__buttons[name].set_active(bool(self.__current_settings[name][1]))

	def __load(self):
		for key in self.__current_settings:
			self.__current_settings[key][1] = self.__read(key, self.__current_settings[key][1])

	def __read(self, name, v):
		return self.__config.get_value("engine/FAFT", name, v)

	def __write(self, name, v):
		return self.__config.set_value("engine/FAFT", name, v)

if __name__ == '__main__':
	Setup().run()
