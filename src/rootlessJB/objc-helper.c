#include "objc-helper.h"

void addObjcSymbols(void){

	dlopen("/System/Library/Frameworks/CoreServices.framework/CoreServices",RTLD_LAZY);					
	
	_objc_getClass=(void*(*)(const char *))dlsym(RTLD_DEFAULT,"objc_getClass");
	_sel_registerName=(char*(*)(const char*))dlsym(RTLD_DEFAULT,"sel_registerName");
	_objc_msgSend=dlsym(RTLD_DEFAULT,"objc_msgSend");
	// Yes what?, I don't want to link frameworks...I'll call objc with just c
}

void* NSStringFromUTF8String(const char *charString){

	void*(*msgSendstringWithUTF8String)(void*,char*,const char *)=(void*(*)(void*,char*,const char *))_objc_msgSend;
	void* theString=msgSendstringWithUTF8String(_objc_getClass("NSString"),_sel_registerName("stringWithUTF8String:"),charString);
	return theString;

}

char *UTF8String(void *object){

	char *(*UTF8StringCall)(void*,const char*)=(char *(*)(void*,const char*))_objc_msgSend;
	char * result=UTF8StringCall(object,_sel_registerName("UTF8String"));
	return result;

}

void *_description(void *object){
	
	char *(*UTF8StringCall)(void*,const char*)=(char *(*)(void*,const char*))_objc_msgSend;
	char * result=UTF8StringCall(object,_sel_registerName("description"));
	return result;

}

char * getPlistValueForKey(const char *key, const char *plistPath){
									
	
	void* (*NSDictionary_dictionaryWithContentsOfFile)(void*,char*,void*) = (void* (*)(void*,char*,void*))_objc_msgSend;
	void* dict = NSDictionary_dictionaryWithContentsOfFile(_objc_getClass("NSDictionary"),_sel_registerName("dictionaryWithContentsOfFile:"),NSStringFromUTF8String(plistPath));
	void* (*dict_valueForKeyPath)(void*,char*,void*) = (void* (*)(void*,char*,void*))_objc_msgSend;
	void* res = dict_valueForKeyPath(dict,_sel_registerName("valueForKeyPath:"),NSStringFromUTF8String(key));
	
	return UTF8String(res);
											
}


void createMinimalEntitlementsPlist(const char *path){
	
	void* (*NSMutableDictionary_dictionary)(void*,char*) = (void* (*)(void*,char*))_objc_msgSend;
	void* dict = NSMutableDictionary_dictionary(_objc_getClass("NSMutableDictionary"),_sel_registerName("dictionary"));
	
	void* (*NSNumber_numberWithBool)(void*,char*,int) = (void* (*)(void*,char*,int))_objc_msgSend;
	void* trueBool = NSNumber_numberWithBool(_objc_getClass("NSNumber"),_sel_registerName("numberWithBool:"),1);
	void* falseBool = NSNumber_numberWithBool(_objc_getClass("NSNumber"),_sel_registerName("numberWithBool:"),0);
	
	void* (*dict_setObjectForKey)(void*,char*,void*,void*) = (void* (*)(void*,char*,void*,void *))_objc_msgSend;
	dict_setObjectForKey(dict,_sel_registerName("setObject:forKey:"),trueBool,NSStringFromUTF8String("platform-application"));
	dict_setObjectForKey(dict,_sel_registerName("setObject:forKey:"),falseBool,NSStringFromUTF8String("com.apple.private.security.container-required"));
	
	void* (*dict_writeToFile_atomically)(void*,char*,void*,int) = (void* (*)(void*,char*,void*,int))_objc_msgSend;
	dict_writeToFile_atomically(dict,_sel_registerName("writeToFile:atomically:"),NSStringFromUTF8String(path),1);
	
	
}

int setPlistBoolValueForKey(int value, const char *key, const char *plistPath){

	void* (*NSDictionary_dictionaryWithContentsOfFile)(void*,char*,void*) = (void* (*)(void*,char*,void*))_objc_msgSend;
	void* dict = NSDictionary_dictionaryWithContentsOfFile(_objc_getClass("NSDictionary"),_sel_registerName("dictionaryWithContentsOfFile:"),NSStringFromUTF8String(plistPath));
	
	void* (*NSNumber_numberWithBool)(void*,char*,int) = (void* (*)(void*,char*,int))_objc_msgSend;
	void* valueBool = NSNumber_numberWithBool(_objc_getClass("NSNumber"),_sel_registerName("numberWithBool:"),value);
	
	void* (*dict_setObject_forKey)(void*,char*,void*,void *) = (void* (*)(void*,char*,void*,void *))_objc_msgSend;
	dict_setObject_forKey(dict,_sel_registerName("setObject:forKey:"),valueBool,NSStringFromUTF8String(key));
	 
	int (*dict_writeToFile_atomically)(void*,char*,void*,int) = (int (*)(void*,char*,void*,int))_objc_msgSend;
	return dict_writeToFile_atomically(dict,_sel_registerName("writeToFile:atomically:"),NSStringFromUTF8String(plistPath),1);
	
}

int validatePlist(const char *plistPath){
									
	void* (*NSDictionary_dictionaryWithContentsOfFile)(void*,char*,void*) = (void* (*)(void*,char*,void*))_objc_msgSend;
	void* dict = NSDictionary_dictionaryWithContentsOfFile(_objc_getClass("NSDictionary"),_sel_registerName("dictionaryWithContentsOfFile:"),NSStringFromUTF8String(plistPath));
	return dict!=NULL;
	
}

char ** getPlistArrayValueForKey(const char *key, const char *plistPath,size_t *count){

 	
	void* (*NSDictionary_dictionaryWithContentsOfFile)(void*,char*,void*) = (void* (*)(void*,char*,void*))_objc_msgSend;
	void* dict = NSDictionary_dictionaryWithContentsOfFile(_objc_getClass("NSDictionary"),_sel_registerName("dictionaryWithContentsOfFile:"),NSStringFromUTF8String(plistPath));
	void* (*dict_valueForKeyPath)(void*,char*,void*) = (void* (*)(void*,char*,void*))_objc_msgSend;
	void* res = dict_valueForKeyPath(dict,_sel_registerName("valueForKeyPath:"),NSStringFromUTF8String(key));
	
	if (res){
		
		size_t (*_array_count)(void*,char*) = (size_t (*)(void*,char*))_objc_msgSend;
		size_t arrCount = _array_count(res,_sel_registerName("count"));
		void* (*array_objectAtIndex)(void*,char*,unsigned) = (void* (*)(void*,char*,unsigned))_objc_msgSend;
	
		*count=arrCount;
	
		if (arrCount){
			char **array=(char **)malloc(8128);
			for (size_t i=0; i<arrCount; i++){
				void *obj = array_objectAtIndex(res,_sel_registerName("objectAtIndex:"),i);
				char *utf8Val=UTF8String(obj);
				array[i]=utf8Val;
			}
			return array;
		}
	}
	//fail if not
	*count=0;
	return NULL;
	
	
}

void removeSubstring(char *s,const char *toremove){
  	
  	while( (s=strstr(s,toremove) )){
    	memmove(s,s+strlen(toremove),1+strlen(s+strlen(toremove)));
	}
	
}
