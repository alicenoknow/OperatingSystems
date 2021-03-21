#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>
#include <string.h>


// TASK 5: Rewrite lines from a file to another file, but break lines
// which are longer than 50 characters into seperate lines


// reads one line from a file, saves line in variable 'output'
// returns number of read bytes/chars
int sys_get_line(int fd, char** output) {
    char* line = (char*) calloc(256, sizeof(char));
    char ch;
    size_t read_bytes = 256;
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
	
    *output = line;
    
    return i;
 
}

// solution using system functions
void sys_solution(char* input_filename, char* output_filename){
		
	if( access( input_filename, F_OK ) != 0 ){
		perror("File does not exists!\n");
		exit(0);
	}
	
	int f_out = creat(output_filename, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);	
	int f_in = open(input_filename, O_RDONLY);
	
	char* line = "";
    int read_bytes = sys_get_line(f_in, &line);
    int write_res1 = 0, write_res2 = 0, curr = 0, line_len = 0;
    char* newline = calloc(52, sizeof(char));
    char* tmp = calloc(50, sizeof(char));
    char* rest;

    while(read_bytes != 0 ){	
		curr = 0;
		line_len = read_bytes;
		
		// if line is longer than 50, split it in 50-elements parts and save them
		while(line_len > 51){
			strncpy(tmp, (line + curr), 50);
			sprintf(newline, "%s\n", tmp);
			write_res1 = write(f_out, newline, strlen(newline));
			
			curr += 50;
			line_len -= 50;
		}
		// if line was shorter than 50 or there are some characters left
		// from above splitting, save it to a file
		if(line_len){
			rest = calloc(line_len, sizeof(char));
			
			strncpy(rest, (line + curr), line_len);
			write_res2 = write(f_out, rest, strlen(rest));
			
			free(rest);
		}
			
		if(write_res1 == -1 || write_res2 == -1){
				perror("Error while writing result to file!\n");
				exit(0);
		}
		
		free(line);
		
		
		if(read_bytes != 0)
			read_bytes = sys_get_line(f_in, &line);
	}
    
    
    close(f_out);
    close(f_in);
    free(line);
    free(newline);
    free(tmp);
}

// solution using library functions
void lib_solution(char* input_filename, char* output_filename){	
	if( access( input_filename, F_OK ) != 0  ){
		perror("File does not exists!\n");
		exit(0);
	}
		
	size_t len = 0;
	int line_len = 0, curr = 0;
	char* line = calloc(256, sizeof(char));
	char* newline = calloc(52, sizeof(char));
    char* tmp = calloc(50, sizeof(char));
    char* rest;
	
	FILE* f_in = fopen(input_filename, "r");
	FILE* f_out = fopen(output_filename, "w+");

	while (f_in != NULL ) {
		if(getline(&line, &len, f_in) == -1)
			break;
		
		curr = 0;
		line_len = strlen(line);

		while(line_len > 51){
			strncpy(tmp, (line + curr), 50);
			sprintf(newline, "%s\n", tmp);
			fwrite(newline, sizeof(char), strlen(newline), f_out);
			
			curr += 50;
			line_len -= 50;
		}

		if(line_len){
			rest = calloc(line_len, sizeof(char));
			
			strncpy(rest, (line + curr), line_len);
			fwrite(rest, sizeof(char), strlen(rest), f_out);
			
			free(rest);
		}
	}
	
	if(line)
		free(line);
	
	fclose(f_in);
	fclose(f_out);
    free(newline);
    free(tmp);
}


int main(int argc, char** argv){
	
	if(argc != 3){
		perror("Invalid number of arguments!\n");
		exit(0);
	}
	char* input_filename = argv[1];
	char* output_filename = argv[2];
		
	struct tms start_usr_sys_time[2];
    clock_t start_real_time[2];
    struct tms end_usr_sys_time[2];
    clock_t end_real_time[2];
    
    
    // sys functions test
    times(&start_usr_sys_time[0]);
    start_real_time[0] = clock();
    
    char* output = calloc(strlen(output_filename) + 4, sizeof(char));
	sprintf( output, "sys_%s", output_filename);
	sys_solution(input_filename, output);
    
    times(&end_usr_sys_time[0]);
    end_real_time[0] = clock();
    
    
    // lib functions test
    times(&start_usr_sys_time[1]);
    start_real_time[1] = clock();
	
	output = calloc(strlen(output_filename) + 4, sizeof(char));
	sprintf( output, "lib_%s", output_filename);
    lib_solution(input_filename, output);
    
    times(&end_usr_sys_time[1]);
    end_real_time[1] = clock();
    
    
    // saving results
    double real[2], sys[2], usr[2];
    
    for(int i = 0; i < 2; i++){
        real[i] = (double) (end_real_time[i] - start_real_time[i]) / CLOCKS_PER_SEC;
        sys[i] = (double) (end_usr_sys_time[i].tms_stime - start_usr_sys_time[i].tms_stime) / sysconf(_SC_CLK_TCK);
        usr[i] = (double) (end_usr_sys_time[i].tms_utime - start_usr_sys_time[i].tms_utime) / sysconf(_SC_CLK_TCK);
    }
    
    FILE* fr = fopen("pomiar_zad_5.txt", "w+");
    
    fprintf(fr, "========== USING SYSTEM FUNCTIONS ==========\n");
    fprintf(fr, "Real time: %lfs\n", real[0]);
	fprintf(fr, "Sys time: %lfs\n", sys[0]);
	fprintf(fr, "Usr time: %lfs\n\n", usr[0]);
	
    fprintf(fr, "========== USING LIBRARY FUNCTIONS ===========\n");
    fprintf(fr, "Real time: %lfs\n", real[1]);
	fprintf(fr, "Sys time: %lfs\n", sys[1]);
	fprintf(fr, "Usr time: %lfs\n\n", usr[1]);
	
	fclose(fr);
	free(output);
	
	return 0;
}
