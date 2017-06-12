#ifndef SYNC_H
#define SYNC_H

#include <string>
#include <iostream>
#include <fstream>

int init_linda();
void get_file_access();
void give_file_access();
void exit_linda();

void openFileInfo(std::fstream &file, const std::string fileName);
void readConfig();

extern std::string LINDA_FILE;
extern std::string TEMP_FILE;
#endif
