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

#define READ 1
#define WRITE 2
#define MYNUM 0x88888888
#define MYSTR1 "3-APP"
#define MYSTR2 "5-APP"
#define MYSTR3 "6-APP"


main() {
	int fd, len, wlen;
	char str[127];
	int num, rnum, i;
	unsigned long inum = MYNUM;
	unsigned long onum;

	strcpy(str, MYSTR1);

	if ((fd = open("/dev/llddrvr", O_RDWR | O_APPEND)) == -1) {
		fprintf(stderr, "ERR:on open():%s\n", strerror(errno));
		exit(0);
	}

	strcpy(str, MYSTR1);
	wlen = strlen(str);
	if ((len = write(fd, str, wlen)) == -1) {
		fprintf(stderr, "ERR:on write():%s\n", strerror(errno));
		exit(1);
	}
	fprintf(stdout, "Write: \"%s\", len %i, wlen %i, \n", str,len, wlen);

	strcpy(str, MYSTR2);
	wlen = strlen(str);
	if ((len = write(fd, str, wlen)) == -1) {
		fprintf(stderr, "ERR:on write():%s\n", strerror(errno));
		exit(1);
	}
	fprintf(stdout, "Write: \"%s\", len %i, wlen %i, \n", str,len, wlen);


	/*
	strcpy(str, MYSTR3);
	wlen = strlen(str);
	if ((len = write(fd, str, wlen)) == -1) {
		fprintf(stderr, "ERR:on write():%s\n", strerror(errno));
		exit(1);
	}
	fprintf(stdout, "Write: %s, len %i, wlen %i, \n", str,len, wlen);
*/








	i = 0;
	while (i++ < 127)
		str[i] = 0;
	// read
	if ((len = read(fd, str, 1)) == -1) {
		fprintf(stderr, "3. ERR:on read():%s\n", strerror(errno));
		exit(1);
	}
	fprintf(stdout, "Read: \"%s\", len: %i\n", str,i);

	i = 0;
	while (i++ < 127)
		str[i] = 0;

	lseek(fd,1,0);

	if ((len = read(fd, str, 1)) == -1) {
		fprintf(stderr, "3. ERR:on read():%s\n", strerror(errno));
		exit(1);
	}
	fprintf(stdout, "Read: \"%s\", len: %i\n", str,i);


	i = 0;
	while (i++ < 127)
		str[i] = 0;

	lseek(fd,1,0);

	if ((len = read(fd, str, 1)) == -1) {
		fprintf(stderr, "3. ERR:on read():%s\n", strerror(errno));
		exit(1);
	}
	fprintf(stdout, "Read: \"%s\", len: %i\n", str,i);


	i = 0;
	while (i++ < 127)
		str[i] = 0;

	lseek(fd,2,0);

	if ((len = read(fd, str, 9999)) == -1) {
		fprintf(stderr, "3. ERR:on read():%s\n", strerror(errno));
		exit(1);
	}
	fprintf(stdout, "Read: \"%s\", len: %i\n", str,i);



	// write using ioctl()
	if ((len = ioctl(fd, WRITE, 0)) == -1) {
		fprintf(stderr,"ERR:on ioctl-write():%s\n",strerror(errno));

	}
	fprintf(stdout, "Ioctl: len: %i\n", len);


	/*
	// read using ioctl()
	if ((len = ioctl(fd, READ, &onum)) == -1) {
		fprintf(stderr,"ERR:on ioctl-read():%s\n",strerror(errno));
		exit(1);
	}
	fprintf(stdout, "ioctl read .. %#0x(%d).  len=%d\n", onum,onum,len);
*/



	close(fd);
}
