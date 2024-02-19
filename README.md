# Arduino Controlled CLoud Storage

## This project consists of 3 main parts: Server, Client and Arduino integration
 
### The first part: Server
First part of project start server directly. It works on `Boost.ASIO`, the library, that allows you to work with asynchrony and implement basic server functions.
Server program can accept signals by clients and interact with them. Server can store some data, that comes from clients, it has a place for all user-authentications data(authentication data base), it conected with arduino by USB and send him relevant server repository content only for user(client), that send a request for the server. 

Server work stages:   
1. Start Server;
2. Catch requests from Clients;
3. Encryption part(changing password by hash-function);
4. Memoryse client data for authentification;
    loop start
5. Catch files from client;
6. Store files;
7. Update arduino display(send to it relevant server content);
8. go to loop start to work in cycle.

Server defenition, as well, processing critical errors, those can arise.


### The Second part: Client

Client part of project has simular functions like a server, but it only send,as we can say its functios work conversaly. It sends requests and files. Requests consists of arduino-requests and server-requests. Arduino-request allows client to check repository data, size of it, and how much data is free, and can be stored, as well here can be add function for changing a password. Server-requests allows client, at first registration, next, user has opportunity to send data to server, and take it from it. 

Client work stages:
1. Start Client application;
2. Send to server any requests;
3. Take files from server;

Like server, this part have basicly the same crtical errors processing.

### The Third part: Arduino integration

Arduino takes commands only from server, and operate it. In real time arduino get current information about state of server and server-repositories for only one user, that send this requests. 
Arduino circuit has a LCD-dasplay I2C, directly Arduino board, joystick, dynamic and some LEDs.
Display allows you to check any info, joystick is for work with display info, travel trough it, dynamic and LEDs are responsible for notification about current connection state.


### Conclusion

There are a server that catch users requests and display, on it you can see some information. All of it work on `Boost.ASIO`. There is using arduino and some modules for it. 

# Development stages

This part of documentation consits of development stages and describe all of it. Whole programm development has 3 main parts, like a structure of it: Server, Client, Electronics. Each stage of development means what kind of 

1. Creating a server application;
    * Class for server;
    * Methods for make a connection with client;
    * Methods for registration users in net cloud-client;
    * Methods for manipulate with user data(password and login)
    * Creating data base class to communicate with DB storage
        *  Encrypting password by hash function;
        *  Store hash of password;
        *  Compare inputed password with stored hash in DB of users;
        *  Processing errors with DB(no user in db, wrong password);
        *  Add and delete users data(may be a user-admin, that has root to operate with whole users data);
    * Catching diferent requests from users and send necessary of those, to electronic part(Arduino) by USB, and another use for operate with authentification and data sending;
    * Some additional functions.


2. Creating a client application;
    * Class for client;
    * Method those asking for authentification user-data;
    * Methods for make a connection with server;
    * Methods for send requests for confirm kind of it(arduino-requests and server-requests);
    * Methods for files sending;
    * Methods for take files from server to user repository;


3. Creating a electronic device;
    * Develop a code for modules communication with arduino;
        * Class for work with display;
        * Methods for catch information from server;
        * Methods for show this info on display;
    * Part of code for check current server updates;
        * Class for catching server requests;
        * Class for catching server information;
        * Method for sending to server current state of display info(which user check info);

