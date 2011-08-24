#include <sys/types.h>  /* for byte and u32 typedefs */
#include <stdio.h>

int main()
{
	union {
		u_int16_t i;
		u_int8_t c[2];
	} u;
	FILE *f;

	u.i=0xAFFE;

	f=fopen("endianess.h","w+");

	if(u.c[0]==0xAF){
		fprintf(f,"#define WORDS_BIGENDIAN 1\n");
	}else{
		fprintf(f,"#define WORDS_LITTLEENDIAN 1\n");
	}
	fclose(f);
	return 0;
}