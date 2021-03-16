#include "firstlib.h"
#include <time.h>
#include <sys/times.h>



void performTest(int size, char* sequance, FILE* fr){
	MainArrayOfBlocks* arr = createMainArray(size);
	char** tmp;
	int idx;
	
	struct tms start_usr_sys_time[4];
    clock_t start_real_time[4];
    struct tms end_usr_sys_time[4];
    clock_t end_real_time[4];
	
	// Merge time test
	times(&start_usr_sys_time[0]);
    start_real_time[0] = clock();
    
	tmp = mergeFiles(size, sequance);
	
	times(&end_usr_sys_time[0]);
    end_real_time[0] = clock();
	
	// Create block time test
	times(&start_usr_sys_time[1]);
    start_real_time[1] = clock();
    
	for(int i = 0; i < size; i++)
		createBlock(arr, tmp[i]);
		
	times(&end_usr_sys_time[1]);
    end_real_time[1] = clock();
	
	// Remove block time test
	times(&start_usr_sys_time[2]);
    start_real_time[2] = clock();
    
	for(int i = 0; i < size; i++)
		removeBlock(arr, i);
		
	times(&end_usr_sys_time[2]);
    end_real_time[2] = clock();
	
	// Add and Delete time test
	times(&start_usr_sys_time[3]);
    start_real_time[3] = clock();
    
	for(int i = 0; i < 2 * size; i++){
		idx = createBlock(arr, tmp[0]);
		removeBlock(arr, idx);
	}
	
	times(&end_usr_sys_time[3]);
    end_real_time[3] = clock();
    
    // Save results
    
    double real[4], sys[4], usr[4];
    
    for(int i = 0; i < 4; i++){
        real[i] = (double) (end_real_time[i] - start_real_time[i]) / CLOCKS_PER_SEC;
        sys[i] = (double) (end_usr_sys_time[i].tms_stime - start_usr_sys_time[i].tms_stime) / sysconf(_SC_CLK_TCK);
        usr[i] = (double) (end_usr_sys_time[i].tms_utime - start_usr_sys_time[i].tms_utime) / sysconf(_SC_CLK_TCK);
    }
    
    
	fprintf(fr, "Merge real time: %lfs\n", real[0]);
	fprintf(fr, "Merge sys time: %lfs\n", sys[0]);
	fprintf(fr, "Merge usr time: %lfs\n\n", usr[0]);
	
	fprintf(fr, "Create real time: %lfs\n", real[1]);
	fprintf(fr, "Create sys time: %lfs\n", sys[1]);
	fprintf(fr, "Create usr time: %lfs\n\n", usr[1]);
	
	fprintf(fr, "Remove real time: %lfs\n", real[2]);
	fprintf(fr, "Remove sys time: %lfs\n", sys[2]);
	fprintf(fr, "Remove usr time: %lfs\n\n", usr[2]);
	
	fprintf(fr, "Add and Delete real time: %lfs\n", real[3]);
	fprintf(fr, "Add and Delete sys time: %lfs\n", sys[3]);
	fprintf(fr, "Add and Delete usr time: %lfs\n\n", usr[3]);
		
    
    cleanAll(arr);
}


int main(int argc, char* argv[]){
	char* reportFile = argv[1];
	FILE* fr = fopen(reportFile, "a+");
	
	printf("Test 1 - small number of pairs\n");
	fputs("Test 1 - small number of pairs\n", fr);
	
	performTest(2, "f1.txt:f2.txt f2.txt:f1.txt",fr);
	
	printf("Test 2 - medium number of pairs\n");
	fputs("Test 2 - medium number of pairs\n", fr);
	
	performTest(8, "f1.txt:f2.txt f2.txt:f1.txt f2.txt:f3.txt f4.txt:f1.txt f1.txt:f3.txt f2.txt:f4.txt f1.txt:f1.txt f3.txt:f1.txt",fr);
	
	printf("Test 3 - large number of pairs\n");
	fputs("Test 3 - large number of pairs\n", fr);
	
	performTest(20, "f1.txt:f2.txt f2.txt:f1.txt f2.txt:f3.txt f4.txt:f1.txt f1.txt:f3.txt f2.txt:f4.txt f1.txt:f1.txt f1.txt:f2.txt f2.txt:f1.txt f2.txt:f3.txt f4.txt:f1.txt f1.txt:f3.txt f2.txt:f4.txt f1.txt:f1.txt f1.txt:f2.txt f2.txt:f1.txt f2.txt:f3.txt f4.txt:f1.txt f1.txt:f3.txt f2.txt:f4.txt",fr);
	
	printf("Test 4 - small numbers of rows\n");
	fputs("Test 4 - small number of rows\n", fr);
	
	performTest(2, "f1.txt:f2.txt f2.txt:f1.txt",fr);
	
	printf("Test 5 - medium numbers of rows\n");
	fputs("Test 5 - small number of rows\n", fr);
	
	performTest(2, "m1.txt:m2.txt m2.txt:m1.txt",fr);
	
	printf("Test 6 - large numbers of rows\n");
	fputs("Test 6 - large number of rows\n", fr);
	
	performTest(2, "l1.txt:l2.txt l2.txt:l1.txt",fr);
	
	fclose(fr);
}



