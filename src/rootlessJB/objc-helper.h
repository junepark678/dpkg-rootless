#include <objc/message.h>

void addObjcSymbols(void);

void *_objc_msgSend;
void* (*_objc_getClass)(const char*);
char* (*_sel_registerName)(const char *);

void *_description(void *object);
char *UTF8String(void *object);
void removeSubstring(char *s,const char *toremove);
void* NSStringFromUTF8String(const char *charString);

int validatePlist(const char *plistPath);
void createMinimalEntitlementsPlist(const char *plistPath);
char * getPlistValueForKey(const char *key, const char *plistPath);
int setPlistBoolValueForKey(int, const char *key, const char *plistPath);
char ** getPlistArrayValueForKey(const char *key, const char *plistPath,size_t *count);
void updatePlistEntitlements(const char *path);
