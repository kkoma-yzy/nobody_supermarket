/*
* filename : sqliteinterface.h
* path: ./
* author: liehu1119
* date : 2013-04-06
* version : 1.0
* note : difine function which  invoke the sqlite interface. 
*/

#ifndef _SQLITEINTERFACE_
#define _SQLITEINTERFACE_

#include "sqlite3.h"

#define SQLMAXBUF   512

enum DB_ERROR_TYPE
{
    DB_ERROR = 0,
    DB_ERROR_OK,
    DB_ERROR_FAILURE,
    DB_ERROR_INVALID_PARAM
};

long InitConnection(void);
long OpenConnection(char *pDBName);
long CloseConnection(sqlite3 *pDB);
long ExecSQL(char *pSql);
long GetTable(char *pSql, char ***pTableResult, int *pRow, int *pColumn);
void ReleaseTableResult(char **pTableResult);

extern sqlite3 *g_db; 
#endif //_SQLITEINTERFACE_

/*
------------------------------------------------------------------------
yyyy-mm-dd		name		description
------------------------------------------------------------------------
2013-04-06		liuhuizhe	        init
------------------------------------------------------------------------
*/


