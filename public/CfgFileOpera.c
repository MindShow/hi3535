#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <pthread.h>

#define KEYVALLEN 256
/* 
功能： 删除左边的空格，提取出该行有用的信息。

*/
static char *l_trim(char * szOutput, const char *szInput)
{
    assert(szInput != NULL);
    assert(szOutput != NULL);
    assert(szOutput != szInput);
    while(*szInput != '\0' && isspace(*szInput))// isspace(c):判断是否为空格。若参数c为空格字符，则返回TRUE，否则返回NULL(0)。
    {
       ++szInput; 
    }
    return strcpy(szOutput, szInput);
}
#if 0
/*   删除右边的空格   */
static char *r_trim(char *szOutput, const char *szInput)
{
    char *p = NULL;
    assert(szInput != NULL);
    assert(szOutput != NULL);
    assert(szOutput != szInput);
    strcpy(szOutput, szInput);
    for(p = szOutput + strlen(szOutput) - 1; p >= szOutput && isspace(*p); --p);
    *(++p) = '\0';
    return szOutput;
}
#endif

/*   删除两边的空格   */
static char * a_trim(char * szOutput, const char * szInput)
{
    char *p = NULL;
    assert(szInput != NULL);
    assert(szOutput != NULL);
    l_trim(szOutput, szInput);
    for(p = szOutput + strlen(szOutput) - 1;p >= szOutput && isspace(*p); --p);
    *(++p) = '\0';
    return szOutput;
}


int GetProfileString(char *profile, char *AppName, char *KeyName, char *KeyVal )
{
    char appname[32],keyname[32];
    char *buf,*c;
    char buf_i[KEYVALLEN], buf_o[KEYVALLEN];
    FILE *fp;
    int found=0; /* 1 AppName 2 KeyName */
    if( (fp=fopen( profile,"r" ))==NULL )
    {
        printf( "openfile [%s] error [%s]\n",profile,strerror(errno) );
        return(-1);
    }
    fseek( fp, 0, SEEK_SET );
    memset( appname, 0, sizeof(appname) );
    sprintf( appname,"[%s]", AppName );

    while( !feof(fp) && fgets( buf_i, KEYVALLEN, fp )!=NULL )
    {
        l_trim(buf_o, buf_i);
        if( strlen(buf_o) <= 0 )
            continue;
        buf = NULL;
        buf = buf_o;

        if( found == 0 )
        {
            if( buf[0] != '[' ) 
            {
                continue;
            } 
            else if ( strncmp(buf,appname,strlen(appname))==0 )
            {
                found = 1;
                continue;
            }

        } 
        else if( found == 1 )
        {
            if( buf[0] == '#' )
            {
                continue;
            } 
            else if ( buf[0] == '[' ) 
            {
                break;
            } 
            else 
            {
                if( (c = (char*)strchr(buf, '=')) == NULL )
                    continue;
                memset( keyname, 0, sizeof(keyname) );

                sscanf( buf, "%[^=|^ |^\t]", keyname );// 获取配置文件中的关键字
                if( strcmp(keyname, KeyName) == 0 )
                {
                    sscanf( ++c, "%[^\n]", KeyVal );//获取关键字值
                    char *KeyVal_o = (char *)malloc(strlen(KeyVal) + 1);
                    if(KeyVal_o != NULL)
                    {
                        memset(KeyVal_o, 0, sizeof(strlen(KeyVal) + 1));
                        a_trim(KeyVal_o, KeyVal);// 删除关键字值左右两边的空格
                        if(KeyVal_o && strlen(KeyVal_o) > 0)
                        {
                            strcpy(KeyVal, KeyVal_o);
                        }
                        free(KeyVal_o);
                        KeyVal_o = NULL;
                    }
                    found = 2;
                    break;
                } 
                else 
                {
                    continue;
                }
            }
        }
    }
    fclose(fp);
    if( found == 2 )
        return(0);
    else
        return(-1);
}

pthread_mutex_t WriteCfgMutex = PTHREAD_MUTEX_INITIALIZER;
void SetProfileString(char *profile, char *KeyName, char *OldKeyVal, char *NewKeyVal)
{
    char buf[128];
    printf("======OOOOOO:%s, =======NNNNNNN:%s\n",OldKeyVal, NewKeyVal);
    sprintf(buf,"sed -i 's/%s = %s/%s = %s/' %s", KeyName, OldKeyVal, KeyName, NewKeyVal, profile);
    printf("buf:%s\n",buf);
    pthread_mutex_lock(&WriteCfgMutex);    
    system(buf);
    pthread_mutex_unlock(&WriteCfgMutex);
}
