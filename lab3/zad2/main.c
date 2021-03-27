#include "firstlib.h"
#include <time.h>
#include <sys/times.h>
#include <sys/wait.h>

// TESTS FOR MERGE TIME

char* reportFile = "raport2.txt";


void performTest(int size, char* sequence, FILE* fr){
	char** tmp = NULL;
	char* name = calloc(20, sizeof(char));
	 
	struct tms start_usr_sys_time[2];
    clock_t start_real_time[2];
    struct tms end_usr_sys_time[2];
    clock_t end_real_time[2];
	
	// Merge time test original
	times(&start_usr_sys_time[0]);
    start_real_time[0] = clock();
    
	tmp = mergeFiles1(size, sequence);
	
	if(tmp)
		free(tmp);
	
	times(&end_usr_sys_time[0]);
    end_real_time[0] = clock();
    
    
    // Merge time test with fork
	times(&start_usr_sys_time[1]);
    start_real_time[1] = clock();
    
    int pid = -1, status;
    char *string = strdup(sequence);
    char* res = NULL;

	for(int i = 0; i < size; i++){
		name = strtok(string, " ");
		name = strtok(NULL, " ");

		if((pid = fork()) == 0){
			res = mergeFiles2(name, i);
			exit(0);
		}
	}
	waitpid(pid, &status, 0);
	
	if(res)
		free(res);
	free(name);
	
	times(&end_usr_sys_time[1]);
    end_real_time[1] = clock();
	
    
    // Save results
    double real[2], sys[2], usr[2];
    
    for(int i = 0; i < 2; i++){
		real[i] = (double) (end_real_time[i] - start_real_time[i]) / CLOCKS_PER_SEC;
		sys[i] = (double) (end_usr_sys_time[i].tms_stime - start_usr_sys_time[i].tms_stime) / sysconf(_SC_CLK_TCK);
		usr[i] = (double) (end_usr_sys_time[i].tms_utime - start_usr_sys_time[i].tms_utime) / sysconf(_SC_CLK_TCK);
	}
	
	fprintf(fr, "\n\n MERGE WITHOUT FORK\n\n");
	fprintf(fr, "Merge real time: %lfs\n", real[0]);
	fprintf(fr, "Merge sys time: %lfs\n", sys[0]);
	fprintf(fr, "Merge usr time: %lfs\n\n", usr[0]);
	
	printf("\n\n MERGE WITHOUT FORK\n\n");
	printf("Merge real time: %lfs\n", real[0]);
	printf("Merge sys time: %lfs\n", sys[0]);
	printf("Merge usr time: %lfs\n\n", usr[0]);
	
	fprintf(fr, "\n\n MERGE WITH FORK\n\n");
	fprintf(fr, "Merge real time: %lfs\n", real[1]);
	fprintf(fr, "Merge sys time: %lfs\n", sys[1]);
	fprintf(fr, "Merge usr time: %lfs\n\n", usr[1]);
	
	printf("\n\n MERGE WITH FORK\n\n");
	printf("Merge real time: %lfs\n", real[1]);
	printf("Merge sys time: %lfs\n", sys[1]);
	printf("Merge usr time: %lfs\n\n", usr[1]);
			
}


int main(int argc, char* argv[]){
	FILE* fr = fopen(reportFile, "w+");
	
	
	printf("Test 1 - small number of pairs\n");
	fputs("Test 1 - small number of pairs\n", fr);
	
	performTest(2, "data/f1.txt:data/f2.txt data/f2.txt:data/f1.txt",fr);
	
		printf("Test 2 - medium number of pairs\n");
	fputs("Test 3 - medium number of pairs\n", fr);
	
	performTest(8, "data/f1.txt:data/f2.txt data/f2.txt:data/f1.txt data/f2.txt:data/f3.txt data/f4.txt:data/f1.txt data/f1.txt:data/f3.txt data/f2.txt:data/f4.txt data/f1.txt:data/f1.txt data/f3.txt:data/f1.txt",fr);
	
	printf("Test 3 - large number of pairs\n");
	fputs("Test 3 - large number of pairs\n", fr);
	
	performTest(20, "data/f1.txt:data/f2.txt data/f2.txt:data/f1.txt data/f2.txt:data/f3.txt data/f4.txt:data/f1.txt data/f1.txt:data/f3.txt data/f2.txt:data/f4.txt data/f1.txt:data/f1.txt data/f1.txt:data/f2.txt data/f2.txt:data/f1.txt data/f2.txt:data/f3.txt data/f4.txt:data/f1.txt data/f1.txt:data/f3.txt data/f2.txt:data/f4.txt data/f1.txt:data/f1.txt data/f1.txt:data/f2.txt data/f2.txt:data/f1.txt data/f2.txt:data/f3.txt data/f4.txt:data/f1.txt data/f1.txt:data/f3.txt data/f2.txt:data/f4.txt",fr);
	
	printf("Test 4 - small numbers of rows\n");
	fputs("Test 4 - small number of rows\n", fr);
	
	performTest(2, "data/f1.txt:data/f2.txt data/f2.txt:data/f1.txt",fr);
	
	printf("Test 5 - medium numbers of rows\n");
	fputs("Test 5 - small number of rows\n", fr);
	
	performTest(2, "data/m1.txt:data/m2.txt data/m2.txt:data/m1.txt",fr);
	
	printf("Test 6 - large numbers of rows\n");
	fputs("Test 6 - large number of rows\n", fr);
	
	performTest(2, "data/l1.txt:data/l2.txt data/l2.txt:data/l1.txt",fr);
	
	fclose(fr);
}
