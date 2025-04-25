# C++ Web Server

## 🚀 About This Project

I built this web server from scratch as a learning experience to dive deeper into **low-level networking, HTTP protocols, and C++ system programming**. Unlike high-level frameworks, this project allows me to understand **how web servers actually handle requests, manage connections, and optimize performance**.

## 🛠️ Features

- ✅ HTTP request parsing (GET, POST, PUT, DELETE)
- ✅ Static file serving with appropriate content types
- ✅ Error handling with appropriate status codes
- ✅ Clean socket management with proper cleanup
- ⬜ Multi-threaded request processing
- ✅ Customizable logging system
- ✅ Graceful shutdown handling

## 🏗️ Architecture

The server follows a modular design with the following components:

### ServerSocket

- Manages the TCP socket connections
- Handles binding to port, listening for connections
- Accepts incoming client connections
- Provides clean socket closure

### HttpHandler

- Low-level communication layer
- Handles raw data sending/receiving over sockets
- Manages buffer sizes and data chunks
- Prevents oversized requests with MAX_MESSAGE_SIZE limit

### HttpRequest

- Parses incoming HTTP requests
- Extracts method, URI, headers, and body
- Provides accessor methods for request components
- Validates request format

### HttpResponse

- Constructs HTTP responses
- Sets status codes, headers, and body content
- Formats response according to HTTP spec
- Handles client connection closure

### ResourceHandler

- Maps URIs to filesystem resources
- Prevents directory traversal attacks
- Determines appropriate content types
- Reads files and prepares response bodies

## 🖥️ Main Server Loop

The main server process in `server.cpp`:

1. Creates a ServerSocket on port 8080
2. Listens for incoming connections
3. For each connection, creates HttpRequest and HttpResponse objects
4. Uses ResourceHandler to process the request
5. Sends the response back to the client

## ⚡ Getting Started

### Prerequisites

- C++17 compatible compiler
- Make

### Building and Running

```sh
# Compile the project
make

# Run the server
./server
```

The server will start listening on port 8080.

### Testing

A test script is provided in `tests/test_server.sh` to verify server functionality:

```sh
# Make the script executable
chmod +x tests/test_server.sh

# Run the tests
./tests/test_server.sh
```

## 📝 Future Improvements

- Implement multi-threading for handling concurrent requests
- Add a configuration file for customizing server settings
- Implement logging system with different verbosity levels
- Add support for CGI or a simple scripting interface
- Implement HTTP/2 support
