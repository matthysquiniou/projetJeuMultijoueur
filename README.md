## Installation and Solution Generation

1. Go to the `bin/` folder.  
2. Run `make.bat`.  
3. The script will generate an `ide/` folder containing `[SOLUTION_NAME].sln`.  
4. Open the Visual Studio solution.  

# Network Project – Multiplayer Game (UDP)

## Description

This project is a network-based project developed as part of a school assignment.  
It was created to learn and practice:

- Network programming
- The use of **threads**
- Communication using the **UDP** protocol

## General Overview

The project is composed of two main parts:

- **A server**
- **One or more clients**

### Server

The server must be started first.  
It allows clients to connect and communicate with each other.

### Client

Once the client is launched, it will ask the user to enter:

1. An **IPv4 address**
2. A **port**
3. A **name** to play

After entering this information, the player can join the game.

## Controls

- **Z Q S D**: move
- **Left click**: shoot
- **Right click**: follow the last projectile

## Notes

This project was developed for educational purposes and mainly demonstrates networking and multithreading concepts.
