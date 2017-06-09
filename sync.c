#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>

#define FLAG "./working_dir/first.flag"
#define MAIN_FIFO "./working_dir/main.fifo"
#define SECONDARY_FIFO "./working_dir/secondary.fifo"
#define LINDA_FILE "./working_dir/linda_file"
void *write_main(void *void_ptr){	
	int fifo = open(MAIN_FIFO, O_WRONLY);
	write(fifo, "a", 2);
}
int init_linda(){
	//Stworz/sprawdz czy istnieje plik FLAG, ten kto go stworzy inicjuje kolejki i pisze do main_fifo
	int fd = open(FLAG, O_CREAT | O_EXCL | O_WRONLY, "w");
	if (fd == -1) {
		printf("nie jestesmy pierwsi\n");
		//TODO zamiast sleep zrobic dopoki nie istnieje
		sleep(1);
		//TODO inicjalizacja nie pierwszego procesu:
		//TODO czyta secondary fifo
		return 1;
	}
	printf("jestesmy pierwsi\n");
	//TODO zakladamy ze sie udalo bo nie ma czasu na glupoty
	int status_main = mkfifo(MAIN_FIFO, 0777);
	int status_secondary = mkfifo(SECONDARY_FIFO, 0777);
	int file = open(LINDA_FILE, O_CREAT);
	close(file);
	close(fd);
	//TODO stworz watek czytający z secondary fifo
	//
	//TODO zakladamy ze sie udalo stworzyc thread, nie ma czasu na glupoty
	pthread_t write_main_thread;	
	pthread_create(&write_main_thread, NULL, write_main, NULL);
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
//void output(krotka){
//}
//void input(wzorzec krotki, timeout){
//}
//void read(wzorzec krotki, timeout){
//}


int main(){
	init_linda();
	while(true){
		get_file_access();
		printf("1\n");
		fflush(stdout);
		sleep(1);
		give_file_access();
	}
	return 0;
}
