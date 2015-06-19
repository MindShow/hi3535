#ifndef _CFG_FILE_OPERA_H__
#define _CFG_FILE_OPERA_H__
#ifdef __cplusplus
extern "C" {
#endif 

int GetProfileString(char *profile, char *AppName, char *KeyName, char *KeyVal );
int SetProfileString(char *profile, char *KeyName, char *OldKeyVal, char *NewKeyVal);

#ifdef __cplusplus
}
#endif
#endif
