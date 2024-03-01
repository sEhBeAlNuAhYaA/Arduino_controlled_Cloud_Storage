# Arduino Controlled CLoud Storage

## This project consists of 3 main parts: Server, Client and Arduino integration
 
### The first part: Server
The first part of the project starts the server directly. It works by using `Boost.ASIO` - the library that allows you to work with asynchronous operations and implement basic server functions.
The server program can accept signals from clients and interact with them. Server can store any data coming from clients, it stores user registration data, and it is connected with arduino by USB and sends it relevant server repository content only for the user(client), that sends a request to the server. 

Server work stages:   
1. Start Server;
2. Catch requests from Clients;
3. Encrypt user data(using a hash-function);
4. Store client data for authentification;
5. Get files from client;
6. Store files;
7. Update arduino display(send it relevant server content);
8. Go to loop start to work in cycle.


### The Second part: Client

The client part of the project has similar functions as the server, except it only sends. It sends requests and files. Requests can be arduino-requests or server-requests. An arduino-request allows the client to check repository data, it's size, and how much space is left. A server-request allows the client to register and send or get data from the server. 

Client work stages:
1. Start the client application;
2. Send any requests to the server;

### The Third part: Arduino integration

Arduino takes and processes commands only from the server. Arduino gets current information about the state of the server and server-repositories for one user at a time. 
The arduino circuit has an I2C LCD display, the arduino board itself and some LEDs.
The display allows a client to check their info.


### Conclusion

There is a server that catches a user's requests and a display, which helps you to see any information. All of this works by using `Boost.ASIO`.

# Development stages

This part of documentation consists of development stages and describes all of them. The program development has 3 main parts: Server, Client, Electronics. 

1. Create a server application;
    * Class for the server;
    * Methods for making a connection with a client;
    * Methods for user registration in cloud-client space;
    * Methods for user data(password and login) manipulation;
    * Create data base class for db storage communication:
        *  Encrypt the password using a hash;
        *  Store the hash of the password;
        *  Compare inputted password with the stored hash in the DB of users;
        *  DB error handling (no user in db, wrong password);
        *  Add and delete user data;
    * Catch different requests from users and send necessary of those, to electronic part(Arduino) by USB, and another use for operate with authentification and data sending;
    * Some additional functions.


2. Creating a client application;
    * Class for client;
    * Method those asking for authentification user-data;
    * Methods for making a connection with server;
    * Methods for sending requests for confirm kind of it(arduino-requests and server-requests);
    * Methods for file sending;
    * Methods for taking files from server to user repository;


3. Creating a electronic device;
    * Develop a code for modules communication with arduino;
        * Class for work with display;
        * Methods for catching information from server;
        * Methods for showing this info on display;
    * Part of code for check current server updates;
        * Class for catching server requests;
        * Class for catching server information;
        * Method for sending to server current state of display info(which user check info);

4. RAID 1 + RAID 0 System;
    * Create disk structure for storing data;
    * Create functions for controlling it's data;
        * Functions for restoring lost files;
        * Functions for adding and deleting files;
    * Functions for operating with file data bases;

    