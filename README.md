UWB Bothell

Professor Name: Morris Bernstein

Course: CSS-503

Assignment #4

Read me file

Student Name: Ahmed Nada

***Assignment#4 description:***

Assignment #4 is all about implementing a Remote Procedure Call
abstraction which hides the complexity of communicating between a client
and server in a distributed application. The way we implemented
assignment#4 makes it like the idea of assignment #2 where we had a pool
of customers coming in randomly and being supported by barbers with each
barber has its own condition variable.

In real life, what happens is that a server will instantiate a new
thread to support each incoming request or customer. Therefore, a server
in that case is more like the barber shop, it will instantiate a server
thread (barber thread) to support that incoming client request (customer
asking for a haircut). If the server runs out of resources, it will be
the same case as if the barber shop runs out of sleeping barbers, and it
will start declining incoming client requests or it will start queuing
them depending on whether or not it has buffering mechanism enabled
(waiting chairs).

Once a connection has been established, a socket address acts as the
condition variable to get the server to serve a client. If a client gets
to know the server's socket address then it can signal the server to
wake it up, if it was sleeping and get the server to support the client
requests until it is has been done. If the server has no other pending
requests (coming constantly or has requests buffers), it will just go to
sleep and sit idle listening on its port for requests.

Out assignment#4 implementation is only handling a single customer and
single customer. If we are to expand this implantation, then we should
be instantiating a new server thread for each incoming request.

***Deliverables:***

1.  RFS.cpp and RFS.h containing my commented code for client side

2.  Server.cpp containing my commented code for the server side.

3.  Build.sh

4.  Run.sh

5.  Read me file

Client run:

anada@ubuntu:\~/Desktop/Assignment_4\$ g++ -std=c++14 \*.cpp

main.cpp: In function 'int main(int, char\*\*)':

main.cpp:13:17: warning: ISO C++ forbids converting a string constant to
'char\*' \[-Wwrite-strings\]

char \* host = \"127.0.0.1\";

\^\~\~\~\~\~\~\~\~\~\~

main.cpp:14:17: warning: ISO C++ forbids converting a string constant to
'char\*' \[-Wwrite-strings\]

char \* port = \"1027\";

\^\~\~\~\~\~

main.cpp:17:19: warning: ISO C++ forbids converting a string constant to
'char\*' \[-Wwrite-strings\]

char \* pathname=\"File.txt\";

\^\~\~\~\~\~\~\~\~\~

main.cpp:18:14: warning: ISO C++ forbids converting a string constant to
'char\*' \[-Wwrite-strings\]

char\* mode=\"r+\";

\^\~\~\~

main.cpp:25:10: warning: ISO C++ forbids converting a string constant to
'char\*' \[-Wwrite-strings\]

buff = \"I LOVE CSS503\";

\^\~\~\~\~\~\~\~\~\~\~\~\~\~\~

RFS.cpp: In member function 'ssize_t
RemoteFileSystem::File::read(void\*, size_t)':

RFS.cpp:164:44: warning: format '%s' expects argument of type 'char\*',
but argument 2 has type 'void\*' \[-Wformat=\]

printf (\"Buf before reading is: %s\\n\",buf);

\^

RFS.cpp:198:47: warning: format '%s' expects argument of type 'char\*',
but argument 2 has type 'void\*' \[-Wformat=\]

printf (\"Buf after reading is now: %s\\n\",buf);

\^

RFS.cpp: In member function 'ssize_t
RemoteFileSystem::File::lseek(off_t, int)':

RFS.cpp:283:58: warning: format '%d' expects argument of type 'int', but
argument 2 has type 'off_t {aka long int}' \[-Wformat=\]

printf( \"lseek successful %d\\n\", (response-\>lseek_result));

\~\~\~\~\~\~\~\~\~\~\~\~\~\~\~\~\~\~\~\~\~\~\~\~\^

anada@ubuntu:\~/Desktop/Assignment_4\$ ./a.out

Done creating socket

sent an open request

Open Response Received

File opened r+

Current Sequnece number 2

Buf before reading is:

sent a reading request

Reading

Response Received

Read bytes are: dlkQJ

Buf after reading is now: dlkQJ

Data read are 5

Current Sequnece number 3

sent a write request

Write Response Received

Data written successfully are 13

whence is 2

Current Sequnece number 4

sent a lseek request

lseek Response Received

lseek successful 61

Current Sequnece number 5

Buf before reading is:

sent a reading request

Reading

Response Received

Read bytes are: BEEP

Buf after reading is now: BEEP

Data read are 4

Current Sequnece number 6

sent a lseek request

chmod Response Received

chmod successful 0

anada@ubuntu:\~/Desktop/Assignment_4\$

Server run:

anada@ubuntu:\~/Desktop/Assignment_4/server\$ g++ -std=c++14 \*.cpp -O3
-pedantic -Wall

main.cpp: In constructor 'Server::Server(short int)':

main.cpp:34:9: warning: unused variable 'sock' \[-Wunused-variable\]

int sock, length, n;

\^\~\~\~

main.cpp:34:15: warning: unused variable 'length' \[-Wunused-variable\]

int sock, length, n;

\^\~\~\~\~\~

main.cpp:34:23: warning: unused variable 'n' \[-Wunused-variable\]

int sock, length, n;

\^

main.cpp:35:16: warning: unused variable 'fromlen' \[-Wunused-variable\]

socklen_t fromlen;

\^\~\~\~\~\~\~

main.cpp:36:25: warning: unused variable 'server' \[-Wunused-variable\]

struct sockaddr_in server;

\^\~\~\~\~\~

main.cpp: In member function 'void Server::Run()':

main.cpp:72:35: warning: unused variable 'recv_msg'
\[-Wunused-variable\]

RemoteFileSystem::message \* recv_msg= (RemoteFileSystem::message
\*)malloc(sizeof(struct RemoteFileSystem::message));

\^\~\~\~\~\~\~\~

anada@ubuntu:\~/Desktop/Assignment_4/server\$ ls -alhtr

total 52K

drwxrwxr-x 4 anada anada 4.0K Jun 15 23:11 ..

-rw-r\--r\-- 1 anada anada 66 Jun 15 23:11 File.txt

-rw-r\--r\-- 1 anada anada 13K Jun 15 23:45 main.cpp

drwxr-xr-x 2 anada anada 4.0K Jun 15 23:46 .

-rwxr-xr-x 1 anada anada 22K Jun 15 23:46 a.out

anada@ubuntu:\~/Desktop/Assignment_4/server\$ ./a.out

waiting

Received a Request

Request Auth Token is: 123456

Request Action type is: 0

Request Sequnce Number is: 1

Open File Request

File.txt

r+

Name: File.txt

File opened r+, File Descriptor is 4

response sent

waiting

Received a Request

Request Auth Token is: 123456

Request Action type is: 1

Request Sequnce Number is: 2

read request

Data read count is: 5

Resulting Read Buffer is: dlkQJ

read response sent

waiting

Received a Request

Request Auth Token is: 123456

Request Action type is: 2

Request Sequnce Number is: 3

Write request

Buffer is: I LOVE CSS503

Data write count is: 13

write response sent

waiting

Received a Request

Request Auth Token is: 123456

Request Action type is: 3

Request Sequnce Number is: 4

lseek request

lseek offset is -5

lseek whence is 2

lseek SEEK_END Request

lseek response sent

waiting

Received a Request

Request Auth Token is: 123456

Request Action type is: 1

Request Sequnce Number is: 5

read request

Data read count is: 4

Resulting Read Buffer is: BEEP

read response sent

waiting

Received a Request

Request Auth Token is: 123456

Request Action type is: 4

Request Sequnce Number is: 6

chmod request

lseek response sent

waiting

\^C

anada@ubuntu:\~/Desktop/Assignment_4/server\$ ls -alhtr

total 52K

-rw-r\--r\-- 1 anada anada 13K Jun 15 23:45 main.cpp

drwxr-xr-x 2 anada anada 4.0K Jun 15 23:46 .

-rwxr-xr-x 1 anada anada 22K Jun 15 23:46 a.out

drwxrwxr-x 4 anada anada 4.0K Jun 15 23:49 ..

-rwxrwxrwx 1 anada anada 66 Jun 15 23:49 File.txt

anada@ubuntu:\~/Desktop/Assignment_4/server\$
