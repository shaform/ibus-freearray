#!/usr/bin/env python3
#Set the nth of short codes
# ??? It's verrry slow!!!!

db = 'freearray.db'
shortcin = 'array-shortcode.cin'

convert_str = "qwertyuiopasdfghjkl;zxcvbnm,./"

def str_to_kcs(s):
	codes = [];
	for c in s:
		codes.append(convert_str.find(c) + 1)
	return codes
def kcs_to_ac(kcs):
	ac = 0;
	shift = 24
	for kc in kcs:
		ac |= (kc << shift)
		shift -= 6
		if shift <= 0: break
	return ac


def str_to_ac(str):
	return kcs_to_ac(str_to_kcs(str))

import sqlite3
import os

	
conn = sqlite3.connect(db)
c = conn.cursor()

with open(shortcin, 'r') as f:
	begin_of_def = False
	sh = ' '
	sh_num = 1
	for line in f:
		if not begin_of_def:
			if line[0] != '#':
				begin_of_def = True
			else:
				continue

		t = line.partition('\t')

		if (t[0].strip() == sh):
			sh_num = sh_num +1
		else:
			sh_num = 1
			sh = t[0].strip()

			
		c.execute('update TC_SHORT set nth=? where word=? and code=?', (sh_num, t[2].strip(), str_to_ac(t[0].strip())))

conn.commit()
c.close()
