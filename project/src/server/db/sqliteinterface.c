/*
* filename : sqliteinterface.h
* path: ./
* author: liehu1119
* date : 2013-04-06
* version : 1.0
* note : invoke the sqlite interface. 
*/
#include "common.h"
#include "sqliteinterface.h"

struct sqlite3 *g_db = NULL;
char g_SqlBuf[SQLMAXBUF + 1];
/*****************************************************************************
 * Function :  InitConnection
 * Author:      liuhuizhe
 * Date:         2013-04-06
 * Input:        N/A
 * Output:      N/A
 * Return:      DB_ERROR_TYPE
 * other:        N/A
 * Description: init g_db
******************************************************************************/
long InitConnection(void)
{
    if (NULL != g_db)
    {
        //close db first
        if (DB_ERROR_OK != CloseConnection(g_db))
        {
            return DB_ERROR_FAILURE;
        }
    }
    g_db = NULL;
    return DB_ERROR_OK;
}

/*****************************************************************************
 * Function :  OpenConnection
 * Author:      liuhuizhe
 * Date:         2013-04-06
 * Input:        char *pDBName  db name
 * Output:      N/A
 * Return:      DB_ERROR_TYPE
 * other:        N/A
 * Description: open/create database
******************************************************************************/
long OpenConnection(char *pDBName)
{
    //check param
    if (NULL == pDBName || strlen(pDBName) <= 0)
    {
        return DB_ERROR_INVALID_PARAM;
    }
    if (NULL != g_db)
    {
        return DB_ERROR_OK;
    }
    //invoke sqlite interface to open/create database
    int iResult = sqlite3_open((const char *)pDBName,&g_db);
    if(SQLITE_OK != iResult)
    {
      return DB_ERROR_FAILURE;
    }
    return DB_ERROR_OK;
}

/*****************************************************************************
 * Function :  CloseConnection
 * Author:      liuhuizhe
 * Date:         2013-04-06
 * Input:        sqlite3 *pDB  dbhandle
 * Output:      N/A
 * Return:      DB_ERROR_TYPE
 * other:        N/A
 * Description: close database
******************************************************************************/
long CloseConnection(sqlite3 *pDB)
{
    if (NULL == g_db)
    {
        return DB_ERROR_OK;
    }
    //invoke sqlite interface to close database
    int iResult = sqlite3_close(g_db);
    
    if(SQLITE_OK != iResult)
    {
      return DB_ERROR_FAILURE;
    }
    g_db = NULL;
    return DB_ERROR_OK;
}
/*****************************************************************************
 * Function : ExecSQL
 * Author:      liuhuizhe
 * Date:         2013-04-06
 * Input:        char *pSql
 * Output:      N/A
 * Return:      DB_ERROR_TYPE
 * other:        N/A
 * Description: close database
******************************************************************************/
long ExecSQL(char *pSql)
{
    if (NULL == pSql || strlen(pSql) <= 0 || NULL == g_db)
    {
        return DB_ERROR_INVALID_PARAM;
    }
    char *pErrMsg = NULL;
    int iResult = sqlite3_exec(g_db, pSql, 0, 0, &pErrMsg);
    if(SQLITE_OK != iResult)
    {
      printf("%s\n", pErrMsg);
      sqlite3_free(pErrMsg);
      return DB_ERROR_FAILURE;
    }
    sqlite3_free(pErrMsg);
    return DB_ERROR_OK;
}
/*****************************************************************************
 * Function : GetTable
 * Author:      liuhuizhe
 * Date:         2013-04-06
 * Input:        char *pSql
 *                  char ***pTableResult
 *                  int *pRow
 *                  int *pColumn
 * Output:      N/A
 * Return:      DB_ERROR_TYPE
 * other:        N/A
 * Description: exec select sql
******************************************************************************/
long GetTable(char *pSql, char ***pTableResult, int *pRow, int *pColumn)
{
    if (NULL == pSql || strlen(pSql) <= 0 || NULL == pTableResult || NULL == pRow || NULL == pColumn || NULL == g_db)
    {
        return DB_ERROR_INVALID_PARAM;
    }
    char* pErrMsg = NULL;
    int iResult = sqlite3_get_table(g_db, pSql, pTableResult, pRow, pColumn,&pErrMsg);

    if (SQLITE_OK != iResult)
    {
        printf("%s\n", pErrMsg);
        sqlite3_free(pErrMsg);
        return DB_ERROR_FAILURE;
    }
    sqlite3_free(pErrMsg);
    return DB_ERROR_OK;
}
/*****************************************************************************
 * Function : ReleaseTableResult
 * Author:      liuhuizhe
 * Date:         2013-04-06
 * Input:        char ***pTableResult
 * Output:      N/A
 * Return:      DB_ERROR_TYPE
 * other:        N/A
 * Description: release select result
******************************************************************************/

void ReleaseTableResult(char **pTableResult)
{
    if (NULL != pTableResult)
    {
        sqlite3_free_table(pTableResult);
    }
}

/*
------------------------------------------------------------------------
yyyy-mm-dd		name		description
------------------------------------------------------------------------
2013-04-06		liuhuizhe	        init
------------------------------------------------------------------------
*/

