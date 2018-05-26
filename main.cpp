//
// main.cpp
//
// Client-side remote (network) filesystem
//
// Implementation: Ahmed Nada

#include "RFS.h"
#include <string.h>
#include <iostream>
#include <map>
using namespace std;

#define MAX_MESG_SIZE   1000
enum WHENCEE{
  SEEEK_SET, SEEEK_CUR, SEEEK_END
};
map < WHENCEE, int> m = {{SEEEK_SET,0},{SEEEK_CUR,1},{SEEEK_END,2}};
int main(int argc, char* argv[]) {
  char * host = "127.0.0.1";
  char * port = "1027";
  unsigned long unl= 123456;
  //char * pathname = new char[10];
  char * pathname="File.txt";
  char* mode="r+";
  RemoteFileSystem* rfs = new RemoteFileSystem(host,port,unl, NULL);
  RemoteFileSystem::File * f = rfs->open(pathname, mode);
  f->rfs = rfs;
  char * buff = (char *) malloc(sizeof(char) *5);
  size_t read = f->read(buff,5);
  buff= (char *) malloc(sizeof(char) *500); //less than payload size
  buff = "I LOVE CSS503";
  //map < WHENCEE, int> m = {{SEEEK_SET,0},{SEEEK_CUR,1},{SEEEK_END,2}};
  size_t wrote = f->write(buff,13);
  off_t offset=-5;
  //int whence;
  ssize_t seek = f->lseek(offset, SEEEK_END);
  buff = (char *) malloc(sizeof(char) *5);
  read = f->read(buff,4);
  mode_t mod = 0777;
  int chmod_res;
  chmod_res = rfs->chmod(pathname, mod);
  return EXIT_SUCCESS;
}
