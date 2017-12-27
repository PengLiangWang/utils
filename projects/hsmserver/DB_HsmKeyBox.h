/*
 This Include File "DB_HsmKeyBox.h" 
 Genenated By
 Application dbaction V3.0 for Oracle
 with the action file "DB_HsmKeyBox.act".
 Create: Wed Mar 19 00:07:32 2014
*/

#ifndef _DB_HsmKeyBoxEx_INCLUDE_H_
#define _DB_HsmKeyBoxEx_INCLUDE_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include "dbutil.h"

typedef struct {
	int	keyIndex;
	char	keyType[2];
	char	keyFlag[2];
	int	keyLen;
	char	keyDataA[257];
	char	keyDataB[257];
	char	keyDac[33];
        } HsmKeyBoxEx;

int DB_HsmKeyBoxEx_read_by_keyIndex( int keyIndex__0,HsmKeyBoxEx *_a_data);
int DB_HsmKeyBoxEx_update_by_keyIndex( int keyIndex__0,HsmKeyBoxEx *_a_data);
int DB_HsmKeyBoxEx_add( HsmKeyBoxEx *_a_data);
int DB_HsmKeyBoxEx_open_select( Select_Info *_a_sInfo);
int DB_HsmKeyBoxEx_fetch_select( Select_Info *_a_sInfo,HsmKeyBoxEx *_a_data);
int DB_HsmKeyBoxEx_close_select( Select_Info *_a_sInfo);

#ifdef	__cplusplus
}
#endif


#endif
