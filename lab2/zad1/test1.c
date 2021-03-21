#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>
#include <string.h>


// TASK 1: Print lines from two files alternately

// reads one line from file and prints the line,
// returns number of read bytes/chars
int sys_print_line(int fd) {
    char* line = (char*) calloc(300, sizeof(char));
    char ch;
    size_t read_bytes = 300;
	int i = 0;

    while(i < read_bytes){
        while(i < read_bytes && read(fd, &ch, 1) == 1){
            line[i++] = ch;
            if (ch == '\n')
				break;
        }
		
		if(i == read_bytes){
			line = realloc(line, read_bytes*2);
			read_bytes *= 2;
		}
		else
			break;
	
    }
	printf("%s", line);
    free(line);
    return i;
 
}

// solution with system functions
void sys_print_alternating_lines(char* name1, char* name2){
	
	if( access( name1, F_OK ) != 0 || access( name2, F_OK ) != 0){
		perror("File does not exists!\n");
		exit(0);
	}
	
	int f1 = open(name1, O_RDONLY);
    int f2 = open(name2, O_RDONLY);
    
    
    int read_bytes1 = sys_print_line(f1);
    int read_bytes2 = sys_print_line(f2);
    while(read_bytes1 != 0 || read_bytes2 != 0)
    {	
		if(read_bytes1 != 0)
			read_bytes1 = sys_print_line(f1);
		if(read_bytes2 != 0)
			read_bytes2 = sys_print_line(f2);
	}
    
    close(f1);
    close(f2);

}


// solution with library functions
void lib_print_alternating_lines(char* name1, char* name2){
	size_t len1 = 0, len2 = 0;
	char* line1 = calloc(300, sizeof(char));
	char* line2 = calloc(300, sizeof(char));
	
	if( access( name1, F_OK ) != 0 || access( name2, F_OK ) != 0){
		perror("File does not exists!\n");
		exit(0);
	}
	
	FILE* f1 = fopen(name1, "r");
    FILE* f2 = fopen(name2, "r");

	while (f1 != NULL && f2 != NULL) {
		if(getline(&line1, &len1, f1) == -1){
			if(getline(&line2, &len2, f2) == -1)
				break;
			else{
				printf("%s\n", line2);
			}
		}
		else if(getline(&line2, &len2, f2) == -1){
			printf("%s\n", line1);
		}
		else{
			printf("%s\n", line1);
			printf("%s\n", line2);
		}
	}
	
	if(line1)
		free(line1);
	if(line2)
		free(line2);
	
	fclose(f1);
    fclose(f2);
}


int main(int argc, char** argv){
	
	char *name1 = calloc(100, sizeof(char));
	char *name2 = calloc(100, sizeof(char));
	
	// collecting input
	if(argc < 3){
		int arg1 = 0, arg2 = 0;
	
		while(arg1 != 1 && arg2 != 1){
			printf("Enter first filename:\n");
			arg1 = scanf("%s",name1);
			printf("Enter second filename:\n");
			arg2 = scanf(" %s",name2);
			}
	
	}
	else{
		strcpy(name1, argv[1]);
		strcpy(name2, argv[2]);
	}
	
	struct tms start_usr_sys_time[2];
    clock_t start_real_time[2];
    struct tms end_usr_sys_time[2];
    clock_t end_real_time[2];
    
    
    // system functions test
    printf("\n========== USING SYSTEM FUNCTIONS ==========\n");
    times(&start_usr_sys_time[0]);
    start_real_time[0] = clock();
    
	sys_print_alternating_lines(name1, name2);
    
    times(&end_usr_sys_time[0]);
    end_real_time[0] = clock();
    
    
    // library functions test
    printf("\n========== USING LIBRARY FUNCTIONS ==========\n");
    times(&start_usr_sys_time[1]);
    start_real_time[1] = clock();

    lib_print_alternating_lines(name1, name2);
    
    times(&end_usr_sys_time[1]);
    end_real_time[1] = clock();
    
    
    // savng results
    double real[2], sys[2], usr[2];
    
    for(int i = 0; i < 2; i++){
        real[i] = (double) (end_real_time[i] - start_real_time[i]) / CLOCKS_PER_SEC;
        sys[i] = (double) (end_usr_sys_time[i].tms_stime - start_usr_sys_time[i].tms_stime) / sysconf(_SC_CLK_TCK);
        usr[i] = (double) (end_usr_sys_time[i].tms_utime - start_usr_sys_time[i].tms_utime) / sysconf(_SC_CLK_TCK);
    }
    
    FILE* fr = fopen("pomiar_zad_1.txt", "w+");
    
    fprintf(fr, "========== USING SYSTEM FUNCTIONS ==========\n");
    fprintf(fr, "Real time: %lfs\n", real[0]);
	fprintf(fr, "Sys time: %lfs\n", sys[0]);
	fprintf(fr, "Usr time: %lfs\n\n", usr[0]);
	
    fprintf(fr, "========== USING LIBRARY FUNCTIONS ===========\n");
    fprintf(fr, "Real time: %lfs\n", real[1]);
	fprintf(fr, "Sys time: %lfs\n", sys[1]);
	fprintf(fr, "Usr time: %lfs\n\n", usr[1]);
	
	free(name1);
	free(name2);
	fclose(fr);
	return 0;
}
