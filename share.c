#include "secshare.h"

/*Solve a random system of linear equations using the Gauss-Jordan-Function
 *
 *This version computes inside the field (M,+,*) for M being the set 0<=k<p for p being a prime number and modulus of the field.
 *
 *Other than gje_intmod.c it uses interdependent coefficients: each row is formed as k ~~ (m x x^2 x^3 ... x^(n-1)), so that
 *k=m*1 + a*x + b*x^2 ... z*x^(n-1)
 *This can be used to split a secret m among n persons by creating random a,b,c,... and x and giving each person his/her k and x,
 *the algorithm below can then compute m,a,b,c... from n sets of k and x.
 *
 *Other than gje_intmod2.c it uses large integers and can thus handle much larger numbers.
 *
 *written by Konrad Rosenbaum, 2004
 *This code is protected by the GNU GPL v2
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <gmp.h>

/*the matrix and result field*/
mpz_t **kmatrix,**hmatrix,*kvec,*hvec;
/*n=size of the matrix*/
static int n,np;
/*p is the prime and modulo*/
static mpz_t p;


static void setbin(mpz_t n,byte*b,int l)
{
	char buf[1024];
	static const char hex[]="0123456789ABCDEF";
	int i;
	for(i=0;i<l;i++){
		buf[i*2]=hex[b[i]>>4];
		buf[i*2+1]=hex[b[i]&0xf];
	}
	buf[l*2]=0;
	mpz_set_str(n,buf,16);
}

void sharevector(int n_)
{
	int i,j;
	byte hdl[17];
	n=n_;
	/*intialize vector and p*/
	kvec=malloc(n*sizeof(mpz_t));
	hvec=malloc(n*sizeof(mpz_t));
	for(i=0;i<n;i++){
		mpz_init(kvec[i]);
		mpz_init(hvec[i]);
		if(i==0){
			setbin(kvec[0],key,16);
			setbin(hvec[0],hashv,16);
			printf("Setting key: %s\n",key_prints(0,key));
			printf("Setting hash: %s\n",key_prints(0,hashv));
		}else{
			for(j=0;j<16;j++)
				hdl[j]=randbyte();
			setbin(kvec[i],hdl,16);
			for(j=0;j<16;j++)
				hdl[j]=randbyte();
			setbin(hvec[i],hdl,16);
		}
	}
	mpz_init(p);
	/*generate random prime*/
	hdl[0]=1;
	for(i=0;i<500;i++){
		for(j=1;j<17;j++)
			hdl[j]=randbyte();
		hdl[16]|=1;
		setbin(p,hdl,17);
		j=mpz_probab_prime_p(p,100);
		if(j)break;
	}
	if(!j){
		fprintf(stderr,"Unable to generate a good prime. Giving up.\n");
		exit(1);
	}
}

void sharematrix(int n_)
{
	int i,j;
	n=n_;np=0;
	kmatrix=malloc(n*sizeof(mpz_t*));
	hmatrix=malloc(n*sizeof(mpz_t*));
	for(i=0;i<n;i++){
		kmatrix[i]=malloc((n+1)*sizeof(mpz_t));
		hmatrix[i]=malloc((n+1)*sizeof(mpz_t));
		for(j=0;j<=n;j++)mpz_init(kmatrix[i][j]);
		for(j=0;j<=n;j++)mpz_init(hmatrix[i][j]);
	}
	mpz_init(p);
}

void share(char*fn)
{
	int i;
	mpz_t x,k,t,e;
	byte rnd[16];
	char buf[1024];
	FILE *f;

	f=fopen(fn,"w+");
	if(f==NULL){
		fprintf(stderr,"Unable to open file %s.\n",fn);
		return;
	}

	/*generate random x*/
	mpz_init(x);
	for(i=0;i<16;i++)rnd[i]=randbyte();
	setbin(x,rnd,16);

	/*calculate k*/
	mpz_init(k);
	mpz_init(t);
	mpz_init(e);
	for(i=0;i<n;i++){
		/*k=k + kvec[i]*x^i*/
		mpz_powm(t,x,e,p);
		mpz_mul(t,t,kvec[i]);
		mpz_add(k,k,t);
		mpz_mod(k,k,p);
		/*incr e so that it matches the next i*/
		mpz_add_ui(e,e,1);
	}

	fprintf(f,"%s\n",mpz_get_str(buf,16,p));
	fprintf(f,"%s\n",mpz_get_str(buf,16,x));
	fprintf(f,"%s\n",mpz_get_str(buf,16,k));

	mpz_set_ui(e,0);
	mpz_set_ui(k,0);
	for(i=0;i<n;i++){
		/*k=k + hvec[i]*x^i*/
		mpz_powm(t,x,e,p);
		mpz_mul(t,t,hvec[i]);
		mpz_add(k,k,t);
		mpz_mod(k,k,p);
		/*incr e so that it matches the next i*/
		mpz_add_ui(e,e,1);
	}

	fprintf(f,"%s\n",mpz_get_str(buf,16,k));

	fclose(f);

	mpz_clear(x);
	mpz_clear(k);
	mpz_clear(t);
	mpz_clear(e);
}

void unshare(char*fn)
{
	FILE *f;
	char buf[1024];
	mpz_t x,kk,kh,e;
	int i;

	if(np>=n)return;

	/*read file, fill values*/
	f=fopen(fn,"r");
	if(f==NULL){
		fprintf(stderr,"Unable to read file %s. Continuing.\n",fn);
		return;
	}
	mpz_init(x);
	mpz_init(kk);
	mpz_init(kh);
	mpz_init(e);
	fscanf(f,"%s",buf);
	mpz_set_str(p,buf,16);
	fscanf(f,"%s",buf);
	mpz_set_str(x,buf,16);
	fscanf(f,"%s",buf);
	mpz_set_str(kk,buf,16);
	fscanf(f,"%s",buf);
	mpz_set_str(kh,buf,16);
	fclose(f);

	/*generate matrixes*/
	for(i=0;i<n;i++){
		mpz_powm(kmatrix[np][i],x,e,p);
		mpz_powm(hmatrix[np][i],x,e,p);
		mpz_add_ui(e,e,1);
	}
	mpz_set(kmatrix[np][n],kk);
	mpz_set(hmatrix[np][n],kh);

	/*next line...*/
	np++;
	mpz_clear(x);
	mpz_clear(kk);
	mpz_clear(kh);
	mpz_clear(e);
}

/*print the matrix*/
void printm(mpz_t**matrix)
{
	int i,j;
	char buf[1024];
	for(i=0;i<n;i++){
		for(j=0;j<=n;j++)
			printf(" %s",mpz_get_str(buf,16,matrix[i][j]));
		printf("\n");
	}
	printf("\n");
}

static void solve(mpz_t**matrix)
{
	int i,j,k;
	mpz_t f,*t,t1;

	mpz_init(f);
	mpz_init(t1);

	/*bring matrix into Row Echelon Form*/
	/*printf("Calculating.\n");*/
	for(i=0;i<n;i++){/*iterate main diagonal*/
		j=1;
		while(!mpz_cmp_ui(matrix[i][i],0)){
			/*exchange rows if working position is zero*/
			if(i+j>=n){
				fprintf(stderr,"System is unsolvable!\n");
				exit(1);
			}
			t=matrix[i];
			matrix[i]=matrix[i+j];
			matrix[i+j]=t;
			j++;
		}
		/*divide row by working position*/
		mpz_invert(f,matrix[i][i],p);/*f=1/m[i,i]*/
		for(j=0;j<=n;j++){
			/*m[i,j]=m[i,j]/m[i,i] mod p*/
			mpz_mul(matrix[i][j],matrix[i][j],f);
			mpz_mod(matrix[i][j],matrix[i][j],p);
		}
		/*go through rows beneath*/
		for(j=i+1;j<n;j++){
			/*store value of current column next row*/
			mpz_set(f,matrix[j][i]);
			/*multiply with current row*/
			/*subtract from processed row, so that current column in that row is forced to zero*/
			for(k=0;k<=n;k++){
				mpz_mul(t1,matrix[i][k],f);
				mpz_sub(matrix[j][k],matrix[j][k],t1);
				mpz_mod(matrix[j][k],matrix[j][k],p);
			}
		}
	}

	/*printf("Row Echelon form.\n");*/

	/*bring matrix into Reduced Row Echelon Form*/
	for(i=n-1;i>=0;i--){/*iterate main diagonal*/
		j=1;
		while(!mpz_cmp_ui(matrix[i][i],0)){
			/*exchange rows if working position is zero*/
			if(i-j<0){
				fprintf(stderr,"System is unsolvable!\n");
				exit(1);
			}
			t=matrix[i];
			matrix[i]=matrix[i+j];
			matrix[i+j]=t;
			j++;
		}
		/*divide row by working position*/
		mpz_invert(f,matrix[i][i],p);/*f=1/m[i,i]*/
		for(j=0;j<=n;j++){
			/*m[i,j]=m[i,j]/m[i,i] mod p*/
			mpz_mul(matrix[i][j],matrix[i][j],f);
			mpz_mod(matrix[i][j],matrix[i][j],p);
		}
		/*go through rows above*/
		for(j=i-1;j>=0;j--){
			/*store value of current column next row*/
			mpz_set(f,matrix[j][i]);
			/*multiply with current row*/
			/*subtract from processed row, so that current column in that row is forced to zero*/
			for(k=0;k<=n;k++){
				mpz_mul(t1,matrix[i][k],f);
				mpz_sub(matrix[j][k],matrix[j][k],t1);
				mpz_mod(matrix[j][k],matrix[j][k],p);
			}
		}
	}

	/*printf("Reduced Row Echelon form.\n");*/
}

static int unhex(char c)
{
	if(c>='0'&&c<='9')return c-'0';
	if(c>='a'&&c<='f')return c-'a'+10;
	if(c>='A'&&c<='F')return c-'A'+10;
	return 0;
}

static void str2bin(byte*tgt,const char*str,int len)
{
	int i;
	char s[1024];
	if(strlen(str)<(len*2)){
		for(i=0;i<len*2;i++)s[i]='0';
		strcpy(s+(len*2-strlen(str)),str);
	}else strcpy(s,str);
	for(i=0;i<len;i++){
		tgt[i]=(unhex(s[i*2])<<4) | unhex(s[i*2+1]);
	}
}

void sharecalc()
{
	char buf[1024];

	if(np<n){
		fprintf(stderr,"Could not read enough shares, unable to compute.\n");
		exit(1);
	}

	/*calculate*/

	/*printf("Key-Matrix:\n");printm(kmatrix);
	printf("Hash-Matrix:\n");printm(hmatrix);*/

	solve(kmatrix);
	solve(hmatrix);

	/*output result*/
	str2bin(key,mpz_get_str(buf,16,kmatrix[0][n]),16);
	str2bin(hashv,mpz_get_str(buf,16,hmatrix[0][n]),16);
	/*printf("Using key: %s\n",key_prints(0,key));
	printf("Expected Hash: %s\n",key_prints(0,hashv));*/
	/*printf("Key-Matrix:\n");printm(kmatrix);
	printf("Hash-Matrix:\n");printm(hmatrix);*/
}