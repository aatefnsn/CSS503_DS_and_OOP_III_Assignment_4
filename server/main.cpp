#include "../RFS.h"
#include <string.h>
#include <iostream>
#include <cstddef> // for size_t
#include <exception>
#include <sys/types.h>// for mode_t
#include <unistd.h>
#include <cassert>
#include <cstdarg>
#include <fcntl.h>  // open
//#include <stdlib.h>
#include <iostream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>


using namespace std;

//#define MAX_MESG_SIZE   1000

class Server{
public:

  short port;
  int sockfd;
  int fd_;
  RemoteFileSystem::message * request_message;
  Server(short port){
    this->port = port;
    int sock, length, n;
     socklen_t fromlen;
     struct sockaddr_in server;


int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
this->sockfd = sockfd;
  if (sockfd < 0) {
    perror("creating socket");
    exit(EXIT_FAILURE);
  }

struct sockaddr_in server_address;
  bzero(&server_address, sizeof(server_address));
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  server_address.sin_port = htons(/*server_port*/port);

  int rc = bind(sockfd, (struct sockaddr *) &server_address, sizeof(server_address));
    if (rc < 0) {
      perror("bind");
      exit(EXIT_FAILURE);
    }
}
  ~Server(){

  }

  int getRequestType(RemoteFileSystem::message * msg){
    return msg->actionType;
  }

  void Run(){
    for (;;){
      int n;
      char buf[8192];
      //char buf [sizeof(struct RemoteFileSystem::message)];
      //ssize_t received = recv(sd, buf, 1024 - 1, 0);
      RemoteFileSystem::message * recv_msg= (RemoteFileSystem::message *)malloc(sizeof(struct RemoteFileSystem::message));
      //struct sockaddr_in client_address;
      //socklen_t *len = (socklen_t *)malloc(sizeof(socklen_t));
      struct sockaddr_in client_address;
      socklen_t len = sizeof(client_address);
      //while (1) {
      cout << "waiting" << endl;
      //n = recvfrom(sockfd, recv_msg, sizeof(struct RemoteFileSystem::message), 0, (struct sockaddr *) &client_address, len);
      n = recvfrom(sockfd, buf, sizeof(struct RemoteFileSystem::message), 0, (struct sockaddr *) &client_address, &len);
      //n = recvfrom(sockfd, buf, 100, 0, (struct sockaddr *) &client_address, len);
        if (n < 0) {
        perror("recvfrom");
        }

        //printf("Received from %s:%d", inet_ntoa(client_address.sin_addr), ntos(client_address.sin-port));

      RemoteFileSystem::message * recv_msg2 = reinterpret_cast<RemoteFileSystem::message*> (buf);
      cout << "Received a Request" << endl;
      cout << "Request Auth Token is: " <<recv_msg2->auth << endl;
      cout << "Request Action type is: " << recv_msg2->actionType << endl;
      cout << "Request Sequnce Number is: " << recv_msg2->sequenceID << endl;

      this->request_message = recv_msg2;

if (getRequestType(recv_msg2) == 0){
  //int fd_;
//  this->fd_ = fd_;
  cout << "Open File Request" << endl;
  cout << recv_msg2->pathname << endl;
  cout << recv_msg2->mode << endl;
  char * mode = recv_msg2->mode;
  char * name = recv_msg2->pathname;
  cout << "Name: " <<name << endl;
  if (*mode == 'r') { // open mode for read
        mode++;
        if (*mode == '+') { // if open mode is r+ give r 4 and w 2 writes to all users, groups so 0666
            fd_ = open(name, O_RDWR, 00666);
            //this->file_access_mode = READING_AND_WRITING;
            if (fd_ < 0) {
                perror("A1");
                exit(1);
            } else {
                cout << "File opened r+, File Descriptor is " << fd_ << endl;
                RemoteFileSystem::message * res_msg= (RemoteFileSystem::message *)malloc(sizeof(struct RemoteFileSystem::message));
                res_msg->sequenceID = recv_msg2->sequenceID;
                res_msg->responseActionType = recv_msg2->actionType;
                strcpy(res_msg->payload, "File opened r+");

                //len = (socklen_t) sizeof(client_address);
int result = sendto(sockfd, (void *)res_msg,sizeof(struct RemoteFileSystem::message), 0, (struct sockaddr *) &client_address,(sizeof(struct sockaddr)));
                if (result < 0) {
                  perror("Server: sending to socket");
                  exit(EXIT_FAILURE);
                }else {
                  cout << "response sent" << endl;

                }
            }
        } else {
            fd_ = open(name, O_RDONLY, 00444); // give read 4 only rights to all users, groups
            //this->file_access_mode = READING_ONLY;
            if (fd_ < 0) {
                perror("B1");
                exit(1);
            } else {
                cout << "File opened r, File Descriptor is " << fd_ << endl;
                RemoteFileSystem::message * res_msg= (RemoteFileSystem::message *)malloc(sizeof(struct RemoteFileSystem::message));
                res_msg->sequenceID = recv_msg2->sequenceID;
                res_msg->responseActionType = recv_msg2->actionType;
                strcpy(res_msg->payload, "File opened r");
            }
        }
    } else if (*mode == 'w') {
        mode++;
        if (*mode == '+') {
            fd_ = open(name, O_RDWR | O_CREAT | O_TRUNC, 00666); // w+ give read and write rights to all users, groups
            //this->file_access_mode = READING_AND_WRITING;
            if (fd_ < 0) {
                perror("C1");
                exit(1);
            } else {
                cout << "File opened w+, File Descriptor is " << fd_ << endl;
                RemoteFileSystem::message * res_msg= (RemoteFileSystem::message *)malloc(sizeof(struct RemoteFileSystem::message));
                res_msg->sequenceID = recv_msg2->sequenceID;
                res_msg->responseActionType = recv_msg2->actionType;
                strcpy(res_msg->payload, "File opened w+");
            }
        } else {
            fd_ = open(name, O_WRONLY | O_CREAT | O_TRUNC, 00222); // w give wirte 2 only for all users. groups
            //this->file_access_mode = WRITING_ONLY;
            if (fd_ < 0) {
                perror("D1");
                exit(1);
            } else {
                cout << "File opened w, File Descriptor is " << fd_ << endl;
                RemoteFileSystem::message * res_msg= (RemoteFileSystem::message *)malloc(sizeof(struct RemoteFileSystem::message));
                res_msg->sequenceID = recv_msg2->sequenceID;
                res_msg->responseActionType = recv_msg2->actionType;
                strcpy(res_msg->payload, "File opened w");
            }
        }
    } else {
        cout << "File operation not allowed" << endl;
    }
}

if (getRequestType(recv_msg2) == 1){
  cout << "read request" << endl;
  char *buffer = new char[recv_msg2->reqReadCount];
  //char buffer[recv_msg2->reqReadCount];
  //cout << "Hi"<< endl;
  //cout << "This fd_ is " << this->fd_ << endl;
  size_t data_read = read(this->fd_, buffer, recv_msg2->reqReadCount);
  cout << "Data read count is: " << data_read << endl;
  cout << "Resulting Read Buffer is: " << buffer << endl;

  RemoteFileSystem::message * res_msg= (RemoteFileSystem::message *)malloc(sizeof(struct RemoteFileSystem::message));
  res_msg->sequenceID = recv_msg2->sequenceID;
  res_msg->responseActionType = recv_msg2->actionType;
  res_msg->resReadCount=data_read;
  strcpy(res_msg->payload, buffer);
  //len = (socklen_t) sizeof(client_address);
int result = sendto(sockfd, (void *)res_msg,sizeof(struct RemoteFileSystem::message), 0, (struct sockaddr *) &client_address,(sizeof(struct sockaddr)));
if (result < 0) {
                  perror("Server: sending to socket");
                  exit(EXIT_FAILURE);
                }else {
                  cout << "read response sent" << endl;
                }
}

if (getRequestType(recv_msg2) == 2){ ///// writeeeeeeeeeeeeeee
  cout << "Write request" << endl;
  char *buffer = new char[recv_msg2->reqWriteCount];
  strcpy(buffer,recv_msg2->payload);
  buffer [recv_msg2->reqWriteCount+1] = '\0';
  cout << "Buffer is: " << buffer << endl;
  //char buffer[recv_msg2->reqReadCount];
  //cout << "Hi"<< endl;
  //cout << "This fd_ is " << this->fd_ << endl;
  size_t data_written = write(this->fd_, (void *)buffer, recv_msg2->reqWriteCount);
  cout << "Data write count is: " << data_written << endl;
  //cout << "Resulting Read Buffer is: " << buffer << endl;

  RemoteFileSystem::message * res_msg= (RemoteFileSystem::message *)malloc(sizeof(struct RemoteFileSystem::message));
  res_msg->sequenceID = recv_msg2->sequenceID;
  res_msg->responseActionType = recv_msg2->actionType;
  res_msg->resWriteCount= (data_written);
  //res_msg->resWriteCount=data_written;
  //strcpy(res_msg->payload, buffer);
  //len = (socklen_t) sizeof(client_address);
int result = sendto(sockfd, (void *)res_msg,sizeof(struct RemoteFileSystem::message), 0, (struct sockaddr *) &client_address,(sizeof(struct sockaddr)));

if (result < 0) {
                  perror("Server: sending to socket");
                  exit(EXIT_FAILURE);
                }else {
                  cout << "write response sent" << endl;
                }
}

if (getRequestType(recv_msg2) == 3){ ///// lseek
  cout << "lseek request" << endl;
  int whence;
  off_t offset;
  off_t lseek_result;
  whence=recv_msg2->lseekwhence;
  offset= recv_msg2->lseekoffset;
  cout << "lseek offset is " << offset << endl;
  cout << "lseek whence is " << whence << endl;
  //cout << "Whence is: " << recv_msg2->lseekwhence  << endl;
  if (whence ==0){
    cout << "lseek SEEK_SET Request" << endl;
    lseek_result = lseek(this->fd_,offset,SEEK_SET);

  } else if (whence ==1){
    cout << "lseek SEEK_CUR Request" << endl;
    lseek_result = lseek(this->fd_,offset,SEEK_CUR);
  }
  else{
    cout << "lseek SEEK_END Request" << endl;
    lseek_result = lseek(this->fd_,offset,SEEK_END);
  }

  RemoteFileSystem::message * res_msg= (RemoteFileSystem::message *)malloc(sizeof(struct RemoteFileSystem::message));
  res_msg->sequenceID = recv_msg2->sequenceID;
  res_msg->responseActionType = recv_msg2->actionType;
  res_msg->lseek_result = lseek_result;
  //res_msg->resWriteCount= data_written;
  //res_msg->resWriteCount=data_written;
  //strcpy(res_msg->payload, buffer);
  //len = (socklen_t) sizeof(client_address);
int result = sendto(sockfd, (void *)res_msg,sizeof(struct RemoteFileSystem::message), 0, (struct sockaddr *) &client_address,(sizeof(struct sockaddr)));
if (result < 0) {
                  perror("Server: sending to socket");
                  exit(EXIT_FAILURE);
                }else {
                  cout << "lseek response sent" << endl;
                }
}


if (getRequestType(recv_msg2) == 4){ ///// chmod
  cout << "chmod request" << endl;
  //int whence;
  mode_t mode= recv_msg2->chmod_mode;
  char * name = recv_msg2->pathname;
  int chmod_res = chmod(name, mode) ;
  //off_t offset;
  //off_t lseek_result;
  //whence=recv_msg2->lseekwhence;
  //offset= recv_msg2->lseekoffset;
  //cout << "chmod offset is " << offset << endl;
  //cout << "lseek whence is " << whence << endl;
  //cout << "Whence is: " << recv_msg2->lseekwhence  << endl;
  //if (whence ==0){
  //  cout << "lseek SEEK_SET Request" << endl;
  //  lseek_result = lseek(this->fd_,offset,SEEK_SET);
  //} else if (whence ==1){
  //  cout << "lseek SEEK_CUR Request" << endl;
  //  lseek_result = lseek(this->fd_,offset,SEEK_CUR);
  //}
  //else{
  //  cout << "lseek SEEK_END Request" << endl;
  //  lseek_result = lseek(this->fd_,offset,SEEK_END);
  //}
  RemoteFileSystem::message * res_msg= (RemoteFileSystem::message *)malloc(sizeof(struct RemoteFileSystem::message));
  res_msg->sequenceID = recv_msg2->sequenceID;
  res_msg->responseActionType = recv_msg2->actionType;
  res_msg->chmod_res = chmod_res;
  //res_msg->resWriteCount= data_written;
  //res_msg->resWriteCount=data_written;
  //strcpy(res_msg->payload, buffer);
  //len = (socklen_t) sizeof(client_address);
int result = sendto(sockfd, (void *)res_msg,sizeof(struct RemoteFileSystem::message), 0, (struct sockaddr *) &client_address,(sizeof(struct sockaddr)));
if (result < 0) {
                  perror("Server: sending to socket");
                  exit(EXIT_FAILURE);
                }else {
                  cout << "lseek response sent" << endl;
                }
}
      //cout << buf << endl;
    }
  }

};


int main(int argc, char* argv[]) {
  short port = 1027;
  Server s = Server(port);
  s.Run();

  return EXIT_SUCCESS;
  }
