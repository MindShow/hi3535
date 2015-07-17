#ifndef __DATABASE_H__
#define __DATABASE_H___ 

#ifdef __cplusplus
extern "C" {
#endif 
#include <stdio.h>
#include "sqlite3.h" 
#include "common.h" 
#define DATABASE_FILE "/home/system.db"

void databaseLibInit(void);

#ifdef __cplusplus
}
#endif
#endif


