/**
 * chewingutil.h
 *
 * Copyright (c) 1999, 2000, 2001
 *	Lu-chuan Kung and Kang-pen Chen.
 *	All rights reserved.
 *
 * Copyright (c) 2004, 2005, 2006
 *	libchewing Core Team. See ChangeLog for details.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file.
 */

#ifndef _CHEWING_UTIL_H
#define _CHEWING_UTIL_H

#include "chewing-private.h"
#ifdef FAFT_CHEWING
#ifndef ZUIN_IGNORE
#define ZUIN_IGNORE 0
#define ZUIN_ABSORB 1
#define ZUIN_COMMIT 2
#define ZUIN_KEY_ERROR 4
#define ZUIN_ERROR 8
#define ZUIN_NO_WORD 16
#endif
#endif

#define SYMBOL_KEY_OK 0
#define SYMBOL_KEY_ERROR 1
#define DECREASE_CURSOR 1
#define NONDECREASE_CURSOR 0

#ifdef FAFT_CHEWING
int MakeOutput(ChewingOutput *, ChewingData *);
int FindSymbolKey(const char *sym);
int CountReleaseNum(ChewingData *);
void KillFromLeft(ChewingData *, int);
#endif

#ifndef FAFT_CHEWING
void AutoLearnPhrase( ChewingData *pgdata );
void SetUpdatePhraseMsg( ChewingData *pgdata, char *addWordSeq, int len, int state );
#endif
int NoSymbolBetween( ChewingData *pgdata, int begin, int end );
#ifndef FAFT_CHEWING
int ChewingIsEntering( ChewingData *pgdata );
#endif
void CleanAllBuf( ChewingData * );
int SpecialSymbolInput( int key, ChewingData *pgdata );
int FullShapeSymbolInput( int key, ChewingData *pgdata );
#ifndef FAFT_CHEWING
int EasySymbolInput( int key, ChewingData *pgdata, ChewingOutput *pgo );
#endif
int SymbolInput( int key, ChewingData *pgdata );
int SymbolChoice( ChewingData *pgdata, int sel_i );
int HaninSymbolInput( ChoiceInfo *pci, AvailInfo *pai, const uint16 phoneSeq[],  int candPerPage );
int WriteChiSymbolToBuf( wch_t csBuf[], int csBufLen, ChewingData *pgdata );
#ifndef FAFT_CHEWING
int ReleaseChiSymbolBuf( ChewingData *pgdata, ChewingOutput *);
#endif
int AddChi( uint16 phone, ChewingData *pgdata );
int CallPhrasing( ChewingData *pgdata );
int MakeOutputWithRtn( ChewingOutput *pgo, ChewingData *pgdata, int keystrokeRtn );
void MakeOutputAddMsgAndCleanInterval( ChewingOutput *pgo, ChewingData *pgdata );
int AddSelect( ChewingData *pgdata, int sel_i );
#ifndef FAFT_CHEWING
int CountSelKeyNum( int key, ChewingData *pgdata );
#endif
int CountSymbols( ChewingData *pgdata, int to );
int PhoneSeqCursor( ChewingData *pgdata );
int ChewingIsChiAt( int cursor, ChewingData *pgdata );
int ChewingKillChar(
		ChewingData *pgdata, 
		int chiSymbolCursorToKill, 
		int minus );
void RemoveSelectElement( int i, ChewingData *pgdata );
int IsPreferIntervalConnted( int cursor, ChewingData *pgdata );
int OpenSymbolChoice( ChewingData *pgdata );

int InitSymbolTable( const char *prefix );
#ifndef FAFT_CHEWING
int InitEasySymbolInput(const char *prefix);
#endif

#endif

