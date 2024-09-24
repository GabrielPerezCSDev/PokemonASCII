

# Pokémon Simulation Project

This project is a Pokémon simulation that implements various aspects of game mechanics, including characters, movement, battle systems, and data manipulation. It uses a combination of custom data structures, heap algorithms, and CSV file parsing to model and simulate the game environment.

## Table of Contents
- [Pokémon Simulation Project](#pokémon-simulation-project)
  - [Table of Contents](#table-of-contents)
  - [Overview](#overview)
  - [Features](#features)
  - [File Structure](#file-structure)
    - [Key source files:](#key-source-files)
  - [Installation](#installation)
    - [Prerequisites](#prerequisites)
    - [Build Instructions](#build-instructions)
  - [Usage](#usage)
    - [Sample Commands:](#sample-commands)

## Overview
This project models a Pokémon-like simulation environment, allowing the user to interact with Pokémon data, battle characters, and move through different environments. It includes implementations for handling character movements, simulating battles, and parsing data from external CSV files. The game will randomly generate maps by an algorithm I created. 

## Features
- **Character Movement**: Supports different types of characters (e.g., Hiker, Rival, Swimmer) with varying movement abilities.
- **Battle Simulation**: Simple battle mechanics triggered when characters meet.
- **Heap Data Structures**: Utilizes a custom heap for managing character movement and actions.
- **Data Parsing**: Parses CSV files to load Pokémon and move data.
- **Pathfinding**: Implements djikstra's algorithm pathfinding for characters to navigate the game map efficiently.
- **Map-Generation**: Over 400 maps are proceduraly generated one-by-one as the playe moves into a new map (with maps saved in a matrix once created for backtracking) so that every level and playtrhough has a unique feel.
- **Dificulty**: Maps get increasingly difficult as a function of the Mannhattan distance from the origin.
  
## File Structure
### Key source files:

1. **`character.cpp`** - Manages character movement and behavior, including the movement of NPCs like Hikers, Rivals, and Swimmers. It also implements different strategies for moving and pathfinding.

2. **`data.cpp`** - Handles loading and processing data from CSV files for objects like Pokémon and moves. This file implements methods for reading the CSV files, creating Pokémon objects, and printing Pokémon and move data.

3. **`heap.c`** - Implements a custom heap data structure used to prioritize character movements and manage game mechanics such as turn order.

4. **`poke327.cpp`** - Implements core game logic, including battle systems, world generation, and interaction between different characters and the environment.

5. **`io.cpp`** - Handles input and output for the game. It processes user commands and updates the display.

## Installation
### Prerequisites
- A C++ compiler (e.g., `g++`).
- A C compiler for compiling the heap structure.
- Make sure you have the necessary libraries for file I/O and heap memory management.

### Build Instructions
1. Clone the repository:
   ```bash
   git clone https://github.com/PokemonASII
   ```
2. Navigate to the project directory and use the make file:
   ```bash
   make
   ```
3. Run the compiled program:
   ```bash
   ./poke_main
   ```

## Usage
1. The simulation will start by generating a map and placing characters at random positions.
2. You can interact with the simulation through the console by entering commands to move the player or simulate battles.
3. The program will read Pokémon and moves data from CSV files (ensure these files are available in the working directory).
4. The game continues until all of your pokemon are defeated
5. See how far you can survuve and explore all the maps you can!

### Sample Commands:
- Move player: Use directional commands to move.
- Start battle: Automatically triggered when a character encounters a player.
- Change map: Move again at the borde of the map.
