

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
  - [Contributing](#contributing)
  - [License](#license)

## Overview
This project models a Pokémon-like simulation environment, allowing the user to interact with Pokémon data, battle characters, and move through different environments. It includes implementations for handling character movements, simulating battles, and parsing data from external CSV files.

## Features
- **Character Movement**: Supports different types of characters (e.g., Hiker, Rival, Swimmer) with varying movement abilities.
- **Battle Simulation**: Simple battle mechanics triggered when characters meet.
- **Heap Data Structures**: Utilizes a custom heap for managing character movement and actions.
- **Data Parsing**: Parses CSV files to load Pokémon and move data.
- **Pathfinding**: Implements pathfinding for characters to navigate the game map efficiently.
  
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
   git clone https://github.com/your-repository/project-name
   ```
2. Navigate to the project directory and compile the source files:
   ```bash
   g++ character.cpp data.cpp poke327.cpp io.cpp heap.c -o pokesim
   ```
3. Run the compiled program:
   ```bash
   ./pokesim
   ```

## Usage
1. The simulation will start by generating a map and placing characters at random positions.
2. You can interact with the simulation through the console by entering commands to move the player or simulate battles.
3. The program will read Pokémon and moves data from CSV files (ensure these files are available in the working directory).

### Sample Commands:
- Move player: Use directional commands to move.
- Start battle: Automatically triggered when a character encounters a player.

## Contributing
Feel free to fork this project and submit pull requests. Contributions in terms of adding new features, improving algorithms, or fixing bugs are welcome.

## License
This project is licensed under the MIT License.
>>>>>>> 4e016a3 (Pushing the pokemon project to my github)
