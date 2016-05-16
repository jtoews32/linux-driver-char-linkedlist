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


main() {
	int fd, len, wlen;
	char str[128];
	int num, rnum, i;
	unsigned long inum = MYNUM;
	unsigned long onum;
	/*
	strcpy(str, MYSTR);


	if((fd = open("/dev/llddrvr", O_RDWR | O_APPEND)) == -1) {
		fprintf(stderr,"ERR:on open():%s\n",strerror(errno));
		exit(0);
	}


	wlen = strlen(str);
	if ((len = write(fd, str, wlen)) == -1) {
		fprintf(stderr,"ERR:on write():%s\n",strerror(errno));
		exit(1);
	}
	fprintf(stdout, "Write: \"%s\"\n", str);


	wlen = strlen(str);
	if ((len = write(fd, str, wlen)) == -1) {
		fprintf(stderr,"ERR:on write():%s\n",strerror(errno));
		exit(1);
	}
	fprintf(stdout, "Write: \"%s\"\n", str);


	if ((len = ioctl(fd, 4, (int *) 0)) == -1) {
		fprintf(stderr,"COMPLETE:\"%s\"\n",strerror(errno));

	}
	fprintf(stdout, "Completion no longer blocked %i\n", len);
*/


	close(fd);
}
