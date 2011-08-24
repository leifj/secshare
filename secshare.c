#include "secshare.h"
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int fdin;
int fdout;
int verbose=1;

int main(int argc,char**argv)
{
	int amt=0,doenc,i;
	char buf[1024];
	if(argc<3){
		fprintf(stderr,"Usage: %s amount filename secrets...\n"
			"\tamount=amount of necessary secrets, negative for decryption\n"
			"\tfilename=name of the file to be en-/de-crypted\n"
			"\tsecrets=names of files with secrets\n"
			"\t\tfor encryption it will create as many secrets,\n"
			"\t\tfor decryption it will read the first $amount\n",*argv);
		return 1;
	}
	sscanf(argv[1],"%i",&amt);
	if(amt>-2 && amt<2){
		fprintf(stderr,"A secret must be shared between at least two persons.\n");
		return 1;
	}
	if(amt<0){
		doenc=0;
		amt=-amt;
	}else{
		doenc=1;
	}
	if(argc<(3+amt)){
		fprintf(stderr,"Not enough secrets specified.\n");
		return 1;
	}
	if(doenc){/*encrypt*/
		fdin=open(argv[2],O_RDONLY);
		if(fdin<0){
			fprintf(stderr,"Unable to open file %s.\n",argv[2]);
			return 1;
		}
		fdout=open(strcat(strcpy(buf,argv[2]),".enc"),O_RDWR|O_CREAT|O_TRUNC,0666);
		if(fdout<0){
			fprintf(stderr,"Unable to create file %s.\n",buf);
			return 1;
		}
		encrypt();
		sharevector(amt);
		for(i=3;i<argc;i++)share(argv[i]);
	}else{/*decrypt*/
		sharematrix(amt);
		for(i=3;i<argc;i++)unshare(argv[i]);
		sharecalc();
		fdin=open(argv[2],O_RDONLY);
		if(fdin<0){
			fprintf(stderr,"Unable to open file %s.\n",argv[2]);
			return 1;
		}
		fdout=open(strcat(strcpy(buf,argv[2]),".dec"),O_RDWR|O_CREAT|O_TRUNC,0666);
		if(fdout<0){
			fprintf(stderr,"Unable to create file %s.\n",buf);
			return 1;
		}
		decrypt();
	}
	return 0;
}