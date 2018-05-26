//
// RemoteFileSystem.h
//
// Client-side remote (network) filesystem
//
// Author: Morris Bernstein
// Copyright 2019, Systems Deployment, LLC.

//#if !defined(RemoteFileSystem_H)
//#define RemoteFileSystem_H


#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <iostream>
using namespace std;

enum action_type {
    READ, WRITE, OPEN, LSEEK, CONFIRM
};

//enum WHENCEE{
//  SEEEK_SET, SEEEK_CUR, SEEEK_END
//};

class RemoteFileSystem {
 public:
int currentSequenceNumber;
////////////////////////////////////////////////////////////////////////
  struct message {
     unsigned long auth;
     int msg_size;
     char payload[1024];
     uint32_t sequenceID;
     int actionType;
     size_t reqReadCount;
     size_t reqWriteCount;
     size_t resReadCount;
     size_t resWriteCount;
     off_t lseekoffset;
     off_t lseek_result; // can reuse lseekoffset to send result
     int lseekwhence;
     int responseActionType;
     action_type action;
     mode_t chmod_mode;
     int chmod_res;
     char pathname[255];
     char mode[2];
     int err_id;
     message(){
     }
   };
/////////////////////////////////////////////////////////////////////

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  // File represents an open file object
  class File {
  public:
    RemoteFileSystem * rfs;
    // Destructor closes open file.
    ~File();
    File(RemoteFileSystem* filesystem, const char *pathname, char *mode);

   ssize_t read(void *buf, size_t count);
   ssize_t write(void *buf, size_t count);
   off_t lseek(off_t offset, int whence);

  private:
    // Only RemoteFileSystem can open a file.
    friend class RemoteFileSystem;

    //File(RemoteFileSystem* filesystem, const char *pathname, char *mode);
    // Disallow copy & assignment
    File(File const &) = delete;
    void operator=(File const &) = delete;
  };
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  // Connect to remote system.  Throw error if connection cannot be
  // made.
  RemoteFileSystem(char *host, /*short*/char* port, unsigned long auth_token, struct timeval *timeout);

  // Disconnect
  ~RemoteFileSystem();

  // Return new open file object.  Client is responsible for
  // deleting.
  File *open( char *pathname, char *mode);
  File *openSerialized(const char *pathname, char *mode);
  char * insertIntInBuffer(char * buf, int len);
  int chmod(const char *pathname, mode_t mode);
  int unlink(const char *pathname);
  int rename(const char *oldpath, const char *newpath);

 private:
  // File class may use private methods of the RemoteFileSystem to
  // implement their operations.  Alternatively, you can create a
  // separate Connection class that is local to your implementation.
  friend class File;
  unsigned long auth_token;
  int sd;
  struct addrinfo *   rfs_addrinfo;

  // Disallow copy & assignment
  RemoteFileSystem(RemoteFileSystem const &) = delete;
  void operator=(RemoteFileSystem const &) = delete;
};


//#endif
