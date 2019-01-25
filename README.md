# CryptoPad

This is a CLI RNG fantasy combat program built in C++ . This game allows the player to do tournament style battle with fantasy characters taking turns attacking the other player. Players can even team up and build a team of up to 10 players to challenge an opponent. The player will be able to choose from the following characters: Barbarian, Vampire, Blue Men, Medusa, and the feared Harry Potter! Each character posseses a unique special ability to allow them to conquer their opponent. Have fun and challenge a few of your friends to this thrilling combat adventure!   

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

### Playing the game

To play the game run the following command:

```
bin/FantasyGame
```

The screen should display the following information if the installation was successful:

```
Welcome to the Fantasy Combat Game! This game allows the player to do tournament style battle with fantasy characters. The player can choose between the following: Barbarian, Vampire, Blue Men, Medusa, and the feared Harry Potter! Each character posseses a unique special ability to allow them to conquer their opponent. Enjoy!!! 

              Menu
---------------------------------
1. Play Game
2. Exit
```

You will navigate throught the world using the numbered options. In this case you would enter 1 + enter to start the game. 

### Errors compiling the game

Try running the following command if there was an error compiling the game:

```
make remake
```

## Uninstalling

If you wish to remove the binary and objects, run the following command:

```
make cleaner
```

or for a full uninstall

```
cd ..; rm -rf starwars
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