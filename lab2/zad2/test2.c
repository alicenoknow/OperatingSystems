#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>
#include <string.h>
#include <errno.h>

// TASK 2: print only lines with given character


// reads one line from file, returns number of read bytes/chars
// if file contains given character, the line is printed
int sys_print_line(int fd, char sign) {
    char* line = (char*) calloc(256, sizeof(char));
    char ch;
    size_t read_bytes = 256;
	int i = 0;
	int contains = 0;

    while(i < read_bytes){
        while(i < read_bytes && read(fd, &ch, 1) == 1){
            line[i++] = ch;
            if (ch == '\n')
				break;
			if(ch == sign)
				contains = 1;
        }
		
		if(i == read_bytes){
			line = realloc(line, read_bytes*2);
			read_bytes *= 2;
		}
		else
			break;
	
    }
    if(contains)
		printf("%s", line);
    free(line);
    return i;
 
}

// solution with system functions
void sys_print_lines_with_char(char sign, char* filename){
	
	if( access( filename, F_OK ) != 0 ){
		perror("File does not exists!\n");
		exit(0);
	}
	
	int fd = open(filename, O_RDONLY);
    int read_bytes = sys_print_line(fd, sign);

    while(read_bytes != 0 ){	
		if(read_bytes != 0)
			read_bytes = sys_print_line(fd, sign);
	}
    
    close(fd);

}

// solution with library functions
void lib_print_lines_with_char(char sign, char* filename){
	size_t len = 0;
	char* line = calloc(256, sizeof(char));
	int contains = 0;
	
	if( access( filename, F_OK ) != 0  ){
		perror("File does not exists!\n");
		exit(0);
	}
	
	FILE* fd = fopen(filename, "r");

	while (fd != NULL ) {
		contains = 0;
		if(getline(&line, &len, fd) == -1)
			break;
		for(int i = 0; i < strlen(line); i++)
			if(line[i] == sign)
				contains = 1;
		if(contains)
			printf("%s", line);
	}
	
	if(line)
		free(line);
	
	fclose(fd);
}


int main(int argc, char** argv){
	
	if(argc != 3){
		perror("Invalid number of arguments!\n");
		exit(0);
	}
	
	char sign = argv[1][0];
	char* filename = argv[2];
	
	struct tms start_usr_sys_time[2];
    clock_t start_real_time[2];
    struct tms end_usr_sys_time[2];
    clock_t end_real_time[2];
    
    // system functions test
    printf("\n========== USING SYSTEM FUNCTIONS ==========\n");
    times(&start_usr_sys_time[0]);
    start_real_time[0] = clock();
    
	sys_print_lines_with_char(sign, filename);
    
    times(&end_usr_sys_time[0]);
    end_real_time[0] = clock();
    
    
    // library functions test
    printf("\n========== USING LIBRARY FUNCTIONS ==========\n");
    times(&start_usr_sys_time[1]);
    start_real_time[1] = clock();

    lib_print_lines_with_char(sign, filename);
    
    times(&end_usr_sys_time[1]);
    end_real_time[1] = clock();
    
    // saving results
    double real[2], sys[2], usr[2];
    
    for(int i = 0; i < 2; i++){
        real[i] = (double) (end_real_time[i] - start_real_time[i]) / CLOCKS_PER_SEC;
        sys[i] = (double) (end_usr_sys_time[i].tms_stime - start_usr_sys_time[i].tms_stime) / sysconf(_SC_CLK_TCK);
        usr[i] = (double) (end_usr_sys_time[i].tms_utime - start_usr_sys_time[i].tms_utime) / sysconf(_SC_CLK_TCK);
    }
    
    FILE* fr = fopen("pomiar_zad_2.txt", "w+");
    
    fprintf(fr, "========== USING SYSTEM FUNCTIONS ==========\n");
    fprintf(fr, "Real time: %lfs\n", real[0]);
	fprintf(fr, "Sys time: %lfs\n", sys[0]);
	fprintf(fr, "Usr time: %lfs\n\n", usr[0]);
	
    fprintf(fr, "========== USING LIBRARY FUNCTIONS ===========\n");
    fprintf(fr, "Real time: %lfs\n", real[1]);
	fprintf(fr, "Sys time: %lfs\n", sys[1]);
	fprintf(fr, "Usr time: %lfs\n\n", usr[1]);
	
	fclose(fr);
	
	return 0;
}
