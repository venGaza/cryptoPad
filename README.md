# CryptoPad

In cryptography, the one-time pad (OTP) is an encryption technique that cannot be cracked, but requires the use of a one-time pre-shared key the same size as, or longer than, the message being sent ([Wikipedia](https://en.wikipedia.org/wiki/One-time_pad)). This program aims to emulate this cryptography technique and allows users to generate keys, encrypt, and decrypt messages. The program is currently designed to run on just one server, but can be modified to run across multiple servers.   

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See deployment for notes on how to deploy the project on a live system.

### Prerequisites

What things you need to install the software and how to install them

```
GNU Compiler (Recommend to install with package manager such as Homebrew)
```

### Installing (Bash Terminal)

In the command line within your desired directory, run the following command to download the program:

```
#Downloads the project directory to the current working directory
git clone https://github.com/venGaza/cryptoPad
```

Then run the following command to build the project:

```
#Build
compileCryptoPad
```

### Generate a key 

Run the following command to generate a key for the program to use for encryption and decryption:

```
keygen <numberOfCharactersDesired>  > <desiredKeyFileName>

#example

keygen 20 > keygen1

ls

keygen1.txt

**Make sure to generate key that has longer character length than the message that will be encrypted
```

### Start the daemons 

To start the server daemons in the background run the following code:

```
encrypt-daemon <portNum> &
decrypt-daemon <portNum> &

**portNum = the port number the daemon will listen for requests
```

### Encrypt a message

```
encrypt-client <encryptionKeyFile> <portNumberOfDaemon> > <outputFileName>

#example

encrypt-client plain1 key70000 3000 > cipher1
```

### Decrypt a message

```
decrypt-client <encryptionKeyFile> <portNumberOfClient> > <outputFileName>

#example

decrypt-client cipher1 key70000 3000 > plainRead1
```

### Stopping servers

Once you are done with the program, you can use the following command to stop the servers:

```
killall -q -u $USER encrypt-daemon* decrypt-daemon
```

## Built With

* [Visual Studio Code](https://code.visualstudio.com/) - Visual Studio Code is a code editor redefined and optimized for building and debugging modern web and cloud applications.
* [Vim](https://www.vim.org/) - Vim is a highly configurable text editor for efficiently creating and changing any kind of text.

## Authors

* **Dustin Jones** - [venGaza](https://github.com/venGaza)

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments

* Oregon State University
* [Beej's Guide to Socket Programming](https://beej.us/guide/bgnet/html/multi/index.html) 
* [Wikipedia](https://en.wikipedia.org/wiki/One-time_pad)