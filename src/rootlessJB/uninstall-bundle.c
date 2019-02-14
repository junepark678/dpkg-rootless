#include <dlfcn.h>
#include <spawn.h>
#include <sys/wait.h>
#include <libgen.h>
#include <limits.h>


static const char *packageToRemove=NULL;
char foundApps[32][256];
int foundAppsCount=0;

void addFoundApp(const char *app);
void findAppInContainers(const char *app);
void removeAppContainedByDebPackage(const char *inPackage);

#include "objc-helper.h"


void addFoundApp(const char *app){
	if (foundAppsCount<100){
		for (int i=0; i<foundAppsCount; i++){
			if (!strcmp(app,foundApps[i])){ //already found
				return;
			}
		}
		strcpy((char *)foundApps[foundAppsCount],app);
	}
	foundAppsCount++;
}


void findAppInContainers(const char *app){

	DIR *_dir;
	struct dirent *_ent;
	
	if ((_dir = opendir ("/var/containers/Bundle/Application")) != NULL) {

		while ((_ent = readdir(_dir)) != NULL) {

			if ((_ent->d_type & DT_DIR) && strcmp(_ent->d_name,".") && strcmp(_ent->d_name,"..")){

				char appPath[4096];
				sprintf(appPath,"/var/containers/Bundle/Application/%s/%s",_ent->d_name,app);
				struct stat dirstat;

				if (!stat(appPath,&dirstat)){

					char jbFlagPath[PATH_MAX];
					sprintf(jbFlagPath,"%s/.jb",appPath);
					char infoPlist[PATH_MAX];
					sprintf(infoPlist,"%s/Info.plist",appPath);
						
					struct stat jbStat;
					int jbExists=!stat(jbFlagPath,&jbStat); //flag file .jb exists in directory (set by my modified dpkg on app install)
					
					char *bundleID=getPlistValueForKey("CFBundleIdentifier",infoPlist);
				 	if (jbExists || !strcmp(bundleID,packageToRemove)){
						
						if (strstr(bundleID,"\n")){
							removeSubstring(bundleID,"\n");
						}
						if (strstr(bundleID,"\r")){
							removeSubstring(bundleID,"\r");
						}
						while(strstr(bundleID," ")){
							bundleID++;
						}
						if (strstr(bundleID,",")){
							removeSubstring(bundleID,",");
						}
						
					
						void* (*NSDictionary_dictionaryWithObject_forKey_)(void*,char*,void*,void*)=(void* (*)(void*,char*,void*,void*))_objc_msgSend;
						void* theDict=NSDictionary_dictionaryWithObject_forKey_(_objc_getClass("NSDictionary"),_sel_registerName("dictionaryWithObject:forKey:"),NSStringFromUTF8String(bundleID),NSStringFromUTF8String("CFBundleIdentifier"));
						void* (*LSApplicationWorkspace_defaultWorkspace)(void*,char*)=(void* (*)(void*,char*))_objc_msgSend;
						void* LSAppWorkspace = LSApplicationWorkspace_defaultWorkspace(_objc_getClass("LSApplicationWorkspace"),_sel_registerName("defaultWorkspace"));
						int (*LSAppWorkspace_uninstallApplication_withOptions)(void*,char*,void*,void*)=(int (*)(void*,char*,void*,void*))_objc_msgSend;
						printf("[*] Uninstalling app %s\n",bundleID);
						LSAppWorkspace_uninstallApplication_withOptions(LSAppWorkspace,_sel_registerName("uninstallApplication:withOptions:"),NSStringFromUTF8String(bundleID),theDict);
						
						// because who needs "-x objective-c" and linking libobjc, in a world filled of those? ;-D
						
						
						//free(bundleID);				 
					}
				}
				
			}
		}
		closedir(_dir); 
	}
}



void removeAppContainedByDebPackage(const char *inPackage){
	
	addObjcSymbols();

	packageToRemove=inPackage;
	const char *dpkg="/var/bin/dpkg";
	
	posix_spawn_file_actions_t value_actions;
	posix_spawn_file_actions_init (&value_actions);
	posix_spawn_file_actions_addopen (&value_actions, 1, "/tmp/_RMPACKAGE", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	posix_spawn_file_actions_adddup2 (&value_actions, 1, 2);
	int ret;
	pid_t apid;
	extern char** environ;
	ret=posix_spawn(&apid, dpkg, &value_actions, NULL, (char **)(const char *[]){dpkg, "-L", packageToRemove, NULL}, environ);
	waitpid(apid,&ret,0);

	FILE *p=fopen("/tmp/_RMPACKAGE","r");
	char string[256];
	char *line;
	while ((line=fgets(string,sizeof(string),p))){
		if (line!=string){
			break;
		}
	 	if (strstr(line,".app/")){
			if (dirname(line)){
				if (basename(dirname(line))){
					addFoundApp(basename(dirname(line)));
					break;
				}
			}

		}
	}
	for (int i=0; i<foundAppsCount; i++){
	 	findAppInContainers(foundApps[i]);
	}

}