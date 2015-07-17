#include "database.h"
void databaseLibInit(void)
{
   sqlite3 *db;
   if(SQLITE_OK != sqlite3_open(DATABASE_FILE, &db))
   {
       Printf("databaseLibInit Fail!\n");
   }
   else
   Printf("databaseLibInit Success!\n");
}
