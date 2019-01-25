/*
 * Description: This program pairs with the encrypt-daemon program and functions as the client
 * in the client / server communication model. This program takes a checks validity of the
 * command input and then sends it over to the file to be encrypted. Once it gets the
 * message back from the server, it prints the encrypted message to stdout.
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
void readFileData(char* fileName, char* fileData, int type);
void validateCharacters(char* fileData);
int createSocket(int portNum);
int createListenSocket(int portNum);

/*Main Function*/
int main(int argc, const char * argv[]) {
    int charSend, charRead, portNum, dataPortNum;
    char messageData[100000];
    memset(messageData, '\0', 100000);
    char keyData[100000];
    memset(keyData, '\0', 100000);
    char combinedData[200000];
    memset(combinedData, '\0', 200000);
    char command[100];
    memset(command, '\0', 100);
    char buffer[100000];
    memset(buffer, '\0', 100000);
    srand((unsigned) time(0));
    
    //Validate number of arguments
    if(argc < 4) {
        error("Incorrect command (Format: otp_enc message key port)");
    }
    
    //Transfer arguments into named variables
    char messageName[1000];
    memset(messageName, '\0', 1000);
    strcpy(messageName, argv[1]);
    
    char keyName[1000];
    memset(keyName, '\0', 1000);
    strcpy(keyName, argv[2]);
    
    portNum = atoi(argv[3]);
    if (portNum < 1024 || portNum > 65535) {                //Validate Port Number
        error("Invalid port number");
    }
    if (portNum != 65535) {
        dataPortNum = portNum + (rand()%3000);
    } else {
        dataPortNum = portNum - (rand()%500);
    }
    
    //Read data from message and key files
    readFileData(messageName, messageData, 1);
    readFileData(keyName, keyData, 2);
    
    //Compare the length of the message and key
    if (strlen(messageData) > strlen(keyData)) {
        error("Hull breach: key too short");
    }
    
    //Validate characters in the message and key
    validateCharacters(messageData);
    validateCharacters(keyData);
    
    //Setup the command socket and connect to the daemon
    int commandSocketFD = createSocket(portNum);
    sprintf(command, "%d,enc", dataPortNum);
    charSend = send(commandSocketFD, command, strlen(command), 0);
    if(charSend < 0) {
        error("Hull breach: error in sending command");
    }
    
    sleep(1); //Wait for server to set up data port
    
    //Setup the data socket and transfer message and key data
    int dataSocketFD = createSocket(dataPortNum);
    sprintf(combinedData, "%s,%s", messageData, keyData);
    charSend = send(dataSocketFD, combinedData, strlen(combinedData), 0);
    if(charSend < 0) {
        error("Hull breach: error in sending data");
    }
    
    //Wait for encrypted message from server then output once received
    charRead = recv(dataSocketFD, buffer, 99999, 0);
    if (charRead < 0) {
        error("Error: read()");
    }
    
    //Add newline back on to encrypted message and print it out
    //buffer[strlen(buffer)] = '\n';
    printf("%s\n", buffer);
    
    //Close the sockets
    close(dataSocketFD);
    close(commandSocketFD);
    
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
 * Function: readFileData
 * Usage: readFileData(name, destination, type)
 *  -------------------------
 *  This function attempts to open the specified file name and read its contents into the
 *  given destination variable. If it is successful, it also strips off the ending newline
 *  character so that it can be sent off for encrytption/decryption. If it is unable to open
 *  or read the file, it prints out an error message and exits the program.
 */
void readFileData(char* fileName, char* fileData, int type) {
    //Open the file
    FILE* file;
    file = fopen(fileName, "r");
    if (file == NULL) { //Error occurred trying to open file
        if (type == 1) {
            error("Hull breach: could not open message file");
        } else {
            error("Hull breach: could not open key file");
        }
    }
    
    //Get file data
    if (fgets(fileData, 99999, file) == NULL) { //Error reading file
        if (type == 1) {
            error("Hull breach: could not read message data");
        } else {
            error("Hull breach: could not read key data");
        }
    }
    
    //Strip off newline character
    fileData[strcspn(fileData, "\n")] = '\0';
    
    //Close the file
    fclose(file);
}

/*
 * Function: validateCharacters
 * Usage: validateCharacters(fileData)
 *  -------------------------
 *  This function loops through a file's data dn checks to make sure all characters are
 *  valid (only uppercase letters and space allowed).
 */
void validateCharacters(char* fileData) {
    int i = 0, j = 0;
    char letters[] = " ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    
    //Check each character of given file
    for (i = 0; i < strlen(fileData); i++) {
        while (fileData[i] != letters[j]) {       //Check for match
            j++;
            
            //End of letters reached
            if (j == 27) {
                error("Message or key contains invalid characters");
            }
        }
        
        //Reset for next character
        j = 0;
    }
    
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
    int socketFD;
    struct sockaddr_in serverAddress;
    struct hostent* serverHostInfo;
    
    //Fill in address struct for the server
    memset((char*)&serverAddress, '\0', sizeof(serverAddress)); //Initialize address struct
    serverAddress.sin_family = AF_INET; //Set type to TCP
    serverAddress.sin_port = htons(portNum); //Convert portNum to network byte order
    serverAddress.sin_addr.s_addr = INADDR_ANY; //Accept from any IP address
    serverHostInfo = gethostbyname("localhost");
    
    if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); } //Validate return
    
    memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address
    
    //Setup the socket
    socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFD < 0) {
        error("Hull Breach: socket()");
    }
    
    // Connect to server
    if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
    error("CLIENT: ERROR connecting to server");
    
    return socketFD;
}
