# basic_protocols.md

This note is a compact systems-programming reference for:

- `TCP`
- `UDP`
- socket programming
- IPC mechanisms
- `sd-bus` / D-Bus architecture

Examples assume a Linux or POSIX-style environment because socket APIs and `sd-bus` are most commonly used there.

---

## TCP vs UDP

### TCP

`TCP` (Transmission Control Protocol) is:

- connection-oriented
- reliable
- ordered
- stream-based

Key properties:

- performs a connection setup before data transfer
- retransmits lost packets
- guarantees in-order delivery
- does not preserve application message boundaries

Typical use cases:

- HTTP/HTTPS
- SSH
- database connections
- file transfer

Think of `TCP` as:

- "I care that all bytes arrive correctly and in order"

### UDP

`UDP` (User Datagram Protocol) is:

- connectionless
- lightweight
- message-based
- not reliable by default

Key properties:

- no handshake is required
- packets may be lost, duplicated, or reordered
- preserves datagram boundaries
- lower overhead than TCP

Typical use cases:

- DNS
- streaming
- online gaming
- telemetry
- service discovery

Think of `UDP` as:

- "I care about speed and simplicity more than guaranteed delivery"

### Quick Comparison

| Feature | TCP | UDP |
|---|---|---|
| Connection setup | Yes | No |
| Reliability | Yes | No |
| Ordering | Yes | No |
| Data model | Byte stream | Datagram/message |
| Overhead | Higher | Lower |
| Common usage | Web, DB, SSH | DNS, VoIP, gaming |

---

## Socket Programming Basics

A socket is an OS-managed endpoint for communication.

Common steps in socket programming:

1. create a socket with `socket()`
2. optionally bind it to an IP/port with `bind()`
3. for servers:
   use `listen()` and `accept()` for TCP
4. for clients:
   use `connect()` for TCP
5. exchange data with `send()` / `recv()` or `write()` / `read()`
6. close the socket with `close()`

### Common Socket APIs

| API | Purpose |
|---|---|
| `socket()` | create a socket |
| `bind()` | attach socket to local address and port |
| `listen()` | mark a TCP socket as passive/listening |
| `accept()` | accept a new incoming TCP connection |
| `connect()` | initiate a TCP connection |
| `send()` / `recv()` | send/receive data on connected sockets |
| `sendto()` / `recvfrom()` | send/receive datagrams, common with UDP |
| `close()` | release the socket |
| `setsockopt()` | tune socket behavior |

### Important Address Helpers

- `sockaddr_in`: IPv4 socket address
- `sockaddr_in6`: IPv6 socket address
- `htons()`: host to network short
- `htonl()`: host to network long
- `ntohs()`: network to host short
- `inet_pton()`: text IP to binary
- `inet_ntop()`: binary IP to text

### Socket Domains and Types

| Domain | Meaning |
|---|---|
| `AF_INET` | IPv4 |
| `AF_INET6` | IPv6 |
| `AF_UNIX` | Unix domain sockets |

| Type | Meaning |
|---|---|
| `SOCK_STREAM` | stream socket, usually TCP |
| `SOCK_DGRAM` | datagram socket, usually UDP |

---

## TCP Communication Flow

### Server side

1. `socket()`
2. `bind()`
3. `listen()`
4. `accept()`
5. `recv()` / `send()`
6. `close()`

### Client side

1. `socket()`
2. `connect()`
3. `send()` / `recv()`
4. `close()`

### Interview point

`TCP` is a byte stream, so:

- one `send()` does not guarantee one matching `recv()`
- application protocols often define their own framing
- examples: newline-delimited messages, fixed headers, length-prefixed payloads

---

## TCP Echo Server Example

```cpp
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "socket failed\n";
        return 1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8080);

    if (bind(server_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::cerr << "bind failed\n";
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 5) < 0) {
        std::cerr << "listen failed\n";
        close(server_fd);
        return 1;
    }

    std::cout << "TCP server listening on port 8080\n";

    sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(server_fd, reinterpret_cast<sockaddr*>(&client_addr), &client_len);
    if (client_fd < 0) {
        std::cerr << "accept failed\n";
        close(server_fd);
        return 1;
    }

    char buffer[1024];
    std::memset(buffer, 0, sizeof(buffer));
    ssize_t n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (n > 0) {
        std::cout << "Received: " << buffer << "\n";
        send(client_fd, buffer, n, 0);
    }

    close(client_fd);
    close(server_fd);
    return 0;
}
```

### What this does

- listens on port `8080`
- accepts one client
- reads one message
- echoes it back

### Build

```bash
g++ -std=c++17 tcp_server.cpp -o tcp_server
```

---

## TCP Client Example

```cpp
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

int main() {
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        std::cerr << "socket failed\n";
        return 1;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);

    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        std::cerr << "invalid address\n";
        close(sock_fd);
        return 1;
    }

    if (connect(sock_fd, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) < 0) {
        std::cerr << "connect failed\n";
        close(sock_fd);
        return 1;
    }

    const char* message = "hello from client";
    send(sock_fd, message, std::strlen(message), 0);

    char buffer[1024];
    std::memset(buffer, 0, sizeof(buffer));
    ssize_t n = recv(sock_fd, buffer, sizeof(buffer) - 1, 0);
    if (n > 0) {
        std::cout << "Server replied: " << buffer << "\n";
    }

    close(sock_fd);
    return 0;
}
```

### Build

```bash
g++ -std=c++17 tcp_client.cpp -o tcp_client
```

---

## UDP Communication Flow

With `UDP`, there is no `listen()` or `accept()`.

### Sender flow

1. `socket()`
2. prepare destination address
3. `sendto()`
4. `close()`

### Receiver flow

1. `socket()`
2. `bind()`
3. `recvfrom()`
4. `close()`

### Interview point

`UDP` preserves message boundaries:

- one datagram stays one datagram
- if the packet is too large for the receive buffer, truncation may happen

---

## UDP Receiver Example

```cpp
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

int main() {
    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) {
        std::cerr << "socket failed\n";
        return 1;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(9090);

    if (bind(sock_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::cerr << "bind failed\n";
        close(sock_fd);
        return 1;
    }

    char buffer[1024];
    sockaddr_in sender{};
    socklen_t sender_len = sizeof(sender);

    ssize_t n = recvfrom(
        sock_fd,
        buffer,
        sizeof(buffer) - 1,
        0,
        reinterpret_cast<sockaddr*>(&sender),
        &sender_len
    );

    if (n > 0) {
        buffer[n] = '\0';
        std::cout << "Received UDP message: " << buffer << "\n";
    }

    close(sock_fd);
    return 0;
}
```

---

## UDP Sender Example

```cpp
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

int main() {
    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) {
        std::cerr << "socket failed\n";
        return 1;
    }

    sockaddr_in receiver{};
    receiver.sin_family = AF_INET;
    receiver.sin_port = htons(9090);

    if (inet_pton(AF_INET, "127.0.0.1", &receiver.sin_addr) <= 0) {
        std::cerr << "invalid address\n";
        close(sock_fd);
        return 1;
    }

    const char* message = "hello over udp";
    sendto(
        sock_fd,
        message,
        std::strlen(message),
        0,
        reinterpret_cast<sockaddr*>(&receiver),
        sizeof(receiver)
    );

    std::cout << "UDP message sent\n";
    close(sock_fd);
    return 0;
}
```

---

## Common Socket Interview Topics

### Blocking vs non-blocking

Blocking socket:

- `recv()` waits until data is available

Non-blocking socket:

- calls return immediately
- may fail with `EAGAIN` or `EWOULDBLOCK`

Used with:

- `select()`
- `poll()`
- `epoll()`

### `SO_REUSEADDR`

Helps restart a server without waiting too long for the old port state to clear.

### Partial send/recv

Important rule:

- `send()` may send fewer bytes than requested
- `recv()` may receive fewer bytes than expected

Production code usually loops until the full message is processed.

### Endianness

Network byte order is big-endian.

That is why we use:

- `htons()`
- `htonl()`
- `ntohs()`
- `ntohl()`

---

## Linux Systems Fundamentals

This section adds related Linux concepts that often appear together with sockets, IPC, and daemon programming.

---

## Process vs Thread

### Process

A process is an independent executing program instance with its own:

- virtual address space
- process ID (`PID`)
- file descriptor table
- resources and security context

Examples:

- browser process
- shell process
- database server process

### Thread

A thread is a unit of execution inside a process.

Threads in the same process usually share:

- address space
- heap
- global variables
- open file descriptors

But each thread has its own:

- stack
- registers
- thread-local state

### Quick comparison

| Topic | Process | Thread |
|---|---|---|
| Address space | separate | shared within same process |
| Creation cost | higher | lower |
| Isolation | strong | weaker |
| Communication | IPC needed | direct shared-memory access |
| Failure impact | isolated | may affect whole process |

### Rule of thumb

Use processes when:

- isolation matters
- crashing one worker should not kill others

Use threads when:

- tasks need to share memory efficiently
- low-overhead concurrency is important

---

## `fork()`

`fork()` creates a new process by duplicating the calling process.

After `fork()`:

- parent and child continue execution from the same next instruction
- child gets a new `PID`
- return value differs in parent and child

Return behavior:

- `< 0`: error
- `0`: running in child
- `> 0`: running in parent, value is child PID

### Example

```c
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        printf("Child: pid=%d, parent=%d\n", getpid(), getppid());
    } else {
        printf("Parent: pid=%d, child=%d\n", getpid(), pid);
    }

    return 0;
}
```

### Important idea

Modern Unix systems usually implement `fork()` efficiently using copy-on-write:

- parent and child initially share physical pages
- pages are copied only when one side writes

### Common use

`fork()` is often followed by `exec()` in the child to launch another program.

---

## `exec()`

The `exec` family replaces the current process image with a new program.

Common variants:

- `execl()`
- `execv()`
- `execlp()`
- `execvp()`

### Important behavior

If `exec()` succeeds:

- the old program code is replaced
- execution continues in the new program
- it does not return to the old code

If `exec()` fails:

- it returns `-1`

### Example

```c
#include <stdio.h>
#include <unistd.h>

int main() {
    printf("Before exec\n");
    execl("/bin/ls", "ls", "-l", (char*)NULL);
    perror("execl");
    return 1;
}
```

### `fork()` + `exec()` pattern

This is the classic shell model:

1. parent calls `fork()`
2. child calls `exec()`
3. parent may call `wait()`

---

## `wait()` and `waitpid()`

When a child process exits, the parent should collect its exit status.

Otherwise, the child can remain as a zombie for some time.

Common APIs:

- `wait()`
- `waitpid()`

### Why it matters

`wait()`:

- blocks until one child finishes
- lets parent read termination status

### Example

```c
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    pid_t pid = fork();

    if (pid == 0) {
        printf("Child running\n");
        return 42;
    }

    int status;
    waitpid(pid, &status, 0);

    if (WIFEXITED(status)) {
        printf("Child exited with code %d\n", WEXITSTATUS(status));
    }

    return 0;
}
```

### Related terms

- zombie process: exited child whose status has not yet been collected
- orphan process: child whose parent exited first

---

## Signals

Signals are asynchronous notifications delivered to a process or thread.

Examples:

- `SIGINT`: interrupt, often from `Ctrl+C`
- `SIGTERM`: polite termination request
- `SIGKILL`: force kill, cannot be caught or ignored
- `SIGSEGV`: invalid memory access
- `SIGCHLD`: child status changed
- `SIGUSR1` / `SIGUSR2`: user-defined signals

### Common uses

- stop or terminate a process
- notify a parent about child exit
- trigger a reload or custom event in daemons

### Signal handling example

```c
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void handle_sigint(int signo) {
    printf("Caught SIGINT (%d)\n", signo);
}

int main() {
    signal(SIGINT, handle_sigint);

    while (1) {
        pause();
    }
}
```

### Important caution

Inside signal handlers, only async-signal-safe operations are safe.

In production code, `sigaction()` is usually preferred over `signal()` because it is more predictable and configurable.

---

## Mutex

A mutex is a mutual exclusion lock.

It ensures that only one thread at a time enters a critical section.

Use a mutex when:

- multiple threads modify shared state
- an operation must be atomic at a higher level than a single instruction

### Example idea

Without a mutex:

- two threads incrementing the same counter may race

With a mutex:

- one thread locks
- updates shared data
- unlocks

### `pthread` example

```c
#include <pthread.h>
#include <stdio.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int counter = 0;

void* worker(void* arg) {
    pthread_mutex_lock(&lock);
    counter++;
    pthread_mutex_unlock(&lock);
    return NULL;
}
```

### Interview note

A mutex is mainly for thread synchronization inside a process, though process-shared mutexes also exist in some setups.

---

## Semaphore

A semaphore is a synchronization primitive that maintains a count.

Unlike a mutex:

- a mutex is usually owned by one thread at a time
- a semaphore represents availability count or event count

### Types

- binary semaphore: similar to a lock or event
- counting semaphore: allows multiple units/resources

### Common uses

- producer-consumer signaling
- limiting access to a pool of resources
- coordinating multiple threads or processes

### Example intuition

If a semaphore starts at `3`:

- up to 3 workers may proceed
- the 4th waits until one worker releases

### Common operations

- `sem_wait()`: decrement or block
- `sem_post()`: increment and wake waiters

---

## Socket Basics

A socket is a file descriptor representing a communication endpoint.

Important socket dimensions:

- domain: `AF_INET`, `AF_INET6`, `AF_UNIX`
- type: `SOCK_STREAM`, `SOCK_DGRAM`
- protocol: usually `0` to select default

### Common server lifecycle

1. `socket()`
2. `bind()`
3. `listen()` for TCP
4. `accept()`
5. `send()` / `recv()`
6. `close()`

### Common client lifecycle

1. `socket()`
2. `connect()`
3. `send()` / `recv()`
4. `close()`

### Key insight

In Linux, sockets behave much like other file descriptors:

- they can be read from
- they can be written to
- they can be monitored with `select()`, `poll()`, or `epoll()`

---

## File Descriptors

A file descriptor (`fd`) is a small integer used by Unix-like systems to represent an open resource.

It may refer to:

- regular file
- pipe
- socket
- terminal
- device

### Standard file descriptors

| FD | Meaning |
|---|---|
| `0` | standard input |
| `1` | standard output |
| `2` | standard error |

### Why file descriptors matter

Unix treats many resources with a common I/O model.

That is why functions like these are so important:

- `read()`
- `write()`
- `close()`
- `dup()`
- `dup2()`

### Example

If a socket is returned as `fd = 5`, then:

- `read(5, ...)` can read from it
- `write(5, ...)` can write to it
- `close(5)` releases it

### `dup2()` use case

Shell redirection is based on descriptor manipulation.

Example idea:

- duplicate a file descriptor onto `1`
- now `stdout` writes into that file or pipe

---

## System Calls

A system call is the controlled way a user-space program requests a service from the kernel.

Examples:

- `open()`
- `read()`
- `write()`
- `close()`
- `fork()`
- `execve()`
- `waitpid()`
- `socket()`

### Why system calls exist

User programs cannot directly perform privileged kernel operations.

So they request them through system calls for:

- file I/O
- process creation
- memory management
- networking
- device access

### User space vs kernel space

- user space: normal application code
- kernel space: privileged OS code

A system call transitions from user space to kernel space and back.

### Important idea

Library functions and system calls are not always the same thing.

For example:

- `printf()` is a C library function
- it may eventually use the `write()` system call underneath

---

## More Related Linux Topics

These are closely related and useful to know with the topics above.

### Context switch

A context switch happens when the CPU switches from one running task to another.

This may occur between:

- processes
- threads

It has overhead because the CPU state must be saved and restored.

### Daemon

A daemon is a long-running background service process.

Examples:

- `sshd`
- `systemd`
- logging services

Daemons often use:

- signals
- sockets
- file descriptors
- forks
- event loops

### Zombie and orphan processes

- zombie: child exited, parent has not collected status yet
- orphan: parent exited, child is adopted by another process, often `init` or `systemd`

### `select()`, `poll()`, `epoll()`

These APIs let one thread monitor many file descriptors.

They are heavily used in:

- socket servers
- event-driven daemons
- multiplexed I/O

General idea:

- wait until one or more descriptors become ready for read/write

### Condition variable

A condition variable is used with a mutex so threads can:

- sleep until a condition becomes true
- wake up efficiently when another thread signals them

This is common in:

- producer-consumer queues
- work scheduling

### `mmap()`

`mmap()` maps files or anonymous memory into a process address space.

It is used for:

- shared memory
- fast file I/O patterns
- loading large files

---
## IPC Mechanisms

IPC means Inter-Process Communication.

It allows separate processes to exchange data or synchronize work.

### Common IPC Mechanisms

| IPC Mechanism | Best for | Notes |
|---|---|---|
| pipe | parent-child simple byte stream | unidirectional by default |
| named pipe (FIFO) | unrelated local processes | file-system visible endpoint |
| message queue | discrete messages | preserves message boundaries |
| shared memory | high-speed data sharing | needs extra synchronization |
| semaphore | synchronization | often used with shared memory |
| signal | event notification | tiny payload, async |
| Unix domain socket | local bidirectional communication | supports stream/datagram |

---

## 1. Pipe

A pipe is a simple byte channel, often used between parent and child processes.

### Example idea

```c
int fd[2];
pipe(fd);
```

- `fd[0]` is read end
- `fd[1]` is write end

### Typical flow

1. parent creates pipe
2. parent calls `fork()`
3. child inherits file descriptors
4. one side reads, the other writes

### Best use

- shell pipelines
- small local streaming data

---

## 2. FIFO / Named Pipe

A FIFO is like a pipe but identified by a filesystem path.

Example:

```bash
mkfifo /tmp/myfifo
```

One process can open it for writing and another for reading.

Best use:

- unrelated processes on the same machine
- quick local IPC demos

---

## 3. Message Queue

Message queues send discrete messages instead of raw byte streams.

Benefits:

- preserves message boundaries
- supports prioritization in some APIs

POSIX APIs commonly include:

- `mq_open()`
- `mq_send()`
- `mq_receive()`
- `mq_close()`

Best use:

- task dispatch
- event passing
- producer-consumer designs

---

## 4. Shared Memory

Shared memory maps the same memory region into multiple processes.

Benefits:

- very fast
- avoids repeated copying through kernel buffers

Challenge:

- synchronization is your responsibility

Common APIs:

- `shm_open()`
- `ftruncate()`
- `mmap()`
- `munmap()`

Best use:

- large data sharing
- performance-critical local IPC

### Shared memory example idea

Process A:

- creates shared memory
- writes a struct or buffer

Process B:

- maps the same shared memory
- reads the data

Usually combined with:

- mutexes
- semaphores
- condition variables in shared memory

---

## 5. Semaphore

A semaphore is used for synchronization, not for carrying bulk data.

It controls access to a resource or coordinates process ordering.

Common POSIX APIs:

- `sem_init()`
- `sem_wait()`
- `sem_post()`
- `sem_destroy()`

Use cases:

- protect shared memory
- signal that data is ready
- limit number of concurrent users

---

## 6. Signal

Signals are asynchronous notifications sent to a process.

Examples:

- `SIGINT`
- `SIGTERM`
- `SIGUSR1`
- `SIGCHLD`

Use cases:

- shutdown request
- child process status
- light event notification

Limitations:

- not good for large payloads
- signal handlers have strict safety rules

---

## 7. Unix Domain Socket

A Unix domain socket is like a normal socket for local-machine communication.

Domain:

- `AF_UNIX`

Benefits:

- local only
- lower overhead than network sockets
- bidirectional
- supports stream or datagram mode

Best use:

- local service communication
- daemon-client models
- passing file descriptors

### Why it matters

Many local IPC systems are built on top of socket-like communication because:

- it is flexible
- it supports request-response patterns
- it integrates well with event loops

---

## Choosing an IPC Mechanism

| Need | Good choice |
|---|---|
| simple parent-child stream | pipe |
| unrelated local processes | FIFO or Unix domain socket |
| fast large local data sharing | shared memory |
| event/task messages | message queue |
| synchronization only | semaphore |
| process notification | signal |

---

## `sd-bus` and D-Bus Architecture

`sd-bus` is a `systemd` library API for talking to `D-Bus`.

### What is D-Bus?

`D-Bus` is a message bus system used for IPC on Linux systems.

It is widely used by:

- desktop services
- system services
- `systemd`
- Linux daemons

Instead of processes talking directly to each other by raw sockets, they communicate through a bus using structured messages.

### Core D-Bus Concepts

| Concept | Meaning |
|---|---|
| bus | central message routing channel |
| service name | logical name of a service, for example `org.freedesktop.systemd1` |
| object path | hierarchical object location, for example `/org/freedesktop/systemd1` |
| interface | group of methods/signals/properties |
| method call | request-response operation |
| signal | broadcast-style event |
| property | readable/writable state |

### Main Bus Types

| Bus | Usage |
|---|---|
| system bus | system-wide privileged services |
| session bus | per-user desktop/session services |

### Architecture Picture

```text
Client Process
    |
    | method call / signal subscription
    v
D-Bus daemon or broker
    |
    | routes messages by service name
    v
Service Process
```

### Why use `sd-bus`?

- cleaner than hand-building D-Bus messages
- integrates well with `systemd`
- supports method calls, properties, signals, and object registration

---

## `sd-bus` Usage Pattern

Typical client flow:

1. connect to system or user bus
2. create a method call
3. send the call and wait for reply
4. parse the reply
5. release resources

Typical server flow:

1. connect to bus
2. request a service name
3. register object paths and interfaces
4. expose methods/properties/signals
5. process incoming requests in an event loop

---

## `sd-bus` Client Example

This example calls `ListNames` on the D-Bus daemon to fetch registered service names.

```c
#include <stdio.h>
#include <systemd/sd-bus.h>

int main() {
    sd_bus *bus = NULL;
    sd_bus_error error = SD_BUS_ERROR_NULL;
    sd_bus_message *reply = NULL;
    int r;

    r = sd_bus_open_system(&bus);
    if (r < 0) {
        fprintf(stderr, "Failed to connect to system bus: %d\n", r);
        return 1;
    }

    r = sd_bus_call_method(
        bus,
        "org.freedesktop.DBus",
        "/org/freedesktop/DBus",
        "org.freedesktop.DBus",
        "ListNames",
        &error,
        &reply,
        ""
    );

    if (r < 0) {
        fprintf(stderr, "Method call failed: %s\n", error.message);
        sd_bus_error_free(&error);
        sd_bus_unref(bus);
        return 1;
    }

    r = sd_bus_message_enter_container(reply, SD_BUS_TYPE_ARRAY, "s");
    if (r < 0) {
        fprintf(stderr, "Failed to enter array\n");
        sd_bus_message_unref(reply);
        sd_bus_unref(bus);
        return 1;
    }

    const char *name;
    while ((r = sd_bus_message_read(reply, "s", &name)) > 0) {
        printf("%s\n", name);
    }

    sd_bus_message_unref(reply);
    sd_bus_error_free(&error);
    sd_bus_unref(bus);
    return 0;
}
```

### Build

```bash
gcc sdbus_client.c -o sdbus_client $(pkg-config --cflags --libs libsystemd)
```

### What this shows

- opens the system bus
- makes a method call
- receives an array reply
- iterates through service names

---

## `sd-bus` Service-Side Architecture

When building your own service, you usually expose:

- a service name
- one or more object paths
- one or more interfaces
- methods and properties

Example design:

| Layer | Example |
|---|---|
| service name | `com.example.Calculator` |
| object path | `/com/example/Calculator` |
| interface | `com.example.Calculator` |
| method | `Add(int a, int b) -> int` |

### Flow

1. service acquires the name `com.example.Calculator`
2. client sends a method call to `/com/example/Calculator`
3. interface method `Add` runs in the service
4. service sends a reply with the result

This is conceptually similar to:

- local RPC
- object-oriented IPC

---

## Minimal `sd-bus` Method Handler Example

This example shows the shape of a method callback that adds two integers.

```c
#include <systemd/sd-bus.h>

static int method_add(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
    int a, b;
    int r = sd_bus_message_read(m, "ii", &a, &b);
    if (r < 0) {
        return r;
    }

    return sd_bus_reply_method_return(m, "i", a + b);
}
```

### What happens here

- request message contains two integers
- handler reads them
- reply returns their sum

In a real service, this callback would be registered in an interface vtable and attached to an object path.

---

## `sd-bus` vs Raw Sockets

| Topic | Raw Socket | `sd-bus` |
|---|---|---|
| abstraction | low-level transport | high-level message bus |
| message format | application-defined | typed D-Bus messages |
| discovery | manual | bus names and object paths |
| local service integration | manual | strong Linux/systemd integration |
| best for | network protocols, custom IPC | Linux service IPC |

### Rule of thumb

Use raw sockets when:

- you are building transport/network communication
- you need protocol-level control

Use `sd-bus` when:

- you are building Linux service-to-service IPC
- you want structured messages, methods, signals, and properties

---

## Interview Revision Summary

- `TCP` is reliable, ordered, connection-oriented, and stream-based.
- `UDP` is lightweight, connectionless, and message-based.
- socket servers usually follow `socket -> bind -> listen -> accept`.
- socket clients usually follow `socket -> connect`.
- `TCP` does not preserve message boundaries; `UDP` does.
- shared memory is fast but requires synchronization.
- semaphores coordinate access; they do not carry bulk data.
- Unix domain sockets are strong local IPC tools.
- `D-Bus` is a structured Linux IPC bus.
- `sd-bus` is a `systemd` API for building D-Bus clients and services.

---

## Good Follow-Up Practice

If you want to deepen this topic, practice:

1. multi-client TCP server with `select()` or `epoll()`
2. Unix domain socket client-server example
3. shared memory plus semaphore producer-consumer
4. small `sd-bus` service exposing one method and one property

---

End of basic_protocols.md

