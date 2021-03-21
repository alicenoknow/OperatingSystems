#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>
#include <string.h>


// TASK 4: Rewrite lines from one file to another with replacing lines
// given as argument 'to_replace' with lines given as argument 'replace_with'


// reads one line from a file, saves current line in variable 'output'
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
void sys_solution(char* input_filename, char* output_filename, char* to_replace, char* replace_with){
		
	if( access( input_filename, F_OK ) != 0 ){
		perror("File does not exists!\n");
		exit(0);
	}
	
	int f_out = creat(output_filename, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);	
	int f_in = open(input_filename, O_RDONLY);
	
	char* line = "";
    int read_bytes = sys_get_line(f_in, &line);
    int write_res;

    while(read_bytes != 0 ){	
		
		if(strcmp(line, to_replace) == 0)
			write_res = write(f_out, replace_with, strlen(replace_with));
		else
			write_res = write(f_out, line, strlen(line));
			
		if(write_res == -1){
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
}


// solution using library functions
void lib_solution(char* input_filename, char* output_filename, char* to_replace, char* replace_with){	
	
	if( access( input_filename, F_OK ) != 0  ){
		perror("File does not exists!\n");
		exit(0);
	}
		
	size_t len = 0;
	char* line = calloc(256, sizeof(char));
	
	FILE* f_in = fopen(input_filename, "r");
	FILE* f_out = fopen(output_filename, "w+");

	while (f_in != NULL ) {
		if(getline(&line, &len, f_in) == -1)
			break;
				
		if(strcmp(line, to_replace) == 0)
			fwrite(replace_with, sizeof(char), strlen(replace_with), f_out);
		else
			fwrite(line, sizeof(char), strlen(line), f_out);
	}
	
	if(line)
		free(line);
	
	fclose(f_in);
	fclose(f_out);
}


int main(int argc, char** argv){
	
	if(argc != 5){
		perror("Invalid number of arguments!\n");
		exit(0);
	}
	char* input_filename = argv[1];
	char* output_filename = argv[2];
	char* to_replace = argv[3];
	char* replace_with = argv[4];
	
	// add newline to arguments
	char* to_rep =  calloc(strlen(to_replace) + 2, sizeof(char));
	char* rep_with = calloc(strlen(replace_with) + 2, sizeof(char));
	sprintf( rep_with, "%s\n", replace_with);
	sprintf( to_rep, "%s\n", to_replace);

		
	struct tms start_usr_sys_time[2];
    clock_t start_real_time[2];
    struct tms end_usr_sys_time[2];
    clock_t end_real_time[2];
    
    // system functions test
    times(&start_usr_sys_time[0]);
    start_real_time[0] = clock();
    
    char* output = calloc(strlen(output_filename) + 4, sizeof(char));
	sprintf( output, "sys_%s", output_filename);
	sys_solution(input_filename, output, to_rep, rep_with);
    
    times(&end_usr_sys_time[0]);
    end_real_time[0] = clock();
    
    
    // library functions test
    times(&start_usr_sys_time[1]);
    start_real_time[1] = clock();
	
	output = calloc(strlen(output_filename) + 4, sizeof(char));
	sprintf( output, "lib_%s", output_filename);
    lib_solution(input_filename, output, to_rep, rep_with);
    
    times(&end_usr_sys_time[1]);
    end_real_time[1] = clock();
    
    
    // savnig results
    double real[2], sys[2], usr[2];
    
    for(int i = 0; i < 2; i++){
        real[i] = (double) (end_real_time[i] - start_real_time[i]) / CLOCKS_PER_SEC;
        sys[i] = (double) (end_usr_sys_time[i].tms_stime - start_usr_sys_time[i].tms_stime) / sysconf(_SC_CLK_TCK);
        usr[i] = (double) (end_usr_sys_time[i].tms_utime - start_usr_sys_time[i].tms_utime) / sysconf(_SC_CLK_TCK);
    }
    
    FILE* fr = fopen("pomiar_zad_4.txt", "w+");
    
    fprintf(fr, "========== USING SYSTEM FUNCTIONS ==========\n");
    fprintf(fr, "Real time: %lfs\n", real[0]);
	fprintf(fr, "Sys time: %lfs\n", sys[0]);
	fprintf(fr, "Usr time: %lfs\n\n", usr[0]);
	
    fprintf(fr, "========== USING LIBRARY FUNCTIONS ===========\n");
    fprintf(fr, "Real time: %lfs\n", real[1]);
	fprintf(fr, "Sys time: %lfs\n", sys[1]);
	fprintf(fr, "Usr time: %lfs\n\n", usr[1]);
	
	fclose(fr);
	free(to_rep);
	free(rep_with);
	free(output);
	
	return 0;
}
