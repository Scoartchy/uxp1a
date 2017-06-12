#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include "sync.h"

#define FLAG "./working_dir/first.flag"
#define MAIN_FIFO "./working_dir/main.fifo"
#define SECONDARY_FIFO "./working_dir/secondary.fifo"
#define NO_TRAFFIC_TIMEOUT 5

struct mutex_data{
	bool spawn_main_writer;
	pthread_mutex_t lock;
};

/* Trying open a file */
void openFileInfo(std::fstream &file, const std::string fileName)
{	
	if(file.bad())
	{
		std::cout << "ERROR: The file " << fileName.c_str() << " could not be found. Program will be terminated." << std::endl;
		std::cout << "Press any key to continue..." << std::endl;
		std::cin.get();
		exit(1);
	}
	else
	{
		std::cout << "The file " << fileName.c_str() << " found and opened." << std::endl;
	}
}

void readConfig()
{
	const std::string CONFIG_FILE = "./working_dir/config";

	std::fstream file;
	file.open(CONFIG_FILE.c_str(), std::ios::in); 
	openFileInfo(file, CONFIG_FILE);

	std::cout << "Reading config file." << std::endl;

	/* Reading config and setting variables */
	std::string line;
	if(getline(file, line).good()) 
	{
		LINDA_FILE = line;
	}
	if(getline(file, line).good()) 
	{
		TEMP_FILE = line;
	}

	/* Default paths */
	if(LINDA_FILE.empty())
		LINDA_FILE = "./working_dir/linda_file";
	
	if(TEMP_FILE.empty())
		TEMP_FILE = "./working_dir/temp_linda_file"; 

	file.close();
}

void *write_main(void *void_ptr){	
	int fifo = open(MAIN_FIFO, O_WRONLY);
	write(fifo, "a", 2);
}
void write_secondary(void *void_ptr){
	int fifo = open(SECONDARY_FIFO, O_WRONLY);
	write(fifo, "a", 2);
}

void *no_traffic_timeout(void *void_ptr){
	char buf[2];
	sleep(NO_TRAFFIC_TIMEOUT);
	printf("No traffic, terminating linda\n");
	fflush(stdout);
	int fifo = open(SECONDARY_FIFO, O_RDONLY);
	read(fifo, &buf, sizeof(char)*2);
}

void *read_secondary(void *void_ptr){
	struct mutex_data *data;
	data = (struct mutex_data *) void_ptr;
	char buf[2];
	int fifo = open(SECONDARY_FIFO, O_RDONLY);
	while(true){
		read(fifo, &buf, sizeof(char)*2);
		pthread_mutex_lock(&(data->lock));
		data->spawn_main_writer = true;
		pthread_mutex_unlock(&(data->lock));
	}
}

void *secondary_manager(void *void_ptr){
	mutex_data data;
	if (pthread_mutex_init(&(data.lock), NULL) != 0)
	{
	    printf("\n mutex init failed\n");
	    return NULL;
	}
	pthread_t read_secondary_thread;
	pthread_t write_main_thread;	
	pthread_create(&read_secondary_thread, NULL, read_secondary, (void*) &data);
	while(true){
		pthread_mutex_lock(&(data.lock));
		if(data.spawn_main_writer){
			pthread_create(&write_main_thread, NULL, write_main, NULL);
			data.spawn_main_writer = false;
		}
		pthread_mutex_unlock(&(data.lock));
		sleep(1);
	}
}

int init_linda(){
	//Stworz/sprawdz czy istnieje plik FLAG, ten kto go stworzy inicjuje kolejki i pisze do main_fifo
	int fd = open(FLAG, O_CREAT | O_EXCL | O_WRONLY, "w");
	pthread_t secondary_manager_thread;	
	if (fd == -1) {
		printf("nie jestesmy pierwsi\n");
		//TODO zamiast sleep zrobic dopoki nie istnieje
		sleep(1);
		pthread_create(&secondary_manager_thread, NULL, secondary_manager, NULL);
		return 1;
	}
	printf("jestesmy pierwsi\n");
	//TODO zakladamy ze sie udalo bo nie ma czasu na glupoty
	int status_main = mkfifo(MAIN_FIFO, 0777);
	int status_secondary = mkfifo(SECONDARY_FIFO, 0777);
	int file = open(LINDA_FILE.c_str(), O_CREAT);
	close(file);
	close(fd);
	//
	//TODO zakladamy ze sie udalo stworzyc thread, nie ma czasu na glupoty
	pthread_t write_main_thread;	
	pthread_create(&write_main_thread, NULL, write_main, NULL);
	pthread_create(&secondary_manager_thread, NULL, secondary_manager, NULL);
	//printf("sec_man_thr: %i\n", secondary_manager_thread);
	return 0;
}

void get_file_access(){
	char buf[2];
	int fifo = open(MAIN_FIFO, O_RDONLY);
	read(fifo, &buf, sizeof(char)*2);
}
void give_file_access(){
	pthread_t write_main_thread;
	//TODO zakladamy ze sie udalo
	pthread_create(&write_main_thread, NULL, write_main, NULL);
}
void exit_linda(){
	//TODO troche nie dziala :P
	get_file_access();
	if(fork()) exit(0);
	pthread_t no_traffic_timeout_thread;	
	pthread_create(&no_traffic_timeout_thread, NULL, no_traffic_timeout, NULL);
	write_secondary(NULL);
	if(fork()) exit(0);
}

