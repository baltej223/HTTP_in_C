# A TCP server written in C using POSIX sockets API

- So I was reading about low level network programming, and to deepen my understanding about network programming, and how UNIX handles that, I thought to make TCP server in C from Scratch.

- So obviously, I wont be writing TCP itself, instead I allowed kernel to handle that for me. And used I am using socket interface provided by OS (POSIX only).

### Current flow goes as follows

- First a new fresh `socket` capable of handling IP4 connections is created.
- Then the socket is bound to an address and port using `bind()`,`120.0.0.`and`8000` in this case.
- Then the program starts to `listen` for new connections.
- When a connection is received, its `accepted` and processed, and closed.

[@baltej223](https://github.com/baltej223)
