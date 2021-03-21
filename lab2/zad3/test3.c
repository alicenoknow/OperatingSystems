#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>
#include <string.h>
#include <math.h>


// TASK 3: Read numbers from a file, count even numbers, save numbers 
// with tens digit equal to 0 or 7 to a file, save numbers which are 
// square of an integer to another file


// check if number is even
int is_even(int number){
	
	return number % 2 == 0;
}

// check if tens digit of given number is equal to 0 or 7
int is_zero_seven(int number){
	
	return (number / 10) % 10 == 0 ||  (number / 10) % 10 == 7;
}

// check if number is a square of an integer
int is_square(int number){
	
	long root = sqrt(number);
	
	return root*root == number;
}


// reads one line from file, returns number of read bytes/chars
// in variable 'number' saves the read number, in variable
// 'number_str' saves string representation of this number
int sys_get_number(int fd, int* number, char** number_str) {
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
	
	*number = atoi(line);
    *number_str = line;
    
    return i;
 
}

// solution usng system functions
void sys_solution(char* filename){
	
	int fa = creat("a_sys.txt", S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	int fb = creat("b_sys.txt", S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	int fc = creat("c_sys.txt", S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	
	if( access( filename, F_OK ) != 0 ){
		perror("File does not exists!\n");
		exit(0);
	}
	
	int fd = open(filename, O_RDONLY);
	int number = 0;
	int even = 0;
	int *p_number = &number;
	char* number_str = "";
    
    
    int read_bytes = sys_get_number(fd, p_number, &number_str);

    while(read_bytes != 0 ){	
		
		if(is_even(number))
			even++;
		if(is_zero_seven(number))
			if(write(fb, number_str, strlen(number_str)) == -1){
				perror("Error while writing result to file!\n");
				exit(0);
			}
		if(is_square(number))
			if(write(fc, number_str, strlen(number_str)) == -1){
				perror("Error while writing result to file!\n");
				exit(0);
			}
		
		number = 0;
		free(number_str);
		
		if(read_bytes != 0)
			read_bytes = sys_get_number(fd, p_number, &number_str);
	}
    
    char* msg = calloc(40, sizeof(char));
    sprintf(msg, "Liczb parzystych jest %d", even);
    
    if(write(fa, msg, strlen(msg)) == -1){
		perror("Error while writing result to file!\n");
		exit(0);
	}
    
    close(fd);
    close(fa);
    close(fb);
    close(fc);
    free(msg);
    free(number_str);
}

// solution usng library functions
void lib_solution(char* filename){
	
	size_t len = 0;
	char* line = calloc(256, sizeof(char));
	int number = 0, even = 0;
	
	if( access( filename, F_OK ) != 0  ){
		perror("File does not exists!\n");
		exit(0);
	}
	
	FILE* fd = fopen(filename, "r");
	FILE* fa = fopen("a_lib.txt", "w+");
	FILE* fb = fopen("b_lib.txt", "w+");
	FILE* fc = fopen("c_lib.txt", "w+");

	while (fd != NULL ) {
		if(getline(&line, &len, fd) == -1)
			break;
			
		number = atoi(line);
		
		if(is_even(number))
			even++;
		if(is_zero_seven(number))
			if(fwrite(line, sizeof(char), strlen(line), fb) != strlen(line)){
				perror("Error while writing result to file!\n");
				exit(0);
			}
		if(is_square(number))
			if(fwrite(line, sizeof(char), strlen(line), fc) != strlen(line)){
				perror("Error while writing result to file!\n");
				exit(0);
			}
	}
	
    char* msg = calloc(40, sizeof(char));
    sprintf(msg, "Liczb parzystych jest %d", even);
    
    fwrite(msg, sizeof(char), strlen(msg), fa);

	if(line)
		free(line);
	
	fclose(fd);
	fclose(fa);
	fclose(fb);
	fclose(fc);
	free(msg);
}


int main(int argc, char** argv){
	
	if(argc != 2){
		perror("Invalid number of arguments!\n");
		exit(0);
	}
	char* filename = argv[1];
	
	struct tms start_usr_sys_time[2];
    clock_t start_real_time[2];
    struct tms end_usr_sys_time[2];
    clock_t end_real_time[2];
    
    // system functions test
    times(&start_usr_sys_time[0]);
    start_real_time[0] = clock();
    
	sys_solution(filename);
    
    times(&end_usr_sys_time[0]);
    end_real_time[0] = clock();
    
    
    // library functions test
    times(&start_usr_sys_time[1]);
    start_real_time[1] = clock();

    lib_solution(filename);
    
    times(&end_usr_sys_time[1]);
    end_real_time[1] = clock();
    
    
    // saving results
    double real[2], sys[2], usr[2];
    
    for(int i = 0; i < 2; i++){
        real[i] = (double) (end_real_time[i] - start_real_time[i]) / CLOCKS_PER_SEC;
        sys[i] = (double) (end_usr_sys_time[i].tms_stime - start_usr_sys_time[i].tms_stime) / sysconf(_SC_CLK_TCK);
        usr[i] = (double) (end_usr_sys_time[i].tms_utime - start_usr_sys_time[i].tms_utime) / sysconf(_SC_CLK_TCK);
    }
    
    FILE* fr = fopen("pomiar_zad_3.txt", "w+");
    
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
