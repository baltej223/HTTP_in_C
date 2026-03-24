# A HTTP server written in C using POSIX sockets API

- So I was reading about low level network programming, and to deepen my understanding about network programming, and how UNIX handles that, I thought to make TCP server in C from Scratch.

- After I was done with the TCP server, I wanted to grow it even more, and convert it into a HTTP server written in C. It's a fork of the previous TCP server which I made ( [baltej223/TCP_in_C](https://github.com/baltej223/TCP_in_C) )

### Current flow goes as follows
- First a new fresh `socket` capable of handling IP4 connections is created.
- Then the socket is bound to an address and port using `bind()`,`INADDR_ANY`and`3000` in this case.
- Then the program starts to `listen` for new connections.
- When a connection is received, its `accepted`.
- The request is read from the client file descriptor.
- Its parsed, and understood, served and closed.

### Build / Run

```bash
make car
```
`car: short form of compile and run`

### Files

- `main.c` - TCP server (socket/bind/listen/accept) + request read loop + prints request.
- `parser.c` / `parser.h` - header terminator detection + header parsing helpers.
- `vector.c` / `vector.h` - a small dynamic vector.
- `def.h` - small macros / defs.
- `Makefile` - build/run target.

[@baltej223](https://github.com/baltej223)
