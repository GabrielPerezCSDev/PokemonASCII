// prints.h
#ifndef PRINTS_H
#define PRINTS_H

#include "data.h"
#include <algorithm>

class PokeStats{
public:
    // Constructor to initialize the PokeStats object with an identifier and a rating
    PokeStats(const std::string& identifier, int rating, int iv)
        : statIdentifier(identifier), statRating(rating), IV(iv) {}

    // Getter for the stat identifier
    const std::string& getStatIdentifier() const {
        return statIdentifier;
    }

    // Getter for the stat rating
    int getStatRating() const {
        return statRating;
    }

    // Setter for the stat rating
    void setStatRating(int newRating) {
        statRating = newRating;
    }

    int getIV() const {
        return IV;
    }

    void setIV(int iv){
        IV = iv;
    }

private:
    std::string statIdentifier; // The identifier for the stat (e.g., "HP", "Attack")
    int statRating;
    int IV;

};


class PokeData{
    private: 
        int level;
        Pokemon poke;
        std::vector<Moves> movesList;
        std::vector<PokeStats> statsList;
        bool shiny;
    public:
    // Getter for level
    int getLevel() const {
        return level;
    }

    // Getter for Pokemon
    const Pokemon& getPokemon() const {
        return poke;
    }

    // Getter for the entire list of Moves
    const std::vector<Moves>& getMovesList() const {
        return movesList;
    }

    // Getter for an individual Move by index
    // Note: You should check the index bounds before calling this function
    const Moves& getMove(size_t index) const {
        if (index >= movesList.size()) {
            throw std::out_of_range("Index out of range");
        }
        return movesList[index];
    }

   // Getter for the entire list of Stats
    const std::vector<PokeStats>& getStatsList() const {
        return statsList;
    }

    // Non-const version of getStatsList
    std::vector<PokeStats>& getStatsList() {
        return statsList;
    }

    // Setter for level
    void setLevel(int newLevel) {
        level = newLevel;
    }

    // Setter for Pokemon
    void setPokemon(const Pokemon& newPoke) {
        poke = newPoke;
    }

    // Setter for the entire list of Moves
    void setMovesList(const std::vector<Moves>& newMovesList) {
        movesList = newMovesList;
    }

    // Add a Move to the list of Moves
    void addMove(const Moves& move) {
        movesList.push_back(move);
    }

    // Remove a Move from the list by index
    void removeMove(size_t index) {
        if (index < movesList.size()) {
            movesList.erase(movesList.begin() + index);
        } else {
            throw std::out_of_range("Index out of range");
        }
    }

    // Add a Stat to the list of Stats
    void addStat(const PokeStats& stat) {
        statsList.push_back(stat);
        // If adding HP, also add maxHP with the same value
        if (stat.getStatIdentifier() == "hp") {
            // Check if maxHP already exists, if not, add it
            auto maxHPit = std::find_if(statsList.begin(), statsList.end(),
                                        [](const PokeStats& s) { return s.getStatIdentifier() == "maxHP"; });
            if (maxHPit == statsList.end()) {
                statsList.push_back(PokeStats("maxHP", stat.getStatRating(), 0));
            }
        }
    }

    // Remove a Stat from the list by index
    void removeStat(size_t index) {
        if (index < statsList.size()) {
            statsList.erase(statsList.begin() + index);
        } else {
            throw std::out_of_range("Index out of range");
        }
    }

    void printPokeData(){
        printf("\nPokeData: \n");
        printf("Level: %d", getLevel());
        
        printf("\nName: %s\n",  getPokemon().getIdentifier().c_str());
        // Check if there are moves to print
    if (getMovesList().empty()) {
        printf("No moves learned.\n");
    } else {
        printf("Moves:\n");
        for (const auto& move : getMovesList()) {
            printf(" - %s \n",
                   move.getIdentifier().c_str());
        }
    }

    // Check if there are stats to print
    if (getStatsList().empty()) {
        printf("No stats available.\n");
    } else {
        printf("Stats:\n");
        for (const auto& stat : getStatsList()) {
            printf(" - %s: %d\n", stat.getStatIdentifier().c_str(), stat.getStatRating());
        }
    }
    }

    bool getShiny() const{
        return shiny;
    }

    void setShiny(bool isShiny){
        shiny = isShiny;
    }

    // Method to modify a stat by identifier
    void modifyStat(const std::string& identifier, int newRating) {
        auto it = std::find_if(statsList.begin(), statsList.end(),
                               [&identifier](const PokeStats& stat) { return stat.getStatIdentifier() == identifier; });
        if (it != statsList.end()) {
            it->setStatRating(newRating);
        }
    }

};




#endif