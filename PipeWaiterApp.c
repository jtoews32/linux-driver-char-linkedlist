/*  CDDapp.c */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include "llddrvr.h"
// /*    */
// IOCTL commands ...

//


#define MYNUM 9999999
#define MYSTR "Hello worldliness"

main() {
	int fd, len, wlen;
	char str[128];


	strcpy(str, MYSTR);

	// open
	if((fd = open("/dev/llddrvr", O_RDWR | O_APPEND)) == -1) {
		fprintf(stderr,"ERR:on open():%s\n",strerror(errno));
		exit(0);
	}


	// write
	wlen = strlen(str);
	if ((len = write(fd, str, wlen)) == -1) {
		fprintf(stderr,"ERR:on write():%s\n",strerror(errno));
		exit(1);
	}
/*
	// read
	if ((len = read(fd, str, sizeof(str))) == -1) {
		fprintf(stderr,"ERR:on read():%s\n",strerror(errno));
		exit(1);
	}
	fprintf(stdout, "%s\n", str);
*/

	unsigned long inum = 191919;
	unsigned long onum;

	// write using ioctl()


	if ((len = ioctl(fd, 1, (int *)inum)) == -1) {
		fprintf(stderr,"ERR:on %i ioctl-write():%s\n",len,strerror(errno));
		//exit(1);
	}
	fprintf(stdout, "ioctl wrote.   len=%d\n", inum);


	// read using ioctl()
	if ((len = ioctl(fd, 2, &onum)) == -1) {
		fprintf(stderr,"ERR:on ioctl-read():%s\n",strerror(errno));
	//	exit(1);
	}
	fprintf(stdout, "ioctl read .. %#0x(%d).  len=%d, %i, %i\n", onum,onum,len,len,onum);


	close(fd);
}

