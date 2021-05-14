#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <sys/time.h>

int width = -1;
int height = -1;
int max_pixel = -1;
int MAX_LINE = 200;
int **image, **negative;
int threads_no;


typedef struct Reply{
	
	int id;
    long unsigned int time;
    int max_pixel;
    int from;
    int to;
    
} Reply;


void open_image(char* file_path){
	
	FILE* fd = fopen(file_path, "r");
    if (fd == NULL){
        perror("Error: cannot open the file.\n");
        exit(1);
    }
    
    int format = -1;
    char* line = calloc(MAX_LINE, sizeof(char));
    
    // get parameters
    while (fgets(line, MAX_LINE*sizeof(char), fd)){
		
		if (line[0] == '#')
			continue;
			
		if (format == -1)
			if (strcmp(line, "P2\n") == 0) format = 1;
			else {
				perror("Error: invalid format of the file.\n");
				exit(1);
			}
		
		else if (width == -1) 
			if (sscanf(line, "%d %d\n", &width, &height) == 2)
				continue;
			else {
				perror("Error: cannot read width and height of image.\n");
				exit(1);
			}
		
		else if (max_pixel == -1)
			if (sscanf(line, "%d\n", &max_pixel) == 1){
				if (max_pixel >= 0 && max_pixel < 256)
					break;
			}
			else {
				perror("Error: cannot read max pixel value of image.\n");
				exit(1);
			}
			
		else{
			break;
		}
	}
	
	int value;
	image = calloc(height, sizeof(int*));
	for (int i = 0; i < height; i++)
		image[i] = calloc(width, sizeof(int));
	
	// Get image content
	for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++){
			
            fscanf(fd, "%d", &value);
            image[i][j] = value;
        }
	
	free(line);
    fclose(fd);
}


void save_negative(char* file_path, int max_neg_pixel){

	FILE* fd = fopen(file_path, "w+");
	if (fd == NULL){
        perror("Error: cannot open output file.\n");
        exit(1);
    }
	
	fprintf(fd, "P2\n%d %d\n%d\n", width, height, max_neg_pixel);
	
	for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++)
            fprintf(fd, "%d ", negative[i][j]);
        fprintf(fd, "\n");
    }
	fclose(fd);
}


void* numbers_method(void* reply){
	struct timeval stop, start;
    gettimeofday(&start, NULL);
	
	int value;
    int from = ((Reply*)reply)->from; 
    int to = ((Reply*)reply)->to;
    
    for (int i = 0; i < height; i++)
        for (int j = 0; j <= width; j++){
            value = image[i][j];
            
            if (value < from && value >= to)
				continue;
				
            negative[i][j] = 255 - value;
            
            if (255 - value > ((Reply*)reply)->max_pixel)
				((Reply*)reply)->max_pixel = 255 - value;
        }
    
    gettimeofday(&stop, NULL);
    ((Reply*)reply)->time = (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
    
    pthread_exit(0);
}


void* blocks_method(void* reply){
	struct timeval stop, start;
    gettimeofday(&start, NULL);
	
	int value;
    int from = ((Reply*)reply)->from; 
    int to = ((Reply*)reply)->to;
    
    for (int i = 0; i < height; i++)
        for (int j = from; j <= to; j++){
            value = image[i][j];
            negative[i][j] = 255 - value;
            
            if (255 - value > ((Reply*)reply)->max_pixel)
				((Reply*)reply)->max_pixel = 255 - value;
        }
    
    gettimeofday(&stop, NULL);
    ((Reply*)reply)->time = (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
    
    pthread_exit(0);
}

void split_by_columns(Reply** threads_block){
	for (int i = 0; i < threads_no; i++){
		threads_block[i]->from = (threads_block[i]->id) * ceil(width / threads_no);
		threads_block[i]->to = ((threads_block[i]->id + 1) * ceil(width / threads_no) - 1) ;
	}
}

void split_by_values(Reply** threads_block){
	
	for (int i = 0; i < threads_no; i++){
		threads_block[i]->from = 256 / threads_no * threads_block[i]->id;
		threads_block[i]->to = (256 / threads_no * (threads_block[i]->id + 1));
	}
}


int main(int argc, char** argv){
	
	// Arguments
	if (argc < 5){
		
		perror("Error: invalid number of arguments.\n");
		exit(1);
	}
	
	threads_no = atoi(argv[1]);
	char* mode = argv[2];
	char* file_path = argv[3];
	char* output_path = argv[4];
	
	// Get input image and allocate variables
	open_image(file_path);
	negative = calloc(height, sizeof(int*));
	for (int i = 0; i < height; i ++)
		negative[i] = calloc(width, sizeof(int));
	
	pthread_t* threads = calloc(threads_no, sizeof(pthread_t));
	Reply** threads_block = calloc(threads_no, sizeof(Reply*));
	for (int i = 0; i < threads_no; i++){
		threads_block[i] = calloc(1, sizeof(Reply));
		threads_block[i]->id = i;
	}
	
	if (strcmp(mode, "numbers") == 0){
		split_by_values(threads_block);
	}else
		split_by_columns(threads_block);
		
	// Starting time measurement
	struct timeval stop, start;
    gettimeofday(&start, NULL);
	
	// Spawn threads	
	 for(int i = 0; i < threads_no; i++){
		threads_block[i]->id = i;
		
        if (strcmp(mode, "numbers") == 0){
            pthread_create(&threads[i], NULL, &numbers_method, threads_block[i]);
        }

        else if (strcmp(mode, "blocks") == 0){
            pthread_create(&threads[i], NULL, &blocks_method, threads_block[i]);
        }
    }
	
	// Join threads and find max pixel value for negative
	int max_neg = 0;
	for(int i = 0; i < threads_no; i++) {
        pthread_join(threads[i], NULL);
        if (threads_block[i]->max_pixel > max_neg)
			max_neg = threads_block[i]->max_pixel;
    }
	
	
	// Stop measuring time and save result
	gettimeofday(&stop, NULL);
    long unsigned int time = (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
	
	FILE* fd = fopen("results.txt", "a+");
	
	fprintf(fd, "\n___________ MEASUREMENT ____________\n\n");
	fprintf(fd, "Threads: %d\nMethod: %s\nTotal time: %lue-6 [s]\n", threads_no, mode, time);
	for (int i = 0; i < threads_no; i++)
		fprintf(fd, "Thread %d time: %lue-6 [s]\n", i, threads_block[i]->time);
	fclose(fd);
	
	// Save image
	save_negative(output_path, max_neg);
	
	
	// Clean up
	for (int i = 0; i < height; i++){
		free(image[i]);
		free(negative[i]);
	}
	
	for (int i = 0; i < threads_no; i++)
		free(threads_block[i]);
	
	free(threads_block);
	free(negative);
	free(image);
	
	return 0;
}
