/** 
 * @file tables.c
 * @brief Handles data tables
 * c version of tables.cpp
 *
 */

/**
 * FreeArray Input Method Library
 * 
 * Copyright (c) 2009-2010
 * 	Yong-Siang Shih (Shaform) <shaform@gmail.com>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file.
 */
#include <cstring>
#include <cstdio>
#include <climits>
#include <sqlite3.h>
#include "tables.h"
#include "code.h"
using namespace std;

#define DB_FILE "freearray.db"
#define TC_TABLE "TC_CHAR"
#define TC_SHORT "TC_SHORT"
#define TC_SPECIAL "TC_SPECIAL"
#define TC_PHRASE "TC_PHRASE"
#define TC_SIGN "TC_SIGN"

#ifndef PATH_MAX
#define PATH_MAX 260
#endif

static sqlite3 *db = NULL;

static sqlite3_stmt *qw_stmt = NULL;
static sqlite3_stmt *qwq_stmt = NULL;
static sqlite3_stmt *qwr_stmt = NULL;
static sqlite3_stmt *qsh_stmt = NULL;
static sqlite3_stmt *qsp_stmt = NULL;
static sqlite3_stmt *qphr_stmt = NULL;
static sqlite3_stmt *qsym_stmt = NULL;

static int ret_qw = 0;
static int ret_qwq = 0;
static int ret_qwr = 0;
static int ret_qsh = 0;
static int ret_qsp = 0;
static int ret_qphr = 0;
static int ret_qsym = 0;


int faft_table_init(const char *DB_PREFIX)
{
	if (db != NULL)
		return 1;

	char DB_PATH[PATH_MAX];
	sprintf(DB_PATH, "%s/%s", DB_PREFIX, DB_FILE);
	if (sqlite3_open_v2(DB_PATH, &db, SQLITE_OPEN_READONLY, NULL) == SQLITE_OK) {
		sqlite3_prepare_v2(db,
				("SELECT word FROM " TC_TABLE " WHERE code=? ORDER BY feq DESC"),
				-1,
				&qw_stmt, NULL);
		sqlite3_prepare_v2(db,
				("SELECT word, code FROM ("
				 "SELECT word, code, feq+327781 as feq FROM " TC_TABLE " WHERE code=? "
				 "UNION "
				 "SELECT word, code, feq FROM " TC_TABLE " WHERE code>? AND code<? "
				 "ORDER BY feq DESC LIMIT 10"
				 ") ORDER BY feq DESC, code ASC"),
				-1,
				&qwq_stmt, NULL);
		sqlite3_prepare_v2(db,
				("SELECT code FROM " TC_TABLE " WHERE word=? ORDER BY code LIMIT 1"),
				-1,
				&qwr_stmt, NULL);
		sqlite3_prepare_v2(db,
				("SELECT word FROM " TC_SHORT " WHERE code=? ORDER BY nth"),
				-1,
				&qsh_stmt, NULL);
		sqlite3_prepare_v2(db,
				("SELECT word, ac FROM " TC_SPECIAL " WHERE code=?"),
				-1,
				&qsp_stmt, NULL);
		sqlite3_prepare_v2(db,
				("SELECT word FROM " TC_PHRASE " WHERE code=?"),
				-1,
				&qphr_stmt, NULL);
		sqlite3_prepare_v2(db,
				("SELECT word FROM " TC_SIGN " WHERE code=?"),
				-1,
				&qsym_stmt, NULL);
	} else {
		db = NULL;
		return 0;
	}

	return 1;
}

int faft_table_exit()
{
	if (db != NULL)
		return 0;

	sqlite3_reset(qw_stmt);
	sqlite3_finalize(qw_stmt);

	sqlite3_reset(qwq_stmt);
	sqlite3_finalize(qwq_stmt);

	sqlite3_reset(qwr_stmt);
	sqlite3_finalize(qwr_stmt);

	sqlite3_reset(qsh_stmt);
	sqlite3_finalize(qsh_stmt);

	sqlite3_reset(qsp_stmt);
	sqlite3_finalize(qsp_stmt);

	sqlite3_reset(qphr_stmt);
	sqlite3_finalize(qphr_stmt);

	sqlite3_reset(qsym_stmt);
	sqlite3_finalize(qsym_stmt);

	sqlite3_close(db);

	db = NULL;
	qw_stmt = qwq_stmt = qwr_stmt = qsh_stmt = qsp_stmt = qphr_stmt = NULL;
	ret_qw = ret_qwq = ret_qwr = ret_qsh = ret_qsp = ret_qphr = ret_qsym = 0;
	return 1;
}
static void get_column(char *dest, int len, sqlite3_stmt *stmt)
{
	const char *re_str = (const char *) sqlite3_column_text(stmt, 0);
	strncpy(dest, re_str, len);
	if (dest[len-1] != '\0')
		dest[len-1] = '\0';
}
static ArrayCode get_column2(sqlite3_stmt *stmt)
{
	return (ArrayCode )sqlite3_column_int(stmt, 1);
}
int faft_table_get_special(Word *w, ArrayCode ac)
{
	sqlite3_reset(qsp_stmt);
	sqlite3_bind_int(qsp_stmt, 1, (int) ac);
	ret_qsp = sqlite3_step(qsp_stmt);

	return faft_table_get_special_again(w);
}
int faft_table_get_special_again(Word  *w)
{
	if (ret_qsp == SQLITE_ROW) {
		if (w)
			get_column(w->word, FAFT_WORD_MAX, qsp_stmt);

		return 1;
	}
	return 0;
}
ArrayCode faft_table_get_special_ac()
{
	if (ret_qsp == SQLITE_ROW)
		return get_column2(qsp_stmt);
	else
		return 0;
}
int faft_table_get_char_first(Word *w, ArrayCode ac)
{
	sqlite3_reset(qw_stmt);
	sqlite3_bind_int(qw_stmt, 1, (int) ac);
	ret_qw = sqlite3_step(qw_stmt);

	return faft_table_get_char_again(w);

}

int faft_table_get_char_again(Word *w)
{
	if (ret_qw == SQLITE_ROW) {
		if (w)
			get_column(w->word, FAFT_WORD_MAX, qw_stmt);

		return 1;
	}
	return 0;
}

int faft_table_get_char_next(Word *w)
{
	if (ret_qw == SQLITE_ROW) {
		ret_qw = sqlite3_step(qw_stmt);

		return faft_table_get_char_again(w);
	}
	return 0;
}

int faft_table_get_quick_char_first(Word *w, ArrayCode ac)
{
	sqlite3_reset(qwq_stmt);
	sqlite3_bind_int(qwq_stmt, 1, (int) ac);
	sqlite3_bind_int(qwq_stmt, 2, (int) ac);
	sqlite3_bind_int(qwq_stmt, 3, (int) faft_keycode_generate_next(ac));
	ret_qwq = sqlite3_step(qwq_stmt);

	return faft_table_get_quick_char_again(w);

}

int faft_table_get_quick_char_again(Word *w)
{
	if (ret_qwq == SQLITE_ROW) {
		if (w)
			get_column(w->word, FAFT_WORD_MAX, qwq_stmt);

		return 1;
	}
	return 0;
}

int faft_table_get_quick_char_next(Word *w)
{
	if (ret_qwq == SQLITE_ROW) {
		ret_qwq = sqlite3_step(qwq_stmt);

		return faft_table_get_quick_char_again(w);
	}
	return 0;
}
ArrayCode faft_table_get_quick_char_ac()
{
	if (ret_qwq == SQLITE_ROW)
		return get_column2(qwq_stmt);
	else
		return 0;
}
ArrayCode faft_table_get_ac(const char *s)
{
	sqlite3_reset(qwr_stmt);
	sqlite3_bind_text(qwr_stmt, 1, s, -1, SQLITE_TRANSIENT);
	ret_qwr = sqlite3_step(qwr_stmt);

	return faft_table_get_ac_again();
}
ArrayCode faft_table_get_ac_again()
{
	if (ret_qwr == SQLITE_ROW)
		return (ArrayCode )sqlite3_column_int(qwr_stmt, 0);
	else
		return 0;
}

int faft_table_get_phrase_first_strn(char *s, int len, ArrayCode ac)
{
	sqlite3_reset(qphr_stmt);
	sqlite3_bind_int(qphr_stmt, 1, (int) ac);
	ret_qphr = sqlite3_step(qphr_stmt);

	return faft_table_get_phrase_again_strn(s, len);
}
int faft_table_get_phrase_again_strn(char *s, int len)
{
	if (ret_qphr == SQLITE_ROW) {
		if (s && len > 0)
			get_column(s, len, qphr_stmt);

		return 1;
	} else
		return 0;
}
int faft_table_get_phrase_next_strn(char *s, int len)
{
	if (ret_qphr == SQLITE_ROW) {
		ret_qphr = sqlite3_step(qphr_stmt);

		return faft_table_get_phrase_again_strn(s, len);
	}
	return 0;
}

int faft_table_get_short_first(Word *w, ArrayCode ac)
{
	sqlite3_reset(qsh_stmt);
	sqlite3_bind_int(qsh_stmt, 1, (int) ac);
	ret_qsh = sqlite3_step(qsh_stmt);

	return faft_table_get_short_again(w);
}
int faft_table_get_short_again(Word *w)
{
	if (ret_qsh == SQLITE_ROW) {
		if (w)
			get_column(w->word, FAFT_WORD_MAX, qsh_stmt);

		return 1;
	}
	return 0;
}
int faft_table_get_short_next(Word *w)
{
	if (ret_qsh == SQLITE_ROW) {
		ret_qsh = sqlite3_step(qsh_stmt);

		return faft_table_get_short_again(w);
	}
	return 0;
}
ArrayCode faft_table_get_short_ac()
{
	if (ret_qsh == SQLITE_ROW)
		return get_column2(qsh_stmt);
	else
		return 0;
}


int faft_table_get_symbol(Word *w, ArrayCode ac)
{
	sqlite3_reset(qsym_stmt);
	sqlite3_bind_int(qsym_stmt, 1, (int) ac);
	ret_qsym = sqlite3_step(qsym_stmt);

	return faft_table_get_symbol_again(w);
}
int faft_table_get_symbol_again(Word *w)
{
	if (ret_qsym == SQLITE_ROW) {
		if (w)
			get_column(w->word, FAFT_WORD_MAX, qsym_stmt);

		return 1;
	}
	return 0;
}
