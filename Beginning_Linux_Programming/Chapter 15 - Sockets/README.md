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

## Networ k Information


## Multiple Clients


## Datagrams

