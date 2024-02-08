# Multiplayer Battleship Game

This is a simple multiplayer Battleship game implemented in C, consisting of two main files: `client.c` and `server.c`. The `client.c` file is executed by the client, while `server.c` manages connections with clients and facilitates gameplay between them.

## Introduction

This project is a modified version of a university assignment developed for an introductory course on System Programming. The game aims to provide a multiplayer experience where players can compete against each other in the classic Battleship game.

## Features

- **Multiplayer Gameplay**: Players can connect to a central server and play against each other.
- **Battleship Gameplay**: Implements the traditional rules of the Battleship game.
- **Client-Server Architecture**: Utilizes a client-server model for handling connections and game logic.
- **Simple Interface**: The interface is kept minimalistic and straightforward for ease of use.

## Usage

### Prerequisites
- Make sure you have a C compiler installed on your system.
- This project relies on the following standard C libraries:
  - `stdio.h`
  - `errno.h`
  - `string.h`
  - `netinet/in.h`
  - `arpa/inet.h`
  - `stdlib.h`
  - `unistd.h`
  - `stdbool.h`
  - `time.h`
- The program should compile and run on most POSIX-compliant systems, including Linux, macOS, and Unix-like operating systems. Some platform-specific differences or requirements may arise during compilation or execution.


### Compilation
To compile the program, execute the following commands in your terminal:

- Using GCC:

```bash
gcc -o client client.c
gcc -o server server.c
```

- Using your preferred C compiler:

Replace "gcc" with the command for your preferred C compiler.

Ensure that your chosen compiler is installed and accessible from the command line. The program should compile successfully on most POSIX-compliant systems.


### Running the Game
1. Start the server by running the `server` executable with the server ID and port number as arguments:

```bash
./server [server_id] [port_number]
```

   For example:

```bash
./server 1 8080
```

2. Start the clients on different terminals or machines by running the `client` executable with the client ID, server IP address, and server port number as arguments:

```bash
./client [client_id] [server_ip] [server_port]
```

   For example:
```bash
./client 1 127.0.0.1 8080
```

3. Follow the on-screen prompts to play the game.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Acknowledgements

This project was inspired by the Battleship game and was developed as part of a university course on System Programming.


