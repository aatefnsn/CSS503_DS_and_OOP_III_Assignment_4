//
// RemoteFileSystem.cpp
//
// Client-side remote (network) filesystem
//
// Author/Implementation: Ahmed Nada

# include "RFS.h"

RemoteFileSystem::RemoteFileSystem(char *host, /*short*/ char *port, unsigned long auth_token, struct timeval *timeout){

struct addrinfo hints;
memset(&hints, 0, sizeof(hints)); // *************************
hints.ai_flags = 0;
hints.ai_family = AF_INET;
hints.ai_socktype = SOCK_DGRAM;
hints.ai_protocol = 0;
hints.ai_addrlen = 0;
hints.ai_addr = NULL;
hints.ai_canonname = NULL;
hints.ai_next = NULL;

int result = getaddrinfo(host, /*(char *)*/port, &hints, &rfs_addrinfo);
  if (result != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(result));
    exit(EXIT_FAILURE);
  }
  this->auth_token = auth_token;
  this->sd = socket(AF_INET, SOCK_DGRAM, 0);
  cout << "Done creating socket" << endl;
    if (this->sd < 0) {
      perror("creating socket");
      exit(EXIT_FAILURE);
    }
}

char * RemoteFileSystem::insertIntInBuffer(char * buf, int len){
  int size =0;
  char * reference= buf;
  char length = ('0' + len);
  char * length_ptr = &length;
  string str= (char *) malloc(sizeof(char) * 100);;
  string bufAsString = buf;
  while (*buf !='\n'&& *buf != '\0'){
    size++;
    buf++;
  }
  //cout << "Size is " << size << endl;
  //cout << "Len is " << len << endl;
  char * toBeCopied = buf-len;
  //cout << "To Be Copied: " << toBeCopied << endl;
  buf = reference;
  //cout << "Inside the function " << buf << endl;
  //cout << " toot" << endl;
    if (size == len){
    str.insert(0,length_ptr);
    str.insert(1,toBeCopied);
    //(str + strlen(str)) == '\0';
    //str.insert(size+1,'0');
    char * toSendBuf =  (char*)malloc(sizeof(char) * size);
    strcpy(toSendBuf,str.c_str());
    toSendBuf[size+1]= '\0';
    return toSendBuf;
    }
    else{
    string previous = bufAsString.substr(0,size-len);
    //cout << "Previous is " << previous << endl;
    char * previous_ptr = (char*)malloc(sizeof(char) * 100);
    strcpy(previous_ptr,previous.c_str());
    //cout << "Previous ptr is " << previous_ptr << endl;
    str.insert(0,previous_ptr);
    //cout << "String is " << str << endl;
    //cout << "size-len+1 is " << size-len+1 << endl;
    str.insert(size-len,length_ptr);
    //cout << "String is " << str << endl;
    str.insert(size-len+1,toBeCopied);
    char * toSendBuf;
    strcpy(toSendBuf,str.c_str());
    toSendBuf[size+1]= '\0';
    return toSendBuf;
  }
}

RemoteFileSystem::File * RemoteFileSystem::openSerialized(const char *pathname, char *mode){
  int pathname_len;
  int mode_len;
  int size=100;
  int len=0;
  string str;
  char * toSendBuf =  (char*)malloc(sizeof(char) * size);
  char * toSendBufTemp =  (char*)malloc(sizeof(char) * size);
  char * beginning= toSendBuf;
  char * reference= toSendBuf;
  while (*pathname !='\n'&& *pathname != '\0'){
    *toSendBuf = *pathname;
    len++;
    toSendBuf++;
    pathname++;
  }
  pathname_len=len;
  toSendBuf = beginning;
  cout << "buffer before is " << toSendBuf << endl;
  toSendBuf = insertIntInBuffer(toSendBuf,len);
  cout << "buffer now is " << toSendBuf << endl;

  beginning=toSendBuf;
  toSendBuf = toSendBuf+len+1;
  len=0;
  while (*mode !='\n'&& *mode != '\0'){
    *toSendBuf = *mode;
    len++;
    toSendBuf++;
    mode++;
  }
  mode_len=len;
  toSendBuf = beginning;
  cout << "buffer before is " << toSendBuf << endl;
  toSendBuf = insertIntInBuffer(toSendBuf,len);
  cout << "buffer now is " << toSendBuf << endl;
  int result = sendto(this->sd, (void *)toSendBuf, pathname_len+mode_len+2, 0, rfs_addrinfo->ai_addr, sizeof(struct sockaddr_in));
    if (result < 0) {
      perror("sending to socket");
      exit(EXIT_FAILURE);
    }else {
      cout << "sent" << endl;
    }
}

RemoteFileSystem::File * RemoteFileSystem::open(char *pathname, char *mode){
  this->currentSequenceNumber++;
  //RemoteFileSystem::message *msg = new message();
  RemoteFileSystem::message *msg = (RemoteFileSystem::message *) malloc(sizeof(RemoteFileSystem::message));

  msg->auth = (this->auth_token);
  htonl(msg->auth);
  strcpy(msg->pathname,pathname);
  strcpy(msg->mode,mode);
  msg->actionType=0;
  msg->sequenceID = currentSequenceNumber;
//  cout << "mode is " << mode << endl;
  //msg->mode = mode;
  int result = sendto(this->sd, (void *)msg, sizeof(struct message), 0, rfs_addrinfo->ai_addr, sizeof(struct sockaddr_in));
  //int result = sendto(this->sd, (void *)toSendBuf, pathname_len+mode_len+2, 0, rfs_addrinfo->ai_addr, sizeof(struct sockaddr_in));
    if (result < 0) {
      perror("sending to socket");
      exit(EXIT_FAILURE);
    }else {
      cout << "sent an open request" << endl;
    }

    //char buf[1024];
  //ssize_t received = recv(sd, buf, 1024 - 1, 0);
  char buffer[sizeof(RemoteFileSystem::message)];
  struct sockaddr_in server_address;
  socklen_t len = sizeof(server_address);
  ssize_t received = recvfrom(sd, buffer, sizeof(RemoteFileSystem::message), 0, (struct sockaddr *) &server_address, &len);
  RemoteFileSystem::message *response = (RemoteFileSystem::message *) malloc(sizeof(RemoteFileSystem::message));
  response = reinterpret_cast<RemoteFileSystem::message*> (buffer);
  cout << "Open Response Received" << endl;
  cout << response->payload << endl;
}


  ssize_t RemoteFileSystem::File::read(void *buf, size_t count){
    //this->rfs = this;
  //cout << "hi" << endl;
  //printf("hi");
  this->rfs->currentSequenceNumber++;

  printf("Current Sequnece number %d\n", this->rfs->currentSequenceNumber);
  printf ("Buf before reading is: %s\n",buf);
   //print ()<< endl;
  //RemoteFileSystem::message *msg = new message();
  RemoteFileSystem::message *msg = (RemoteFileSystem::message *) malloc(sizeof(RemoteFileSystem::message));
  msg->auth = (this->rfs->auth_token);
  htonl(msg->auth);
  //strcpy(msg->pathname,pathname);
  //strcpy(msg->mode,mode);
  msg->actionType=1;
  msg->sequenceID = this->rfs->currentSequenceNumber;
  msg->reqReadCount = count;
  //  cout << "mode is " << mode << endl;
  //msg->mode = mode;
  int result = sendto(this->rfs->sd, (void *)msg, sizeof(struct message), 0, this->rfs->rfs_addrinfo->ai_addr, sizeof(struct sockaddr_in));
  //int result = sendto(this->sd, (void *)toSendBuf, pathname_len+mode_len+2, 0, rfs_addrinfo->ai_addr, sizeof(struct sockaddr_in));
//cout << "sent" << endl;
    if (result < 0) {
      perror("sending to socket");
      exit(EXIT_FAILURE);
    }else {
      printf("sent a reading request\n");
    }

    char buffer[sizeof(RemoteFileSystem::message)];
    struct sockaddr_in server_address;
    socklen_t len = sizeof(server_address);
    ssize_t received = recvfrom(this->rfs->sd, buffer, /*1024 - 1*/sizeof(RemoteFileSystem::message), 0, (struct sockaddr *) &server_address, &len);
  printf("Reading\n");
  RemoteFileSystem::message *response = (RemoteFileSystem::message *) malloc(sizeof(RemoteFileSystem::message));
  response = reinterpret_cast<RemoteFileSystem::message*> (buffer);
  //cout << "Read Response Received" << endl;
  printf("Response Received\n");
  printf("Read bytes are: %s\n", response->payload);
  strcpy( (char *) buf,response->payload);
  printf ("Buf after reading is now: %s\n",buf);
  printf ("Data read are %zd\n", response->resReadCount);
  //cout << response->payload << endl;
}

ssize_t RemoteFileSystem::File::write(void *buf, size_t count){
  //this->rfs = this;
//cout << "hi" << endl;
this->rfs->currentSequenceNumber++;
printf("Current Sequnece number %d\n", this->rfs->currentSequenceNumber);
//RemoteFileSystem::message *msg = new message();
RemoteFileSystem::message *msg = (RemoteFileSystem::message *) malloc(sizeof(RemoteFileSystem::message));
msg->auth = (this->rfs->auth_token);
htonl(msg->auth);
//strcpy(msg->pathname,pathname);
//strcpy(msg->mode,mode);
strcpy(msg->payload,(char *)buf);
msg->actionType=2;
msg->sequenceID = this->rfs->currentSequenceNumber;
msg->reqWriteCount = count;
//  cout << "mode is " << mode << endl;
//msg->mode = mode;
int result = sendto(this->rfs->sd, (void *)msg, sizeof(struct message), 0, this->rfs->rfs_addrinfo->ai_addr, sizeof(struct sockaddr_in));
//int result = sendto(this->sd, (void *)toSendBuf, pathname_len+mode_len+2, 0, rfs_addrinfo->ai_addr, sizeof(struct sockaddr_in));
//cout << "sent" << endl;
  if (result < 0) {
    perror("sending to socket");
    exit(EXIT_FAILURE);
  }else {
    //cout << "sent" << endl;
    printf("sent a write request\n");
  }

//char buffer[1024];
char buffer[sizeof(RemoteFileSystem::message)];
struct sockaddr_in server_address;
socklen_t len = sizeof(server_address);
ssize_t received = recvfrom(this->rfs->sd, buffer, /*1024 - 1*/sizeof(RemoteFileSystem::message), 0, (struct sockaddr *) &server_address, &len);
//printf("write response message size is: %d\n",received);
//cout << "Writing" << endl;
RemoteFileSystem::message *response = (RemoteFileSystem::message *) malloc(sizeof(RemoteFileSystem::message));
response = reinterpret_cast<RemoteFileSystem::message*> (buffer);
printf("Write Response Received\n");
printf( "Data written successfully are %zd\n",(response->resWriteCount));
}


ssize_t RemoteFileSystem::File::lseek(off_t offset, int whence){
  //this->rfs = this;
//cout << "hi" << endl;
printf( "whence is %d\n", whence);
this->rfs->currentSequenceNumber++;
printf("Current Sequnece number %d\n", this->rfs->currentSequenceNumber);
//RemoteFileSystem::message *msg = new message();
RemoteFileSystem::message *msg = (RemoteFileSystem::message *) malloc(sizeof(RemoteFileSystem::message));
msg->auth = (this->rfs->auth_token);
htonl(msg->auth);
//strcpy(msg->pathname,pathname);
//strcpy(msg->mode,mode);
//strcpy(msg->payload,(char *)buf);
msg->actionType=3;
msg->sequenceID = this->rfs->currentSequenceNumber;
msg->lseekoffset = offset;
msg->lseekwhence = whence;
//  cout << "mode is " << mode << endl;
//msg->mode = mode;
int result = sendto(this->rfs->sd, (void *)msg, sizeof(struct message), 0, this->rfs->rfs_addrinfo->ai_addr, sizeof(struct sockaddr_in));
//int result = sendto(this->sd, (void *)toSendBuf, pathname_len+mode_len+2, 0, rfs_addrinfo->ai_addr, sizeof(struct sockaddr_in));
//cout << "sent" << endl;
  if (result < 0) {
    perror("sending to socket");
    exit(EXIT_FAILURE);
  }else {
    //cout << "sent" << endl;
    printf("sent a lseek request\n");
  }

char buffer[sizeof(RemoteFileSystem::message)];
struct sockaddr_in server_address;
socklen_t len = sizeof(server_address);
ssize_t received = recvfrom(this->rfs->sd, buffer, /*1024 - 1*/sizeof(RemoteFileSystem::message), 0, (struct sockaddr *) &server_address, &len);
cout << "lseek-ing" << endl;
RemoteFileSystem::message *response = (RemoteFileSystem::message *) malloc(sizeof(RemoteFileSystem::message));
response = reinterpret_cast<RemoteFileSystem::message*> (buffer);
printf("lseek Response Received\n");
printf( "lseek successful %d\n", (response->lseek_result));
}

int RemoteFileSystem::chmod(const char *pathname, mode_t mode){
  //this->rfs = this;
//cout << "hi" << endl;
//printf( "whence is %d\n", whence);
this->currentSequenceNumber++;
printf("Current Sequnece number %d\n", this->currentSequenceNumber);
//RemoteFileSystem::message *msg = new message();
RemoteFileSystem::message *msg = (RemoteFileSystem::message *) malloc(sizeof(RemoteFileSystem::message));
msg->auth = (this->auth_token);
htonl(msg->auth);
//strcpy(msg->pathname,pathname);
//strcpy(msg->mode,mode);
//strcpy(msg->payload,(char *)buf);
msg->actionType=4;
msg->sequenceID = this->currentSequenceNumber;
strcpy(msg->pathname,pathname);
msg->chmod_mode = mode;
int result = sendto(this->sd, (void *)msg, sizeof(struct message), 0, this->rfs_addrinfo->ai_addr, sizeof(struct sockaddr_in));
  if (result < 0) {
    perror("sending to socket");
    exit(EXIT_FAILURE);
  }else {
    printf("sent a lseek request\n");
  }

char buffer[sizeof(RemoteFileSystem::message)];
struct sockaddr_in server_address;
socklen_t len = sizeof(server_address);
ssize_t received = recvfrom(this->sd, buffer, /*1024 - 1*/sizeof(RemoteFileSystem::message), 0, (struct sockaddr *) &server_address, &len);
cout << "chmod-ing" << endl;
RemoteFileSystem::message *response = (RemoteFileSystem::message *) malloc(sizeof(RemoteFileSystem::message));
response = reinterpret_cast<RemoteFileSystem::message*> (buffer);
printf("chmod Response Received\n");
printf( "chmod successful %d\n", (response->chmod_res));
}
