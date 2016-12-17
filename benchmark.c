#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <assert.h>

#define TRUE 1
#define FALSE 0

#define TIMER_START() assert(clock_gettime( CLOCK_REALTIME, &ts_start)==0)
#define TIMER_END() assert(clock_gettime( CLOCK_REALTIME, &ts_end)==0)
#define TIMER_ELAPSE_SEC() (ts_end.tv_sec - ts_start.tv_sec)
#define TIMER_ELAPSE_NSEC() (ts_end.tv_nsec - ts_start.tv_nsec)

struct timespec ts_start;
struct timespec ts_end;

long int sec, nsec;
char * log_filepath;

#define BUF_SIZE 1024*1024 // 1mb
#define STR_BUF_SIZE 1024

void rw_writefile_benchmark(const char * filepath, int block_size, int count, char isFsyncOn){
	char filepath_r[1024] = "";
	int fd;
	int ret_val, str_len;
	char buf[BUF_SIZE];
	char str_buf[STR_BUF_SIZE];

	long int* elapse_time_ary[2]; // To save sec dif and nsec dif

	elapse_time_ary[0] = (long int*)malloc(sizeof(long int)*count);
	elapse_time_ary[1] = (long int*)malloc(sizeof(long int)*count);

	for(int i=0;i<BUF_SIZE;i++){
		buf[i] = i % 2;
	}

	for(int i=0; i<count; i++){
		sprintf(filepath_r, "%s_%d", filepath, i);

		fd = open(filepath_r, O_RDWR | O_CREAT | O_TRUNC, 0666);
		if (fd < 0) {
			perror("Failed to open file\n");
			exit(1);
		}

		TIMER_START();

		ret_val = write(fd, buf, block_size);
		if(ret_val != block_size){
			printf("failed to write a whole block\n");
			exit(1);
		}

		if(isFsyncOn == TRUE){
		    fsync(fd);
		}

		TIMER_END();

		nsec = TIMER_ELAPSE_NSEC();
		sec = TIMER_ELAPSE_SEC();

		if (nsec < 0) {
			nsec = 1000000000 + nsec;
			sec--;
		}

		elapse_time_ary[0][i] = sec;
		elapse_time_ary[1][i] = nsec;

		close(fd);
	}
	
	fd = open(log_filepath, O_RDWR | O_CREAT | O_TRUNC, 0666);
	if (fd < 0) {
		perror("Failed to make a log file\n");
		exit(1);
	}

	str_len = sprintf(str_buf, "%d bytes write() time\n", block_size);
	ret_val = write(fd, str_buf, str_len);
	if(ret_val != str_len){
		perror("Failed to write log file\n");
		exit(1);
	}
	
	for(int i=0;i<count;i++){
		str_len = sprintf(str_buf, "%ld.%09ld\n", elapse_time_ary[0][i], elapse_time_ary[1][i]);
		ret_val = write(fd, str_buf, str_len);
		if(ret_val != str_len){
			perror("Failed to write log file\n");
			exit(1);
		}
	}
	
	close(fd);

	free(elapse_time_ary[0]);
	free(elapse_time_ary[1]);
}
void rw_readfile_benchmark(const char * filepath, int block_size, int count){
	char filepath_r[1024] = "";
	int fd;
	int ret_val, str_len;
	char buf[BUF_SIZE];
	char str_buf[STR_BUF_SIZE];

	long int* first_elapse_time_ary[2]; // To save sec dif and nsec dif
	long int* second_elapse_time_ary[2]; // To save sec dif and nsec dif

	first_elapse_time_ary[0] = (long int*)malloc(sizeof(long int)*count);
	first_elapse_time_ary[1] = (long int*)malloc(sizeof(long int)*count);

	second_elapse_time_ary[0] = (long int*)malloc(sizeof(long int)*count);
	second_elapse_time_ary[1] = (long int*)malloc(sizeof(long int)*count);

	for(int i=0;i<BUF_SIZE;i++){
		buf[i] = i % 2;
	}

	for(int i=0; i<count; i++){
		sprintf(filepath_r, "%s_%d", filepath, i);

		fd = open(filepath_r, O_RDONLY, 0);
		if (fd < 0) {
			perror("Failed to open file\n");
			exit(1);
		}

		// first read
		TIMER_START();

		ret_val = read(fd, buf, block_size);
		if(ret_val != block_size){
			printf("failed to read a whole block %d/%d\n", ret_val, block_size);
			exit(1);
		}

		TIMER_END();

		nsec = TIMER_ELAPSE_NSEC();
		sec = TIMER_ELAPSE_SEC();

		if (nsec < 0) {
			nsec = 1000000000 + nsec;
			sec--;
		}

		first_elapse_time_ary[0][i] = sec;
		first_elapse_time_ary[1][i] = nsec;

		//rewind file pointer
		ret_val = lseek(fd, 0, SEEK_SET);
		if(ret_val <0){
			perror("failed to lseek");
			exit(1);
		}

		// second read
		TIMER_START();

		ret_val = read(fd, buf, block_size);
		if(ret_val != block_size){
			printf("failed to read a whole block %d/%d\n", ret_val, block_size);
			exit(1);
		}

		TIMER_END();

		nsec = TIMER_ELAPSE_NSEC();
		sec = TIMER_ELAPSE_SEC();

		if (nsec < 0) {
			nsec = 1000000000 + nsec;
			sec--;
		}

		second_elapse_time_ary[0][i] = sec;
		second_elapse_time_ary[1][i] = nsec;

		close(fd);
	}
	fd = open(log_filepath, O_RDWR | O_CREAT | O_TRUNC, 0666);
	if (fd < 0) {
		perror("Failed to make a log file\n");
		exit(1);
	}

	str_len = sprintf(str_buf, "%d bytes read() time (not cached)\n", block_size);
	ret_val = write(fd, str_buf, str_len);
	if(ret_val != str_len){
		perror("Failed to write log file\n");
		exit(1);
	}
	
	for(int i=0;i<count;i++){
		str_len = sprintf(str_buf, "%ld.%09ld\n", first_elapse_time_ary[0][i], first_elapse_time_ary[1][i]);
		ret_val = write(fd, str_buf, str_len);
		if(ret_val != str_len){
			perror("Failed to write log file\n");
			exit(1);
		}
	}

	str_len = sprintf(str_buf, "%d bytes write() time (cached)\n", block_size);
	ret_val = write(fd, str_buf, str_len);
	if(ret_val != str_len){
		perror("Failed to write log file\n");
		exit(1);
	}
	
	for(int i=0;i<count;i++){
		str_len = sprintf(str_buf, "%ld.%09ld\n", second_elapse_time_ary[0][i], second_elapse_time_ary[1][i]);
		ret_val = write(fd, str_buf, str_len);
		if(ret_val != str_len){
			perror("Failed to write log file\n");
			exit(1);
		}
	}

	close(fd);

	free(first_elapse_time_ary[0]);
	free(first_elapse_time_ary[1]);

	free(second_elapse_time_ary[0]);
	free(second_elapse_time_ary[1]);
}

void rw_remove(const char * filepath, int count){
	char filepath_r[1024] = "";

	for(int i=0; i<count; i++){
		sprintf(filepath_r, "%s_%d", filepath, i);

		unlink(filepath_r);
	}
}

#define FILENAME "test"

void main(int argc, char ** argv){
	int repeat;
	int block_size;
	char isFsyncOn;

	if(argc != 6){
		printf("Usage : ./bm <write|read> <block_size> <repeat_count> <log_filename> <on|off(fsync)>\n");
		exit(1);
	}

	block_size = atoi(argv[2]);
	repeat = atoi(argv[3]);
	log_filepath = argv[4];
	if(strcmp(argv[5], "on") ==0){
	    isFsyncOn = TRUE;
	} else if (strcmp(argv[5], "off") ==0){
	    isFsyncOn = FALSE;
	} else {
		printf("Usage : ./bm <write|read> <block_size> <repeat_count> <log_filename> <on|off(fsync)>\n");
		exit(1);
	}

	if(strcmp(argv[1], "write") == 0){
		rw_writefile_benchmark(FILENAME, block_size, repeat, isFsyncOn);
	} else if(strcmp(argv[1], "read") == 0){
		rw_readfile_benchmark(FILENAME, block_size, repeat);
	} else if(strcmp(argv[1], "remove") == 0){
		rw_remove(FILENAME, repeat);
	} else {
		printf("Usage : ./bm <write|read> <block_size> <repeat_count> <log_filename> <on|off<fsync)>\n");
		exit(1);
	}
}
