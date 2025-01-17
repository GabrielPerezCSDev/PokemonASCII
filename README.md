

# Pokémon Simulation Project

A sophisticated Pokémon simulation built in C/C++ that features procedural map generation, advanced pathfinding algorithms, and dynamic difficulty scaling. The project implements custom data structures and algorithms to create an evolving game world with over 400 unique, procedurally generated maps.

## Overview

This Pokémon simulation creates an expansive game world where:
- Maps are procedurally generated based on player movement
- Difficulty scales with Manhattan distance from origin
- NPCs utilize advanced pathfinding algorithms
- Custom data structures manage game mechanics
- Battle systems integrate with character movement

## Features

### World Generation
- **Procedural Map Creation**: Dynamic generation of over 400 unique maps
- **Difficulty Scaling**: Map complexity increases with distance from origin
- **Terrain Variety**: Multiple terrain types affecting movement and accessibility
- **Persistent World**: Generated maps are stored in a matrix for consistent backtracking

### Character System
- **Multiple NPC Types**:
  - Hikers with terrain-aware pathfinding
  - Rivals with aggressive pursuit behavior
  - Swimmers confined to water routes
- **Custom Movement Patterns**: Each NPC type implements unique movement strategies
- **Dijkstra's Algorithm**: Efficient pathfinding for NPC movement

### Technical Implementation
- **Custom Heap Structure**: 
  - Priority-based movement management
  - Efficient turn order calculation
  - Memory-optimized data storage
- **Data Management**:
  - CSV parsing for Pokémon data
  - Dynamic object creation
  - Efficient data retrieval systems

### Battle Mechanics
- **Encounter System**: Proximity-based battle initiation
- **Type Advantages**: Implementation of Pokémon type system
- **Move Sets**: Custom move implementation for each Pokémon
- **Battle Resolution**: Turn-based combat system

## Architecture

### Core Components

### Data Structures
- **Custom Heap**: Manages movement priority and turn order
- **Character Matrix**: Tracks entity positions and movements
- **Map Storage**: Efficient storage of generated maps
- **Pokemon Database**: Structured storage of Pokemon data

## Getting Started

### Prerequisites
- C++ Compiler (GCC 7.0+)
- Make build system

### Installation

1. Clone the repository:
```bash
git clone https://github.com/YourUsername/Pokemon-Simulation.git
cd Pokemon-Simulation
```

2. Build the project:
```bash
make clean
make
```

3. Run the simulation:
```bash
./pokemon_sim
```

## Game Controls

### Movement
- **↑**: Move North
- **↓**: Move South
- **←**: Move West
- **→**: Move East
- **q**: Quit game
- **f**: Fly to different coordinates

### Battle System
- Battles initiate automatically on collision
- Follow on-screen prompts for battle options
- Use number keys to select moves

## Development Status

### Current Features
- Full map generation system
- Basic NPC movement patterns
- Battle system implementation
- Character pathfinding
- Data parsing and management

### Planned Enhancements
- Improved map generation algorithms
- Save/Load functionality

## Technical Details

### Map Generation
- Uses seeding natural growth algorithms for terrain generation
- Implements connectivity checks for path validity
- Maintains consistency across adjacent maps
- Scales difficulty based on distance from origin map metrics

### Pathfinding Implementation
- Dijkstra's algorithm for NPC movement
- Custom weight system for terrain types
- Optimized path recalculation
- Collision avoidance system