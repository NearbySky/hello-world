/*
 ============================================================================
 Name        : mygetcwd.c
 Author      : liuht
 Version     :
 Copyright   : Your copyright notice
 Description : getcwd()
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdbool.h>

struct strconcat {
	int maxsize;
	int nchar;
	char split;
	char *catstr;
};

int init_strcat(struct strconcat * strptr);
int do_strcat(struct strconcat *strptr,char *from,int len);
char * mygetcwd(char *buf,size_t size);
void destroy_strcat(struct strconcat *strptr);

int main(int argc,char **argv) {
	struct strconcat scc;
	int i=1,ret;
	char buf[4096];
	memset(buf,0,4096);

	if(init_strcat(&scc) < 0){
		exit(-1);
	}
	while(i<argc){
		ret = do_strcat(&scc,argv[i],strlen(argv[i]));
		i++;
	}
	if(ret >= 0){
		printf("%s\n",&scc.catstr[ret]);
	}
	mygetcwd(buf,4096);
	printf("cwd is : %s\n",buf);
	destroy_strcat(&scc);

	return EXIT_SUCCESS;
}

int init_strcat(struct strconcat * strptr){
	strptr->maxsize = PATH_MAX;
	strptr->nchar = 0;
	strptr->split = '/';
	if((strptr->catstr = malloc(strptr->maxsize+1)) == NULL){
		printf("malloc error\n");
		return-1;
	}
	strptr->catstr[strptr->maxsize] = 0;
	return 0;
}

void destroy_strcat(struct strconcat *strptr){
	free(strptr->catstr);
}

int do_strcat(struct strconcat *strptr,char *from,int len){
	int ind;
	int cur = strptr->maxsize - strptr->nchar;

	if((strptr->maxsize - strptr->nchar) < len){
		return -1;
	}
	if((strptr->nchar > 0) && (strcmp(from,"/") != 0)){
		strptr->catstr[--cur] = strptr->split;
		strptr->nchar++;
	}

	ind = 0;
	while(ind < len){
		strptr->catstr[cur - len + ind] = from[ind];
		ind++;
	}

	strptr->nchar += len;
	return (cur - len);
}

char * mygetcwd(char *buf,size_t size){
	struct strconcat scc;
	struct stat stbuf;
	DIR *dp;
	struct dirent *dirp;
	char dotpath[32*3],dot2path[32*3];
	bool isRoot = false;
	int retpos;

	memset(dotpath,0 ,sizeof(dotpath));
	memset(dot2path,0, sizeof(dot2path));
	init_strcat(&scc);

	strcpy(dotpath,".");
	strcpy(dot2path,"..");

	while(isRoot != true){
		//strcpy(pathtmp,path);

		if(stat(dotpath,&stbuf) < 0 ){//get the inode number of current dir
			exit(-1);
		}
		if((dp = opendir(dot2path)) == NULL){//open parent dir of current dir
			exit(-1);
		}
		while((dirp = readdir(dp)) != NULL){
			if(dirp->d_ino == stbuf.st_ino){//find the name of current dir
				if(strcmp(dirp->d_name,".") == 0){
					retpos = do_strcat(&scc,"/",1);
					isRoot = true;
					break;
				}
				do_strcat(&scc,dirp->d_name,strlen(dirp->d_name));
				//strcpy(path,"/");
				//strcat(path,dirp->d_name);
				//strcat(path,pathtmp);
				break;
			}
		}
		strcat(dotpath,"./.");//like "../../."
		strcat(dot2path,"/..");//like "../../.."
	}
	strcpy(buf,&scc.catstr[retpos]);
	return buf;
}
