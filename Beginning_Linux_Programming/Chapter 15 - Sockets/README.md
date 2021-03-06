# Summary - Sockets

Specifically, we look at the following:
❑ How a socket connection operates
❑ Socket attributes, addresses, and communications
❑ Network information and the Internet daemon ( inetd / xinetd )
❑ Clients and servers

## What Is a Socket?

A socket is a communication mechanism that allows client/server systems to be developed either locally, on a single machine, or across networks. Linux functions such as printing, connecting to databases, and serving web pages as well as network utilities such as rlogin for remote login and ftp for file transfer usually use sockets to communicate.
Sockets are created and used differently from pipes because they make a clear distinction between client and server. The socket mechanism can implement multiple clients attached to a single server.

## Socket Connections
You can think of socket connections as telephone calls into a busy building. A call comes into an organization and is answered by a receptionist who puts the call through to the correct department (the server process) and from there to the right person (the server socket). Each incoming call (client) is routed to an appropriate end point and the intervening operators are free to deal with further calls. Before you look at the way socket connections are established in Linux systems, you need to understand how they operate for socket applications that maintain a connection.

#### Try It Out
* client1.c
* server1.c

```
$ ./client1
$ ./server1 &
$ ls -lF server_socket
$ ps lx
$ ./client1
$ ./client1 & ./client1 & ./client1 &
```

### Socket Attributes

Sockets are characterized by three attributes: domain, type, and protocol. They also have an address used as their name. The formats of the addresses vary depending on the domain, also known as the protocol family. Each protocol family can use one or more address families to define the address format.
#### Socket Domains

Domains specify the network medium that the socket communication will use. The most common socket domain is AF_INET , which refers to Internet networking that’s used on many Linux local area networks and, of course, the Internet itself. The underlying protocol, Internet Protocol (IP), which only has one address family, imposes a particular way of specifying computers on a network. This is called the IP address.
#### Socket Types

Internet protocols provide two communication mechanisms with distinct levels of service: streams and datagrams.
##### Stream Sockets

Stream sockets (in some ways similar to standard input/output streams) provide a connection that is a sequenced and reliable two-way byte stream. Thus, data sent is guaranteed not to be lost, duplicated, or reordered without an indication that an error has occurred. Large messages are fragmented, transmitted, and reassembled. This is similar to a file stream, which also accepts large amounts of data and splits it up for writing to the low-level disk in smaller blocks. Stream sockets have predictable behavior.

TCP/IP stands for Transmission Control Protocol/Internet Protocol. IP is the low-level protocol for packets that provides routing through the network from one computer to another. TCP provides sequencing, flow control, and retransmission to ensure that large data transfers arrive with all of the data present and correct or with an appropriate error condition reported.

##### Datagram Sockets

Datagram sockets are implemented in the AF_INET domain by UDP/IP connections and provide an unsequenced, unreliable service. (UDP stands for User Datagram Protocol.) However, they are relatively inexpensive in terms of resources, because network connections need not be maintained. They’re fast because there is no associated connection setup time.
Datagrams are useful for “single-shot” inquiries to information services, for providing regular status information, or for performing low-priority logging. They have the advantage that the death of a server doesn’t unduly inconvenience a client and would not require a client restart. Because datagram-based servers usually retain no connection information, they can be stopped and restarted without disturbing their clients.
For now, we leave the topic of datagrams; see the “Datagrams” section near the end of this chapter for more information.

#### Socket Protocols

### Creating a Socket
The socket system call creates a socket and returns a descriptor that can be used for accessing the socket.
```
#include <sys/types.h>
#include <sys/socket.h>
int socket(int domain, int type, int protocol);
```
The socket created is one end point of a communication channel. The domain parameter specifies the address family, the type parameter specifies the type of communication to be used with this socket, and protocol specifies the protocol to be employed.

### Socket Addresses
Each socket domain requires its own address format. For an AF_UNIX socket, the address is described by a structure, sockaddr_un , defined in the sys/un.h include file.
```
struct sockaddr_un {
    sa_family_t sun_family;
    char sun_path[];
};
```
### Naming a Socket
To make a socket (as created by a call to socket ) available for use by other processes, a server program needs to give the socket a name. Thus, AF_UNIX sockets are associated with a file system pathname, as you saw in the server1 example. AF_INET sockets are associated with an IP port number.
```
#include <sys/socket.h>
int bind(int socket, const struct sockaddr *address, size_t address_len);
```
The bind system call assigns the address specified in the parameter, address , to the unnamed socket associated with the file descriptor socket . The length of the address structure is passed as address_len .
The length and format of the address depend on the address family. A particular address structure pointer will need to be cast to the generic address type (struct sockaddr *) in the call to bind .


### Creating a Socket Queue
To accept incoming connections on a socket, a server program must create a queue to store pending requests. It does this using the listen system call.
```
#include <sys/socket.h>
int listen(int socket, int backlog);
```
A Linux system may limit the maximum number of pending connections that may be held in a queue.

### Accepting Connections
Once a server program has created and named a socket, it can wait for connections to be made to the socket by using the accept system call.
```
#include <sys/socket.h>
int accept(int socket, struct sockaddr *address, size_t *address_len);
```
The accept system call returns when a client program attempts to connect to the socket specified by the parameter socket . The client is the first pending connection from that socket’s queue. The accept function creates a new socket to communicate with the client and returns its descriptor. The new socket will have the same type as the server listen socket.
The socket must have previously been named by a call to bind and had a connection queue allocated by listen . The address of the calling client will be placed in the sockaddr structure pointed to by address .
A null pointer may be used here if the client address isn’t of interest.
The address_len parameter specifies the length of the client structure. If the client address is longer than this value, it will be truncated. Before calling accept , address_len must be set to the expected address length. On return, address_len will be set to the actual length of the calling client’s address structure.
If there are no connections pending on the socket’s queue, accept will block (so that the program won’t continue) until a client does make connection. You can change this behavior by using the O_NONBLOCK flag on the socket file descriptor, using the fcntl function in your code like this:
```
int flags = fcntl(socket, F_GETFL, 0);
fcntl(socket, F_SETFL, O_NONBLOCK|flags);

```
The accept function returns a new socket file descriptor when there is a client connection pending or -1 on error.

### Requesting Connections
Client programs connect to servers by establishing a connection between an unnamed socket and the server listen socket. They do this by calling connect .
```
#include <sys/socket.h>
int connect(int socket, const struct sockaddr *address, size_t address_len);
```
The socket specified by the parameter socket is connected to the server socket specified by the parameter address , which is of length address_len . The socket must be a valid file descriptor obtained by a call to socket .
### Closing a Socket
You can terminate a socket connection at the server and client by calling close , just as you would for lowlevel file descriptors. You should always close the socket at both ends. For the server, you should do this when read returns zero. Note that the close call may block if the socket has untransmitted data, is of a connection-oriented type, and has the SOCK_LINGER option set.

### Socket Communications

#### Try It Out
* client2.c
* server2.c

### Host and Network Byte Ordering
When we run these versions of the server and client programs on an Intel processor–based Linux machine, we can see the network connections by using the netstat command. This command will also be available on most UNIX systems configured for networking. It shows the client/server connection waiting to close down. The connection closes down after a small timeout. (Again, the exact output may vary among different versions of Linux.)

#### Try It Out
* client3.c
* server3.c
```
$ ./server2 & ./client2
$ netstat –A inet
# important to terminate running example server programs:
$ killall server1 server2 server3 server4 server5
```

## Network Information
Host database functions are declared in the interface header file netdb.h .
```
#include <netdb.h>
struct hostent *gethostbyaddr(const void *addr, size_t len, int type);
struct hostent *gethostbyname(const char *name);
```
The structure returned by these functions must contain at least these members:
```
struct hostent {
char *h_name;
char **h_aliases;
int h_addrtype;
int h_length;
char **h_addr_list
};
```
If there is no database entry for the specified host or address, the information functions return a null pointer.
Similarly, information concerning services and associated port numbers is available through some service information functions.
```
#include <netdb.h>
struct servent *getservbyname(const char *name, const char *proto);
struct servent *getservbyport(int port, const char *proto);
```
The proto parameter specifies the protocol to be used to connect to the service, either “tcp” for SOCK_STREAM TCP connections or “udp” for SOCK_DGRAM UDP datagrams. The structure servent contains at least these members:
```
struct servent {
char *s_name;
char **s_aliases;
int s_port;
char *s_proto;
};
```


#### Try It Out
* getname.c
* getdate.c

```
$ ./getname tilde
$ ./getname localhost
$ ./getdate localhost
```


### The Internet Daemon (xinetd/inetd)


The Internet daemon is implemented in modern Linux systems by xinetd . This implementation has replaced the original UNIX program, inetd , although you will still see inetd in older Linux systems and on other UNIX-like systems.
xinetd is usually configured through a graphical user interface for managing network services, but it is also possible to modify its configuration files directly. These are typically /etc/xinetd.conf and files in the /etc/xinetd.d directory.
Each service that is to be provided via xinetd has a configuration file in /etc/xinetd.d . xinetd will read all of these configuration files when it starts up, and again if instructed to do so.
- both SOCK_STREAM ( tcp ) and SOCK_DGRAM ( udp ) sockets.



### Socket Options
There are many options that you can use to control the behavior of socket connections — too many to detail here. The setsockopt function is used to manipulate options.
```
#include <sys/socket.h>
int setsockopt(int socket, int level, int option_name,
const void *option_value, size_t option_len);
```
You can set options at various levels in the protocol hierarchy. To set options at the socket level, you must set level to SOL_SOCKET . To set options at the underlying protocol level (TCP, UDP, and so on), set level to the number of the protocol (from either the header file netinet/in.h or as obtained by the
function getprotobyname ).
The option_name parameter selects an option to set; the option_value parameter is an arbitrary value of length option_len bytes passed unchanged to the underlying protocol handler.
setsockopt returns 0 if successful, -1 otherwise. The manual pages describe further options and errors.
## Multiple Clients

Once established, socket connections behave like low-level open file
descriptors and in many ways like bi-directional pipes.
You might need to consider the case of multiple, simultaneous clients connecting to a server. You’ve seen that when a server program accepts a new connection from a client, a new socket is created and the original listen socket remains available for further connections. If the server doesn’t immediately accept further connections, they will be held pending in a queue.
The fact that the original socket is still available and that sockets behave as file descriptors gives you a method of serving multiple clients at the same time. If the server calls fork to create a second copy of itself, the open socket will be inherited by the new child process. It can then communicate with the connecting client while the main server continues to accept further client connections. This is, in fact, a fairly easy change to make to your server program, which is shown in the following Try It Out section.

#### Try It Out
* server4.c
```
$ ./server4 &
$ ./client3 & ./client3 & ./client3 & ps x
```
### select
The select system call allows a program to wait for input to arrive (or output to complete) on a number of low-level file descriptors at once. This means that the terminal emulator program can block until there is something to do. Similarly, a server can deal with multiple clients by waiting for a request on many open sockets at the same time.
The select function operates on data structures, fd_set , that are sets of open file descriptors. A number of macros are defined for manipulating these sets:
```
#include <sys/types.h>
#include <sys/time.h>
void FD_ZERO(fd_set *fdset);
void FD_CLR(int fd, fd_set *fdset);
void FD_SET(int fd, fd_set *fdset);
int FD_ISSET(int fd, fd_set *fdset);
```
The select function can also use a timeout value to prevent indefinite blocking. The timeout value is given using a struct timeval . This structure, defined in sys/time.h , has the following members:
```
struct timeval {
    time_t tv_sec;
    long tv_usec;
}
```
The time_t type is defined in sys/types.h as an integral type.
The select system call has the following prototype:
```
#include <sys/types.h>
#include <sys/time.h>
int select(int nfds, fd_set *readfds, fd_set *writefds,
fd_set *errorfds, struct timeval *timeout);
```
A call to select is used to test whether any one of a set of file descriptors is ready for reading or writing or has an error condition pending and will optionally block until one is ready.

#### Try It Out
* select.c
```
$ ./select
```
### Multiple Clients

Your simple server program can benefit by using select to handle multiple clients simultaneously, without resorting to child processes. For real applications using this technique, you must take care that you do not make other clients wait too long while you deal with the first to connect.
The server can use select on both the listen socket and the clients’ connection sockets at the same time.
Once activity has been indicated, you can use FD_ISSET to cycle through all the possible file descriptors to discover which one the activity is on.
If the listen socket is ready for input, this will mean that a client is attempting to connect and you can call accept without risk of blocking. If a client descriptor is indicated ready, this means that there’s a client request pending that you can read and deal with. A read of zero bytes will indicate that a client process has ended and you can close the socket and remove it from your descriptor set.

#### Try It Out
* server5.c
```
$ ./server5 &
$ ./client3 & ./client3 & ./client3 & ps x
```

## Datagrams

In this chapter, we have concentrated on programming applications that maintain connections to their clients, using connection-oriented TCP socket connections. There are cases where the overhead of establishing and maintaining a socket connection is unnecessary.
The daytime service used in getdate.c earlier provides a good example. You create a socket, make a connection, read a single response, and close the connection. That’s a lot of operations just to get the date.
The daytime service is also available by UDP using datagrams. To use it, you send a single datagram to the service and get a single datagram containing the date and time in response. It’s simple.
Services provided by UDP are typically used where a client needs to make a short query of a server and expects a single short response. If the cost in terms of processing time is low enough, the server is able to provide this service by dealing with requests from clients one at a time, allowing the operating system to hold incoming requests in a queue. This simplifies the coding of the server. Because UDP is not a guaranteed service, however, you may find that your datagram or your response goes missing. So if the data is important to you, you would need to code your UDP clients carefully to check for errors and retry if necessary. In practice, on a local area network, UDP datagrams are very reliable.
To access a service provided by UDP, you need to use the socket and close system calls as before, but rather than using read and write on the socket, you use two datagram-specific system calls, sendto and recvfrom .
#### Try It Out
* getdate2.c

The sendto system call sends a datagram from a buffer on a socket using a socket address and address length. Its prototype is essentially
```
int sendto(int sockfd, void *buffer, size_t len, int flags, struct sockaddr *to, socklen_t tolen);
```
In normal use, the flags parameter can be kept zero.
The recvfrom system call waits on a socket for a datagram from a specified address and receives it into a buffer. Its prototype is essentially
```
int recvfrom(int sockfd, void *buffer, size_t len, int flags,
struct sockaddr *from, socklen_t *fromlen);
```
