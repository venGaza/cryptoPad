/*
 * Description: This program pairs with the decrypt-client program and functions as the server
 * in the client / server communication model. This program takes a plaintext file and key
 * from the client and encrypts the file using the provided key. The file is then returned
 * to the client.
 * **SOURCE: Much of the code is based from Beej's guide to Socket Programming at https://beej.us/guide/bgnet/html/multi/clientserver.html**
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <dirent.h>

/*Function Prototypes*/
void error(const char* message);
int createSocket(int portNum);
void readCommand(int establishedConnectionFD, int* dataPort, char* dataCommand, char* dataFile, char* buffer);
void sendDirectory(char* buffer, int dataSocketFD);
void sendReqFile(char* buffer, char* dataFile, int dataSocketFD);
void recMessage(char* buffer, int establishedConndectionFD, char* message, char* key);
void encryptMessage(char* key, char* message, char* encMessage);
void decryptMessage(char* key, char* message, char* decMessage);
void sendMessage(char* message, int establishedConnectionFD);

/*Main Function*/
int main(int argc, const char * argv[]) {
    int listenSocketFD, listenDataSocketFD, establishedConnectionFD, dataSocketFD, portNumber;
    int dataPort = 3333;
    socklen_t sizeOfClientInfo, sizeOfClientInfo2;
    char buffer[200000];
    memset(buffer, '\0', 200000);
    char dataCommand[1024];
    memset(dataCommand, '\0', 1024);
    char dataFile[1024];
    memset(dataFile, '\0', 1024);
    struct sockaddr_in clientAddress, clientAddress2;
    char key[100000];
    memset(key, '\0', 100000);
    char message[100000];
    memset(message, '\0', 100000);
    char decMessage[100000];
    memset(decMessage, '\0', 100000);
    
    //Check if port number was given as an argument
    if (argc < 2) {
        fprintf(stderr,"Usage: %s port\n", argv[0]);
        exit(1);
    } else {
        portNumber = atoi(argv[1]);  //Get port number from command argument
    }
    
    listenSocketFD = createSocket(portNumber); //Create listening socket
    
    //Keep server in infinite accept loop
    while (1) {
        //Accept connection (block while waiting for one to connect)
        sizeOfClientInfo = sizeof(clientAddress); //Gets client address size
        establishedConnectionFD = accept(listenSocketFD, (struct sockaddr*)&clientAddress, &sizeOfClientInfo); //Accepts connection
        if (establishedConnectionFD < 0) {
            error("Hull breach: accept()");
        }
        printf("Server: Connected to client at port %d\n", ntohs(clientAddress.sin_port));
        
        int spawnPID = fork(); //Spawn new process for data handling
        switch(spawnPID) {
            case -1:                                //Error
                error("Hull breach: fork()");
                exit(1);
                
            case 0:                                 //Child handling data request
                readCommand(establishedConnectionFD, &dataPort, dataCommand, dataFile, buffer);
                listenDataSocketFD = createSocket(dataPort);
                dataSocketFD = accept(listenDataSocketFD, (struct sockaddr*)&clientAddress2, &sizeOfClientInfo2);
                
                //Execute specified command or return error if uknown
                if ((strstr(dataCommand, "dec")) != NULL) {              //Decrypt file
                    recMessage(buffer, dataSocketFD, message, key);        //Get message/key
                    decryptMessage(key, message, decMessage);              //Encrypt Message
                    sendMessage(decMessage, dataSocketFD);                 //Send encryption
                } else if ((strstr(dataCommand, "enc")) != NULL) {       //Wrong server
                    printf("otp_dec connecting to wrong server");
                } else {                                                //Error
                    printf("Data Command is: %s\n", dataCommand);
                    fflush(stdout);
                    error("Error reading command\n");
                }
                
                close(dataSocketFD);
                break;
                
            default:;                                //Parent
                //Originally planned to have parent intialize values but realized it has sep copy
        }
        
        
        close(establishedConnectionFD); //Close the socket
    }
    
    close(listenSocketFD); //Close the listening socket
    
    return 0;
}

/*
 * Function: error
 * Usage: error(message)
 *  -------------------------
 *  This function prints takes a string parameter which contains a message and outputs
 *  it as an error. It then exits the program with a status code of 1.
 *  SOURCE: Code based off CS344-400 lecture material 4.3
 */
void error(const char* message) {
    perror(message);
    exit(1);
}

/*
 * Function: createSocket
 * Usage: createSocket(12345)
 *  -------------------------
 *  This function creates a listening socket on the server and returns this socket
 *  back to the calling function. It takes one parameter which is the desired port
 *  number.
 */
int createSocket(int portNum) {
    int listenSocketFD;
    struct sockaddr_in serverAddress;
    
    //Fill in address struct for the server
    memset((char*)&serverAddress, '\0', sizeof(serverAddress)); //Initialize address struct
    serverAddress.sin_family = AF_INET; //Set type to TCP
    serverAddress.sin_port = htons(portNum); //Convert portNum to network byte order
    serverAddress.sin_addr.s_addr = INADDR_ANY; //Accept from any IP address
    
    //Setup the socket
    listenSocketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocketFD < 0) {
        error("Hull Breach: socket()");
    }
    
    //Bind the socket
    if (bind(listenSocketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
       	printf("%s ", portNum);
	 error("Hull breach: bind()");
    }
    listen(listenSocketFD, 5); //Socket turned on and listens for up to 5 connections
    
    return listenSocketFD;
}

/*
 * Function: readCommand
 * Usage: readCommand(socket, dataPort, command, buffer)
 *  -------------------------
 *  This function reads in a command line from a client. The format of the request
 *  must be passed in the following format "portNumber , command." This function
 *  makes use of the tokenizer to parse the command and determine the port number
 *  to send data back on as well as what data needs to be sent.
 */
void readCommand(int establishedConnectionFD, int* dataPort, char* dataCommand, char* dataFile, char* buffer) {
    int charRead;
    memset(buffer, '\0', 200000);
    char* token;
    
    //Read in command from client
    charRead = recv(establishedConnectionFD, buffer, 199999, 0);
    if (charRead < 0) {
        error("Error: read()");
    }
    
    //Extract port number
    token = strtok(buffer, ",");
    *dataPort = atoi(token);
    
    //Extract command
    token = strtok(NULL, ",");
    strcpy(dataCommand, token);
    
    //Extract file name if the file transfer command given
    if (strcmp(dataCommand, "-g") == 0) {
        token = strtok(NULL, ",");
        strcpy(dataFile, token);
    }
}

/*
 * Function: recMessage
 * Usage: recMessage(buffer, scoket, message)
 *  -------------------------
 *  This function reads in data from a connected socket and stores it into the
 *  character string passed as a parameter. If no data is read, it will return an error
 *  message and exit.
 */
void recMessage(char* buffer, int establishedConndectionFD, char* message, char* key) {
    int charRead;
    memset(buffer, '\0', 200000);
    char* token;
    
    //Read in message from client
    charRead = recv(establishedConndectionFD, buffer, 199999, 0);
    if (charRead < 0) {
        error("Error: read()");
    }
    
    //Extract message
    token = strtok(buffer, ",");
    strcpy(message, token);
    
    //Extract key
    token = strtok(NULL, ",");
    strcpy(key, token);
    
}

/*
 * Function: encryptMessage
 * Usage: encryptMessage(key, message, encryptedMessage)
 *  -------------------------
 *  This function encrypts a message using a designated key. It first converts the key and
 *  message into integer values before adding them together and taking the modulo. The
 *  integers are then converted back to characters to represent the encrypted message.
 */
void encryptMessage(char* key, char* message, char* encMessage) {
    int numMessage[99999] = {0};
    int numMessageCount = 0;
    int numKey[99999] = {0};
    int numKeyCount = 0;
    int encrypted[99999] = {0};
    char letters[] = " ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int i = 0, j = 0;
    
    //Convert key into integers
    for (i = 0; i < strlen(key); i++) {
        while (key[i] != letters[j]) {
            j++;
        }
        numKey[i] = j;
        numKeyCount++;
        j = 0;
    }
    
    //Convert original message into integers
    for (i = 0; i < strlen(message); i++) {
        while (message[i] != letters[j]) {
            j++;
        }
        numMessage[i] = j;
        numMessageCount++;
        j = 0;
    }
    
    //Combine key and message into encrypted message
    for (i = 0; i < numMessageCount; i++) {
        encrypted[i] = ( numMessage[i] + numKey[i] ) % 27;
    }
    
    //Convert encrypted message from integer form back to characters
    for (i = 0; i < numMessageCount; i++) {
        encMessage[i] = letters[encrypted[i]];
    }
}

/*
 * Function: decryptMessage
 * Usage: decryptMessage(key, message, encryptedMessage)
 *  -------------------------
 *  This function decrypts a message using a designated key. It first converts the key and
 *  message into integer values before adding them together and taking the modulo. The
 *  integers are then converted back to characters to represent the encrypted message.
 */
void decryptMessage(char* key, char* message, char* decMessage) {
    int numMessage[99999] = {0};
    int numMessageCount = 0;
    int numKey[99999] = {0};
    int numKeyCount = 0;
    int decrypted[99999] = {0};
    char letters[] = " ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int i = 0, j = 0;
    
    //Convert key into integers
    for (i = 0; i < strlen(key); i++) {
        while (key[i] != letters[j]) {
            j++;
        }
        numKey[i] = j;
        numKeyCount++;
        j = 0;
    }
    
    //Convert original message into integers
    for (i = 0; i < strlen(message); i++) {
        while (message[i] != letters[j]) {
            j++;
        }
        numMessage[i] = j;
        numMessageCount++;
        j = 0;
    }
    
    //Combine key and message into decrypted message
    for (i = 0; i < numMessageCount; i++) {
        decrypted[i] = ( numMessage[i] - numKey[i] );
	if (decrypted[i] < 0) {
		decrypted[i] += 27;
	}
    }
    
    //Convert decrypted message from integer form back to characters
    for (i = 0; i < numMessageCount; i++) {
        decMessage[i] = letters[decrypted[i]];
    }
}

void sendMessage(char* message, int establishedConnectionFD) {
    int charSend;
    charSend = send(establishedConnectionFD, message, strlen(message), 0);
    if(charSend < 0) {
        error("Hull breach: error in sending encrypted message");
    }
}
