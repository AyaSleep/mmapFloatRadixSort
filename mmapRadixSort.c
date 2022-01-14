#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <math.h>
void nSort(float *map, long n, long v){
	long zeroCount=0;
	long oneCount=0;
	float* bucket0=malloc(n*sizeof(float));
	float* bucket1=malloc(n*sizeof(float));
	unsigned int b;
	union ufi{
		float f;
		int i;
	} u;
	int z;
	for(z=0;z<n;z++){
		u.f=map[z];
		b=u.i&v;
		if(b==0){
			bucket0[zeroCount++]=map[z];
		} else {
			bucket1[oneCount++]=map[z];
		}
	}
	memcpy(map,bucket1,oneCount*sizeof(float));
	for(z=0;z<zeroCount;z++){
		map[z+oneCount]=bucket0[z];
	}
	free(bucket0);
	free(bucket1);
}
void pSort(float *map, long n, long v){
	long zeroCount=0;
	long oneCount=0;
	float* bucket0=malloc(n*sizeof(float));
	float* bucket1=malloc(n*sizeof(float));
	unsigned int b;
	union ufi {
    		float f;
    		int i;
	} u;
	long z;
	for(z=0;z<n;z++){
		u.f=map[z];
		b=u.i&v;
		if(b==0){
			bucket0[zeroCount++]=map[z];	
		} else {
			bucket1[oneCount++]=map[z];
		}
	}
	memcpy(map,bucket0,zeroCount*sizeof(float));
	for(z=0;z<oneCount;z++){
		map[zeroCount+z]=bucket1[z];
	}
	free(bucket0);
	free(bucket1);
}
void radix(float *map, long n){
	long q;
	long negativeCount=0;
	for(q=0;q<n;q++){
		if(map[q]<0.0){
			negativeCount++;
		}
	}
	long nPosition=0;
	long pPosition=0;
	float* negatives=malloc(negativeCount*sizeof(float));
	float* positives=malloc((n-negativeCount)*sizeof(float));
	for(q=0;q<n;q++){
		if(map[q]<0.0){
			negatives[nPosition++]=map[q];
		} else {
			positives[pPosition++]=map[q];
		}
	}
	int v=0;
	while(v<31){
		nSort(negatives,negativeCount,pow(2,v));
		pSort(positives,n-negativeCount,pow(2,v));
		v++;
	}
	memcpy(map,negatives,negativeCount*sizeof(float));
	for(q=0;q<n-negativeCount;q++){
		map[q+negativeCount]=positives[q];
	}
	free(negatives);
	free(positives);
}
int main(int argc, char * argv[]){
	int fd;
	fd=open(argv[1],O_RDWR);
	if(fd==-1){
		perror("Error opening file for writing");
		exit(EXIT_FAILURE);
    	}
	struct stat sb;
	if(fstat(fd,&sb)==-1){
		close(fd);
		perror("issues with ur fstat homie");
		exit(EXIT_FAILURE);
	}
	
	float *map;
	map=mmap(0, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    	if(map == MAP_FAILED){
		close(fd);
		perror("Error mmapping the file");
		exit(EXIT_FAILURE);
    	}
	long n=sb.st_size/sizeof(float);
	radix(map,n);
	if(munmap(map, sb.st_size)==-1){
		perror("Error un-mmapping the file");
    	}
    	close(fd);
	return 1;
}
