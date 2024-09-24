// Data.h
#ifndef DATA_H
#define DATA_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>

// Generic CSV Reader
class CSVReader {
public:
    static std::vector<std::vector<std::string>> readCSV(const std::string& filename);
};



// Base class for all Data objects
class DataObject {
protected:
    

public:
    // Virtual method for printing object data
    virtual void printObject() const = 0;

    // Virtual destructor to ensure proper cleanup
    virtual ~DataObject() {}

      
};

// Derived class for Pokemon CSV
class Pokemon : public DataObject {
private:
    int id;
    std::string identifier;
    int speciesId;
    int height;
    int weight;
    int baseExp;
    int order;
    int isDefault;
public:
    Pokemon(int id, std::string identifier,int speciesId,int height,int weight, int baseExp, int order, int isDefault);
    Pokemon() {
        // Initialize members to default values if necessary
    }
    // Override the printObject method
    void printObject() const override;
    // Getter methods for each field
    int getId() const { return id; }
    std::string getIdentifier() const { return identifier; }
    int getSpeciesId() const { return speciesId; }
    int getHeight() const { return height; }
    int getWeight() const { return weight; }
    int getBaseExp() const { return baseExp; }
    int getOrder() const { return order; }
    int getIsDefault() const { return isDefault; }
};


class Moves : public DataObject {
private:
    int id;
    std::string identifier;
    int generationId;
    int typeId;
    int power;
    int pp;
    int accuracy;
    int priority;
    int targetId;
    int damageClass;
    int effectId;
    int effectChance;
    int contestTypeId;
    int contestEffectId;
    int superContestEffectId;
public:
    Moves(int id, std::string identifier, int generationId, int typeId, int power, int pp, int accuracy, int priority, int targetId, int damageClass, int effectId, int effectChance, int contestTypeId, int contestEffectId, int superContestEffectId);
    Moves(){};
    // Override the printObject method
    void printObject() const override;

    // Getter methods
    int getId() const { return id; }
    std::string getIdentifier() const { return identifier; }
    int getGenerationId() const { return generationId; }
    int getTypeId() const { return typeId; }
    int getPower() const { return power; }
    int getPp() const { return pp; }
    int getAccuracy() const { return accuracy; }
    int getPriority() const { return priority; }
    int getTargetId() const { return targetId; }
    int getDamageClass() const { return damageClass; }
    int getEffectId() const { return effectId; }
    int getEffectChance() const { return effectChance; }
    int getContestTypeId() const { return contestTypeId; }
    int getContestEffectId() const { return contestEffectId; }
    int getSuperContestEffectId() const { return superContestEffectId; }
};

class PokemonMoves : public DataObject {
private:
    int pokemonId;
    int versionGroupId;
    int moveId;
    int pokemonMoveMethodId;
    int level;
    int order;

public:
    // Public constructor with an initializer list
    PokemonMoves(int pokemonId, int versionGroupId, int moveId, int pokemonMoveMethodId, int level, int order);
    PokemonMoves(){};
    // Override the printObject method
    void printObject() const override;

    // Getter methods
    int getPokemonId() const { return pokemonId; }
    int getVersionGroupId() const { return versionGroupId; }
    int getMoveId() const { return moveId; }
    int getPokemonMoveMethodId() const { return pokemonMoveMethodId; }
    int getLevel() const { return level; }
    int getOrder() const { return order; }
};

// Derived class for Pokemon CSV
class PokemonSpecies : public DataObject {
private:
    int id;
    std::string identifier;
    int generationId;
    int evolvesFromSpeciesId;
    int evolutionChainId;
    int colorId;
    int shapeId;
    int habitatId;
    int genderRate;
    int captureRate;
    int baseHappiness;
    int isBaby;
    int hatchCounter;
    int hasGenderDifferences;
    int growthRateId;
    int formsSwitchable;
    int isLegendary;
    int isMythical;
    int order;
    int conquestOrder;

public:
    // Public constructor with an initializer list
    PokemonSpecies(int id, std::string identifier, int generationId, int evolvesFromSpeciesId, int evolutionChainId, int colorId, int shapeId, int habitatId, int genderRate, int captureRate, int baseHappiness, int isBaby, int hatchCounter, int hasGenderDifferences, int growthRateId, int formsSwitchable, int isLegendary, int isMythical, int order, int conquestOrder);
    
    // Override the printObject method
    void printObject() const override;

     // Getter methods
    int getId() const { return id; }
    std::string getIdentifier() const { return identifier; }
    int getGenerationId() const { return generationId; }
    int getEvolvesFromSpeciesId() const { return evolvesFromSpeciesId; }
    int getEvolutionChainId() const { return evolutionChainId; }
    int getColorId() const { return colorId; }
    int getShapeId() const { return shapeId; }
    int getHabitatId() const { return habitatId; }
    int getGenderRate() const { return genderRate; }
    int getCaptureRate() const { return captureRate; }
    int getBaseHappiness() const { return baseHappiness; }
    int getIsBaby() const { return isBaby; }
    int getHatchCounter() const { return hatchCounter; }
    int getHasGenderDifferences() const { return hasGenderDifferences; }
    int getGrowthRateId() const { return growthRateId; }
    int getFormsSwitchable() const { return formsSwitchable; }
    int getIsLegendary() const { return isLegendary; }
    int getIsMythical() const { return isMythical; }
    int getOrder() const { return order; }
    int getConquestOrder() const { return conquestOrder; }
};

// Derived class for Pokemon CSV
class Experience : public DataObject {
private:
    int growthRate;
    int level;
    int exp;
public:
    // Public constructor with an initializer list
    Experience(int growthRate, int level, int exp);

    // Override the printObject method
    void printObject() const override;

    // Getter methods
    int getGrowthRate() const { return growthRate; }
    int getLevel() const { return level; }
    int getExp() const { return exp; }

};

// Derived class for Pokemon CSV
class TypeNames : public DataObject {
private:
    int typeId;
    int localLanguageId;
    std::string name;

public:
    // Public constructor with an initializer list
    TypeNames(int typeId, int localLanguageId, std::string name);

    // Override the printObject method
    void printObject() const override;

    // Getter methods
    int getTypeId() const { return typeId; }
    int getLocalLanguageId() const { return localLanguageId; }
    std::string getName() const { return name; }  
};

// Derived class for Pokemon CSV
class PokemonStats : public DataObject {
private:
    int pokemonId;
    int statId;
    int baseStat;
    int effort;

public:
    // Public constructor with an initializer list
    PokemonStats(int pokemonId, int statId, int baseStat, int effort);

    // Override the printObject method
    void printObject() const override;

    // Getter methods
    int getPokemonId() const { return pokemonId; }
    int getStatId() const { return statId; }
    int getBaseStat() const { return baseStat; }
    int getEffort() const { return effort; }
};

// Derived class for Pokemon CSV
class Stats : public DataObject {
private:
    int id;
    int damageClassId;
    std::string identifier;
    int isBattleOnly;
    int gameIndex;

public:
    // Public constructor with an initializer list
    Stats(int id, int damageClassId, std::string identifier, int isBattleOnly, int gameIndex);
    Stats(){};
    // Override the printObject method
    void printObject() const override;

    // Getter methods
    int getId() const { return id; }
    int getDamageClassId() const { return damageClassId; }
    std::string getIdentifier() const { return identifier; }
    int getIsBattleOnly() const { return isBattleOnly; }
    int getGameIndex() const { return gameIndex; }
};

// Derived class for Pokemon CSV
class PokemonTypes : public DataObject {
private:
    int pokemonId;
    int typeId;
    int slot;

public:
    // Public constructor with an initializer list
    PokemonTypes(int pokemonId, int typeId, int slot);

    // Override the printObject method
    void printObject() const override;

    // Getter methods
    int getPokemonId() const { return pokemonId; }
    int getTypeId() const { return typeId; }
    int getSlot() const { return slot; }
};

// Base class for all DataList objects
class DataObjectList {
protected:
    std::vector<std::vector<std::string>> data;

public:
    // Constructor that reads data from a CSV file
    DataObjectList(const std::string& filename);
    std::vector<std::unique_ptr<DataObject>> objects;
    // Pure virtual method for printing data
    virtual void printData() const = 0;

    // Pure virtual function for setting fields
    virtual void setFields(const std::vector<std::vector<std::string>>& data) = 0;

    // Method to get access to the objects vector
    const std::vector<std::unique_ptr<DataObject>>& getObjects() const;


    // Virtual destructor to ensure proper cleanup
    virtual ~DataObjectList();
};

// Derived class for Pokemon CSV
class PokemonList : public DataObjectList {
public:
    // Using DataObject's constructor
    //using DataObjectList::DataObjectList;
    // Explicitly declare the constructor that takes a filename
    PokemonList(const std::string& filename);
    
    // Override the pure virtual function setFields
    void setFields(const std::vector<std::vector<std::string>>& data) override;
    void printData() const override;
    Pokemon* getPokemonById(int id);
};

// Derived class for Pokemon CSV
class MovesList : public DataObjectList {
public:
    
    MovesList(const std::string& filename);

    // Override the pure virtual function setFields
    void setFields(const std::vector<std::vector<std::string>>& data) override;
    void printData() const override;
};

// Derived class for Pokemon CSV
class PokemonMovesList : public DataObjectList {
public:

    PokemonMovesList(const std::string& filename);

    // Override the pure virtual function setFields
    void setFields(const std::vector<std::vector<std::string>>& data) override;
    void printData() const override;
};

// Derived class for Pokemon CSV
class PokemonSpeciesList : public DataObjectList {
public:
    
    PokemonSpeciesList(const std::string& filename);

    // Override the pure virtual function setFields
    void setFields(const std::vector<std::vector<std::string>>& data) override;
    void printData() const override;
};

// Derived class for Pokemon CSV
class ExperienceList : public DataObjectList {
public:
   
   ExperienceList(const std::string& filename);

    // Override the pure virtual function setFields
    void setFields(const std::vector<std::vector<std::string>>& data) override;
    void printData() const override;
};

// Derived class for Pokemon CSV
class TypeNamesList : public DataObjectList {
public:
    
   TypeNamesList(const std::string& filename);

   // Override the pure virtual function setFields
    void setFields(const std::vector<std::vector<std::string>>& data) override;
    void printData() const override;
};

// Derived class for Pokemon CSV
class PokemonStatsList : public DataObjectList {
public:
    
   PokemonStatsList(const std::string& filename);

    // Override the pure virtual function setFields
    void setFields(const std::vector<std::vector<std::string>>& data) override;
    void printData() const override;
};

// Derived class for Pokemon CSV
class StatsList : public DataObjectList {
public:
     
   StatsList(const std::string& filename);

    // Override the pure virtual function setFields
    void setFields(const std::vector<std::vector<std::string>>& data) override;
    void printData() const override;
};

// Derived class for Pokemon CSV
class PokemonTypesList : public DataObjectList {
public:
      
   PokemonTypesList(const std::string& filename);

   // Override the pure virtual function setFields
    void setFields(const std::vector<std::vector<std::string>>& data) override;
    void printData() const override;
};


//helpers for printing lists for debugging for assignment 8 

//print pokemonmoves list 
void printPokemonMovesList(const std::vector<PokemonMoves>& pokeMoves);
// Add declarations for other derived classes here...

#endif // DATA_H