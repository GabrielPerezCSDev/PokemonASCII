#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>
#include "data.h"
#include <memory>
#include <iomanip> // Include for std::setw
#include <climits>

/*CSVReader stuff*/

std::vector<std::vector<std::string>> CSVReader::readCSV(const std::string& filename) {
    std::vector<std::vector<std::string>> data;
    
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line)) {
        std::vector<std::string> row;
        std::stringstream lineStream(line);
        std::string cell;

        // Special handling to correctly parse empty cells
        while (lineStream.good()) {
            std::getline(lineStream, cell, ',');
            row.push_back(cell);
        }

        data.push_back(row);
    }

    return data;
}


/*Data Object*/
DataObjectList::DataObjectList(const std::string& filename) {
    
    data = CSVReader::readCSV(filename);
}

const std::vector<std::unique_ptr<DataObject>>& DataObjectList::getObjects() const {
    return objects;
}

DataObjectList::~DataObjectList() {
    // Destructor implementation (if needed)
}






/*Pokemon related stuff*/

// Pokemon class 
// Implement the Pokemon constructor
Pokemon::Pokemon(int id, std::string identifier, int speciesId, int height, int weight, int baseExp, int order, int isDefault)
    : id(id), identifier(identifier), speciesId(speciesId), height(height), weight(weight), baseExp(baseExp), order(order), isDefault(isDefault) {
    
}
//override the print method 
void Pokemon::printObject() const {
    std::cout << "ID: " << id << "\n"
              << "Identifier: " << identifier << "\n"
              << "Species ID: " << speciesId << "\n"
              << "Height: " << height << "\n"
              << "Weight: " << weight << "\n"
              << "Base Experience: " << baseExp << "\n"
              << "Order: " << order << "\n"
              << "Is Default: " << isDefault << std::endl;
}

//set the fields in the constructor 
PokemonList::PokemonList(const std::string& filename) : DataObjectList(filename) {
    // Now that the base class constructor has read the CSV data into `data`,
    // call setFields to initialize the objects based on that data.
    setFields(data);
}
//pokemon list setField override
void PokemonList::setFields(const std::vector<std::vector<std::string>>& newData) {
    
    bool firstRowIsHeader = true; // Set to true if the first row is always a header
    for (const auto& row : newData) {
        if (firstRowIsHeader) {
            // Skip the first row if it contains headers
            firstRowIsHeader = false;
            continue;
        }
        if (row.size() != 8) {
            throw std::invalid_argument("Row does not contain the correct number of fields for a Pokemon object.");
        }
        try {
            // Convert each string to the appropriate type and create a new Pokemon object
            int id = std::stoi(row[0]);
            std::string identifier = row[1];
            int speciesId = std::stoi(row[2]);
            int height = std::stoi(row[3]);
            int weight = std::stoi(row[4]);
            int baseExp = std::stoi(row[5]);
            int order = std::stoi(row[6]);
            int isDefault = std::stoi(row[7]);

            // Add the new Pokemon object to the objects vector
            objects.push_back(std::make_unique<Pokemon>(id, identifier, speciesId, height, weight, baseExp, order, isDefault));
        } catch (const std::invalid_argument& e) {
            // Handle the case where the string cannot be converted to an integer
            std::cerr << "Invalid argument: " << e.what() << " for input string: " << row[0] << ", " << row[2] << ", " << row[3] << ", " << row[4] << ", " << row[5] << ", " << row[6] << ", " << row[7] << std::endl;
            // Handle the error, e.g., by skipping this row, logging the error, or re-throwing the exception
        } catch (const std::out_of_range& e) {
            // Handle the case where the integer is out of range for int
            std::cerr << "Out of range: " << e.what() << std::endl;
            // Handle the error, e.g., by skipping this row, logging the error, or re-throwing the exception
        }
    }
}

void PokemonList::printData() const {
    if (objects.empty()) {
        std::cerr << "No data to print." << std::endl;
        return;
    }

    // Custom headers for the PokemonList
    std::vector<std::string> headers = {"ID", "Identifier", "Species ID", "Height", "Weight", "Base Exp", "Order", "Is Default"};

    // Determine the maximum width needed for each column
    std::vector<std::size_t> columnWidths(headers.size(), 0);
    for (std::size_t i = 0; i < headers.size(); ++i) {
        columnWidths[i] = headers[i].length();
    }

    // Update column widths based on the data
    for (const auto& obj : objects) {
        const Pokemon* pokemon = dynamic_cast<const Pokemon*>(obj.get());
        if (pokemon) {
            columnWidths[0] = std::max(columnWidths[0], std::to_string(pokemon->getId()).length());
            columnWidths[1] = std::max(columnWidths[1], pokemon->getIdentifier().length());
            columnWidths[2] = std::max(columnWidths[2], std::to_string(pokemon->getSpeciesId()).length());
            columnWidths[3] = std::max(columnWidths[3], std::to_string(pokemon->getHeight()).length());
            columnWidths[4] = std::max(columnWidths[4], std::to_string(pokemon->getWeight()).length());
            columnWidths[5] = std::max(columnWidths[5], std::to_string(pokemon->getBaseExp()).length());
            columnWidths[6] = std::max(columnWidths[6], std::to_string(pokemon->getOrder()).length());
            columnWidths[7] = std::max(columnWidths[7], std::to_string(pokemon->getIsDefault()).length());
        }
    }

    // Print the headers with proper spacing
    for (std::size_t i = 0; i < headers.size(); ++i) {
        std::cout << std::left << std::setw(columnWidths[i] + 2) << headers[i]; // +2 for padding
    }
    std::cout << std::endl;

    // Print the data with aligned columns
    for (const auto& obj : objects) {
        const Pokemon* pokemon = dynamic_cast<const Pokemon*>(obj.get());
        if (pokemon) {
            std::cout << std::left << std::setw(columnWidths[0] + 2) << pokemon->getId()
                      << std::left << std::setw(columnWidths[1] + 2) << pokemon->getIdentifier()
                      << std::left << std::setw(columnWidths[2] + 2) << pokemon->getSpeciesId()
                      << std::left << std::setw(columnWidths[3] + 2) << pokemon->getHeight()
                      << std::left << std::setw(columnWidths[4] + 2) << pokemon->getWeight()
                      << std::left << std::setw(columnWidths[5] + 2) << pokemon->getBaseExp()
                      << std::left << std::setw(columnWidths[6] + 2) << pokemon->getOrder()
                      << std::left << std::setw(columnWidths[7] + 2) << pokemon->getIsDefault()
                      << std::endl;
        }
    }
}
// PokemonList class method to get a Pokemon by its ID using binary search
Pokemon* PokemonList::getPokemonById(int id) {
    for (const auto& obj : objects) {
        // Cast the DataObject pointer to a Pokemon pointer
        Pokemon* pokemon = dynamic_cast<Pokemon*>(obj.get());
        if (pokemon && pokemon->getId() == id) {
            // Found the Pokemon with the matching ID
            return pokemon;
        }
    }

    // If the Pokemon with the given ID is not found, return nullptr
    return nullptr;
}



/*Moves related stuff*/

// Moves class
// Moves class constructor implementation
Moves::Moves(int id, std::string identifier, int generationId, int typeId, int power, int pp, int accuracy, int priority, int targetId, int damageClass, int effectId, int effectChance, int contestTypeId, int contestEffectId, int superContestEffectId)
    : id(id), identifier(identifier), generationId(generationId), typeId(typeId), power(power), pp(pp), accuracy(accuracy), priority(priority), targetId(targetId), damageClass(damageClass), effectId(effectId), effectChance(effectChance), contestTypeId(contestTypeId), contestEffectId(contestEffectId), superContestEffectId(superContestEffectId) {}

// Override the print method for Moves
void Moves::printObject() const {
    std::cout << "ID: " << id << "\n"
              << "Identifier: " << identifier << "\n"
              << "Generation ID: " << generationId << "\n"
              << "Type ID: " << typeId << "\n"
              << "Power: " << power << "\n"
              << "PP: " << pp << "\n"
              << "Accuracy: " << accuracy << "\n"
              << "Priority: " << priority << "\n"
              << "Target ID: " << targetId << "\n"
              << "Damage Class: " << damageClass << "\n"
              << "Effect ID: " << effectId << "\n"
              << "Effect Chance: " << effectChance << "\n"
              << "Contest Type ID: " << contestTypeId << "\n"
              << "Contest Effect ID: " << contestEffectId << "\n"
              << "Super Contest Effect ID: " << superContestEffectId << std::endl;
}


//set the fields in the constructor 
MovesList::MovesList(const std::string& filename) : DataObjectList(filename) {
    // Now that the base class constructor has read the CSV data into `data`,
    // call setFields to initialize the objects based on that data.
    setFields(data);
}
// MovesList setFields override
void MovesList::setFields(const std::vector<std::vector<std::string>>& newData) {
    bool firstRowIsHeader = true; // Assuming the first row is always a header
    for (const auto& row : newData) {
        if (firstRowIsHeader) {
            firstRowIsHeader = false;
            continue;
        }
        if (row.size() != 15) { // Ensure the row has the correct number of fields for a Moves object
            throw std::invalid_argument("Row does not contain the correct number of fields for a Moves object.");
        }
        try {
            // Convert each string to the appropriate type and create a new Moves object
            int id = row[0].empty() ? INT_MAX : std::stoi(row[0]);
            std::string identifier = row[1];
            int generationId = row[2].empty() ? INT_MAX : std::stoi(row[2]);
            int typeId = row[3].empty() ? INT_MAX : std::stoi(row[3]);
            int power = row[4].empty() ? 1 : std::stoi(row[4]); // Set to 1 for non-attack moves
            int pp = row[5].empty() ? INT_MAX : std::stoi(row[5]);
            int accuracy = row[6].empty() ? -1 : std::stoi(row[6]); // Set to -1 for moves where accuracy is not applicable
            int priority = row[7].empty() ? INT_MAX : std::stoi(row[7]);
            int targetId = row[8].empty() ? INT_MAX : std::stoi(row[8]);
            int damageClass = row[9].empty() ? INT_MAX : std::stoi(row[9]);
            int effectId = row[10].empty() ? INT_MAX : std::stoi(row[10]);
            int effectChance = row[11].empty() ? INT_MAX : std::stoi(row[11]);
            int contestTypeId = row[12].empty() ? INT_MAX : std::stoi(row[12]);
            int contestEffectId = row[13].empty() ? INT_MAX : std::stoi(row[13]);
            int superContestEffectId = row[14].empty() ? INT_MAX : std::stoi(row[14]);

            // Add the new Moves object to the objects vector
            objects.push_back(std::make_unique<Moves>(id, identifier, generationId, typeId, power, pp, accuracy, priority, targetId, damageClass, effectId, effectChance, contestTypeId, contestEffectId, superContestEffectId));
        } catch (const std::invalid_argument& e) {
            std::cerr << "Invalid argument: " << e.what() << " for input string in Moves data." << std::endl;
        } catch (const std::out_of_range& e) {
            std::cerr << "Out of range: " << e.what() << " in Moves data." << std::endl;
        }
    }
}

void MovesList::printData() const {
    if (objects.empty()) {
        std::cerr << "No data to print." << std::endl;
        return;
    }

    // Custom headers for the MovesList
    std::vector<std::string> headers = {
        "ID", "Identifier", "Generation ID", "Type ID", "Power", "PP", "Accuracy",
        "Priority", "Target ID", "Damage Class", "Effect ID", "Effect Chance",
        "Contest Type ID", "Contest Effect ID", "Super Contest Effect ID"
    };

    // Lambda function to handle INT_MAX values
    auto intToString = [](int value) -> std::string {
        return value == INT_MAX ? "" : std::to_string(value);
    };

    // Determine the maximum width needed for each column
    std::vector<std::size_t> columnWidths(headers.size(), 0);
    for (std::size_t i = 0; i < headers.size(); ++i) {
        columnWidths[i] = headers[i].length();
    }
    for (const auto& obj : objects) {
        const Moves* move = dynamic_cast<const Moves*>(obj.get());
        if (move) {
            std::vector<std::string> fields = {
                intToString(move->getId()),
                move->getIdentifier(),
                intToString(move->getGenerationId()),
                intToString(move->getTypeId()),
                intToString(move->getPower()),
                intToString(move->getPp()),
                intToString(move->getAccuracy()),
                intToString(move->getPriority()),
                intToString(move->getTargetId()),
                intToString(move->getDamageClass()),
                intToString(move->getEffectId()),
                intToString(move->getEffectChance()),
                intToString(move->getContestTypeId()),
                intToString(move->getContestEffectId()),
                intToString(move->getSuperContestEffectId())
            };
            for (std::size_t i = 0; i < fields.size(); ++i) {
                columnWidths[i] = std::max(columnWidths[i], fields[i].length());
            }
        }
    }

    // Print the headers with proper spacing
    for (std::size_t i = 0; i < headers.size(); ++i) {
        std::cout << std::left << std::setw(columnWidths[i] + 2) << headers[i]; // +2 for padding
    }
    std::cout << std::endl;

    // Print the data with aligned columns
    for (const auto& obj : objects) {
        const Moves* move = dynamic_cast<const Moves*>(obj.get());
        if (move) {
            std::cout << std::left << std::setw(columnWidths[0] + 2) << intToString(move->getId())
                      << std::left << std::setw(columnWidths[1] + 2) << move->getIdentifier()
                      << std::left << std::setw(columnWidths[2] + 2) << intToString(move->getGenerationId())
                      << std::left << std::setw(columnWidths[3] + 2) << intToString(move->getTypeId())
                      << std::left << std::setw(columnWidths[4] + 2) << intToString(move->getPower())
                      << std::left << std::setw(columnWidths[5] + 2) << intToString(move->getPp())
                      << std::left << std::setw(columnWidths[6] + 2) << intToString(move->getAccuracy())
                      << std::left << std::setw(columnWidths[7] + 2) << intToString(move->getPriority())
                      << std::left << std::setw(columnWidths[8] + 2) << intToString(move->getTargetId())
                      << std::left << std::setw(columnWidths[9] + 2) << intToString(move->getDamageClass())
                      << std::left << std::setw(columnWidths[10] + 2) << intToString(move->getEffectId())
                      << std::left << std::setw(columnWidths[11] + 2) << intToString(move->getEffectChance())
                      << std::left << std::setw(columnWidths[12] + 2) << intToString(move->getContestTypeId())
                      << std::left << std::setw(columnWidths[13] + 2) << intToString(move->getContestEffectId())
                      << std::left << std::setw(columnWidths[14] + 2) << intToString(move->getSuperContestEffectId())
                      << std::endl;
        }
    }
}




/*PokemonMoves related stuff*/

// PokemonMoves class
PokemonMoves::PokemonMoves(int pokemonId, int versionGroupId, int moveId, int pokemonMoveMethodId, int level, int order)
    : pokemonId(pokemonId), versionGroupId(versionGroupId), moveId(moveId), 
      pokemonMoveMethodId(pokemonMoveMethodId), level(level), order(order) {}
// Override the print method for PokemonMoves
void PokemonMoves::printObject() const {
    std::cout << "Pokemon ID: " << pokemonId << "\n"
              << "Version Group ID: " << versionGroupId << "\n"
              << "Move ID: " << moveId << "\n"
              << "Pokemon Move Method ID: " << pokemonMoveMethodId << "\n"
              << "Level: " << level << "\n"
              << "Order: " << order << std::endl;
}

//set the fields in the constructor 
PokemonMovesList::PokemonMovesList(const std::string& filename) : DataObjectList(filename) {
    // Now that the base class constructor has read the CSV data into `data`,
    // call setFields to initialize the objects based on that data.
    setFields(data);
}
// PokemonMovesList setFields override
void PokemonMovesList::setFields(const std::vector<std::vector<std::string>>& newData) {
    bool firstRowIsHeader = true; // Assuming the first row is always a header
    for (const auto& row : newData) {
        if (firstRowIsHeader) {
            firstRowIsHeader = false;
            continue;
        }
        if (row.size() != 6) { // Ensure the row has the correct number of fields for a PokemonMoves object
            throw std::invalid_argument("Row does not contain the correct number of fields for a PokemonMoves object.");
        }
        try {
            // Convert each string to the appropriate type and create a new PokemonMoves object
            int pokemonId = row[0].empty() ? INT_MAX : std::stoi(row[0]);
            int versionGroupId = row[1].empty() ? INT_MAX : std::stoi(row[1]);
            int moveId = row[2].empty() ? INT_MAX : std::stoi(row[2]);
            int pokemonMoveMethodId = row[3].empty() ? INT_MAX : std::stoi(row[3]);
            int level = row[4].empty() ? INT_MAX : std::stoi(row[4]);
            int order = row[5].empty() ? INT_MAX : std::stoi(row[5]);

            // Check if versionGroupId is equal to 1 before adding the object
            if (versionGroupId == 1) {
                // Add the new PokemonMoves object to the objects vector only if versionGroupId is 1
                objects.push_back(std::make_unique<PokemonMoves>(pokemonId, versionGroupId, moveId, pokemonMoveMethodId, level, order));
            }
        } catch (const std::invalid_argument& e) {
            std::cerr << "Invalid argument: " << e.what() << " for input string in PokemonMoves data." << std::endl;
        } catch (const std::out_of_range& e) {
            std::cerr << "Out of range: " << e.what() << " in PokemonMoves data." << std::endl;
        }
    }
}

void PokemonMovesList::printData() const {
    if (objects.empty()) {
        std::cerr << "No data to print." << std::endl;
        return;
    }

    // Custom headers for the PokemonMovesList
    std::vector<std::string> headers = {
        "Pokemon ID", "Version Group ID", "Move ID", "Pokemon Move Method ID", "Level", "Order"
    };

    // Lambda function to handle INT_MAX values
    auto intToString = [](int value) -> std::string {
        return value == INT_MAX ? "" : std::to_string(value);
    };

    // Determine the maximum width needed for each column
    std::vector<std::size_t> columnWidths(headers.size(), 0);
    for (std::size_t i = 0; i < headers.size(); ++i) {
        columnWidths[i] = headers[i].length();
    }
    for (const auto& obj : objects) {
        const PokemonMoves* pokemonMove = dynamic_cast<const PokemonMoves*>(obj.get());
        if (pokemonMove) {
            std::vector<std::string> fields = {
                intToString(pokemonMove->getPokemonId()),
                intToString(pokemonMove->getVersionGroupId()),
                intToString(pokemonMove->getMoveId()),
                intToString(pokemonMove->getPokemonMoveMethodId()),
                intToString(pokemonMove->getLevel()),
                intToString(pokemonMove->getOrder())
            };
            for (std::size_t i = 0; i < fields.size(); ++i) {
                columnWidths[i] = std::max(columnWidths[i], fields[i].length());
            }
        }
    }

    // Print the headers with proper spacing
    for (std::size_t i = 0; i < headers.size(); ++i) {
        std::cout << std::left << std::setw(columnWidths[i] + 2) << headers[i]; // +2 for padding
    }
    std::cout << std::endl;

    // Print the data with aligned columns
    for (const auto& obj : objects) {
        const PokemonMoves* pokemonMove = dynamic_cast<const PokemonMoves*>(obj.get());
        if (pokemonMove) {
            std::cout << std::left << std::setw(columnWidths[0] + 2) << intToString(pokemonMove->getPokemonId())
                      << std::left << std::setw(columnWidths[1] + 2) << intToString(pokemonMove->getVersionGroupId())
                      << std::left << std::setw(columnWidths[2] + 2) << intToString(pokemonMove->getMoveId())
                      << std::left << std::setw(columnWidths[3] + 2) << intToString(pokemonMove->getPokemonMoveMethodId())
                      << std::left << std::setw(columnWidths[4] + 2) << intToString(pokemonMove->getLevel())
                      << std::left << std::setw(columnWidths[5] + 2) << intToString(pokemonMove->getOrder())
                      << std::endl;
        }
    }
}









/*Pokemon species*/
// PokemonSpecies class constructor implementation
PokemonSpecies::PokemonSpecies(int id, std::string identifier, int generationId, int evolvesFromSpeciesId, int evolutionChainId, int colorId, int shapeId, int habitatId, int genderRate, int captureRate, int baseHappiness, int isBaby, int hatchCounter, int hasGenderDifferences, int growthRateId, int formsSwitchable, int isLegendary, int isMythical, int order, int conquestOrder)
    : id(id), identifier(identifier), generationId(generationId), evolvesFromSpeciesId(evolvesFromSpeciesId), evolutionChainId(evolutionChainId), colorId(colorId), shapeId(shapeId), habitatId(habitatId), genderRate(genderRate), captureRate(captureRate), baseHappiness(baseHappiness), isBaby(isBaby), hatchCounter(hatchCounter), hasGenderDifferences(hasGenderDifferences), growthRateId(growthRateId), formsSwitchable(formsSwitchable), isLegendary(isLegendary), isMythical(isMythical), order(order), conquestOrder(conquestOrder) {}

// Override the print method for PokemonSpecies
void PokemonSpecies::printObject() const {
    std::cout << "ID: " << id << "\n"
              << "Identifier: " << identifier << "\n"
              << "Generation ID: " << generationId << "\n"
              << "Evolves From Species ID: " << evolvesFromSpeciesId << "\n"
              << "Evolution Chain ID: " << evolutionChainId << "\n"
              << "Color ID: " << colorId << "\n"
              << "Shape ID: " << shapeId << "\n"
              << "Habitat ID: " << habitatId << "\n"
              << "Gender Rate: " << genderRate << "\n"
              << "Capture Rate: " << captureRate << "\n"
              << "Base Happiness: " << baseHappiness << "\n"
              << "Is Baby: " << isBaby << "\n"
              << "Hatch Counter: " << hatchCounter << "\n"
              << "Has Gender Differences: " << hasGenderDifferences << "\n"
              << "Growth Rate ID: " << growthRateId << "\n"
              << "Forms Switchable: " << formsSwitchable << "\n"
              << "Is Legendary: " << isLegendary << "\n"
              << "Is Mythical: " << isMythical << "\n"
              << "Order: " << order << "\n"
              << "Conquest Order: " << conquestOrder << std::endl;
}

//set the fields in the constructor 
PokemonSpeciesList::PokemonSpeciesList(const std::string& filename) : DataObjectList(filename) {
    // Now that the base class constructor has read the CSV data into `data`,
    // call setFields to initialize the objects based on that data.
    setFields(data);
}

// PokemonSpeciesList setFields override
void PokemonSpeciesList::setFields(const std::vector<std::vector<std::string>>& newData) {
    bool firstRowIsHeader = true; // Assuming the first row is always a header
    for (const auto& row : newData) {
        if (firstRowIsHeader) {
            firstRowIsHeader = false;
            continue;
        }
        if (row.size() != 20) { // Ensure the row has the correct number of fields for a PokemonSpecies object
            throw std::invalid_argument("Row does not contain the correct number of fields for a PokemonSpecies object.");
        }
        try {
            // Convert each string to the appropriate type and create a new PokemonSpecies object
            int id = row[0].empty() ? INT_MAX : std::stoi(row[0]);
            std::string identifier = row[1]; // No need to check for empty, it's a string
            int generationId = row[2].empty() ? INT_MAX : std::stoi(row[2]);
            int evolvesFromSpeciesId = row[3].empty() ? INT_MAX : std::stoi(row[3]);
            int evolutionChainId = row[4].empty() ? INT_MAX : std::stoi(row[4]);
            int colorId = row[5].empty() ? INT_MAX : std::stoi(row[5]);
            int shapeId = row[6].empty() ? INT_MAX : std::stoi(row[6]);
            int habitatId = row[7].empty() ? INT_MAX : std::stoi(row[7]);
            int genderRate = row[8].empty() ? INT_MAX : std::stoi(row[8]);
            int captureRate = row[9].empty() ? INT_MAX : std::stoi(row[9]);
            int baseHappiness = row[10].empty() ? INT_MAX : std::stoi(row[10]);
            int isBaby = row[11].empty() ? INT_MAX : std::stoi(row[11]);
            int hatchCounter = row[12].empty() ? INT_MAX : std::stoi(row[12]);
            int hasGenderDifferences = row[13].empty() ? INT_MAX : std::stoi(row[13]);
            int growthRateId = row[14].empty() ? INT_MAX : std::stoi(row[14]);
            int formsSwitchable = row[15].empty() ? INT_MAX : std::stoi(row[15]);
            int isLegendary = row[16].empty() ? INT_MAX : std::stoi(row[16]);
            int isMythical = row[17].empty() ? INT_MAX : std::stoi(row[17]);
            int order = row[18].empty() ? INT_MAX : std::stoi(row[18]);
            int conquestOrder = row[19].empty() ? INT_MAX : std::stoi(row[19]);

            // Add the new PokemonSpecies object to the objects vector
            objects.push_back(std::make_unique<PokemonSpecies>(id, identifier, generationId, evolvesFromSpeciesId, evolutionChainId, colorId, shapeId, habitatId, genderRate, captureRate, baseHappiness, isBaby, hatchCounter, hasGenderDifferences, growthRateId, formsSwitchable, isLegendary, isMythical, order, conquestOrder));
        } catch (const std::invalid_argument& e) {
            std::cerr << "Invalid argument: " << e.what() << " for input string in PokemonSpecies data." << std::endl;
        } catch (const std::out_of_range& e) {
            std::cerr << "Out of range: " << e.what() << " in PokemonSpecies data." << std::endl;
        }
    }
}

void PokemonSpeciesList::printData() const {
    if (objects.empty()) {
        std::cerr << "No data to print." << std::endl;
        return;
    }

    // Custom headers for the PokemonSpeciesList
    std::vector<std::string> headers = {
        "ID", "Identifier", "Generation ID", "Evolves From Species ID", "Evolution Chain ID", 
        "Color ID", "Shape ID", "Habitat ID", "Gender Rate", "Capture Rate", 
        "Base Happiness", "Is Baby", "Hatch Counter", "Has Gender Differences", 
        "Growth Rate ID", "Forms Switchable", "Is Legendary", "Is Mythical", "Order", "Conquest Order"
    };

    // Lambda function to handle INT_MAX values
    auto intToString = [](int value) -> std::string {
        return value == INT_MAX ? "" : std::to_string(value);
    };

    // Determine the maximum width needed for each column
    std::vector<std::size_t> columnWidths(headers.size(), 0);
    for (std::size_t i = 0; i < headers.size(); ++i) {
        columnWidths[i] = headers[i].length();
    }
    for (const auto& obj : objects) {
        const PokemonSpecies* species = dynamic_cast<const PokemonSpecies*>(obj.get());
        if (species) {
            std::vector<std::string> fields = {
                intToString(species->getId()),
                species->getIdentifier(),
                intToString(species->getGenerationId()),
                intToString(species->getEvolvesFromSpeciesId()),
                intToString(species->getEvolutionChainId()),
                intToString(species->getColorId()),
                intToString(species->getShapeId()),
                intToString(species->getHabitatId()),
                intToString(species->getGenderRate()),
                intToString(species->getCaptureRate()),
                intToString(species->getBaseHappiness()),
                intToString(species->getIsBaby()),
                intToString(species->getHatchCounter()),
                intToString(species->getHasGenderDifferences()),
                intToString(species->getGrowthRateId()),
                intToString(species->getFormsSwitchable()),
                intToString(species->getIsLegendary()),
                intToString(species->getIsMythical()),
                intToString(species->getOrder()),
                intToString(species->getConquestOrder())
            };
            for (std::size_t i = 0; i < fields.size(); ++i) {
                columnWidths[i] = std::max(columnWidths[i], fields[i].length());
            }
        }
    }

    // Print the headers with proper spacing
    for (std::size_t i = 0; i < headers.size(); ++i) {
        std::cout << std::left << std::setw(columnWidths[i] + 2) << headers[i]; // +2 for padding
    }
    std::cout << std::endl;

    for (const auto& obj : objects) {
    const PokemonSpecies* species = dynamic_cast<const PokemonSpecies*>(obj.get());
    if (species) {
        std::vector<std::string> fieldValues = {
            intToString(species->getId()),
            species->getIdentifier(),
            intToString(species->getGenerationId()),
            intToString(species->getEvolvesFromSpeciesId()),
            intToString(species->getEvolutionChainId()),
            intToString(species->getColorId()),
            intToString(species->getShapeId()),
            intToString(species->getHabitatId()),
            intToString(species->getGenderRate()),
            intToString(species->getCaptureRate()),
            intToString(species->getBaseHappiness()),
            intToString(species->getIsBaby()),
            intToString(species->getHatchCounter()),
            intToString(species->getHasGenderDifferences()),
            intToString(species->getGrowthRateId()),
            intToString(species->getFormsSwitchable()),
            intToString(species->getIsLegendary()),
            intToString(species->getIsMythical()),
            intToString(species->getOrder()),
            intToString(species->getConquestOrder())
        };

        for (std::size_t i = 0; i < headers.size(); ++i) {
            std::cout << std::left << std::setw(columnWidths[i] + 2) << fieldValues[i];
        }
        std::cout << std::endl;
    }
}
}










/*Experience Stuff*/

// Experience class constructor implementation
Experience::Experience(int growthRate, int level, int exp)
    : growthRate(growthRate), level(level), exp(exp) {}

// Override the print method for Experience
void Experience::printObject() const {
    std::cout << "Growth Rate: " << growthRate << "\n"
              << "Level: " << level << "\n"
              << "Experience: " << exp << std::endl;
}

//set the fields in the constructor 
ExperienceList::ExperienceList(const std::string& filename) : DataObjectList(filename) {
    // Now that the base class constructor has read the CSV data into `data`,
    // call setFields to initialize the objects based on that data.
    setFields(data);
}

// ExperienceList setFields override
void ExperienceList::setFields(const std::vector<std::vector<std::string>>& newData) {
    bool firstRowIsHeader = true; // Assuming the first row is always a header
    for (const auto& row : newData) {
        if (firstRowIsHeader) {
            firstRowIsHeader = false;
            continue;
        }
        if (row.size() != 3) { // Ensure the row has the correct number of fields for an Experience object
            throw std::invalid_argument("Row does not contain the correct number of fields for an Experience object.");
        }
        try {
            // Convert each string to the appropriate type and create a new Experience object
            int growthRate = row[0].empty() ? INT_MAX : std::stoi(row[0]);
            int level = row[1].empty() ? INT_MAX : std::stoi(row[1]);
            int exp = row[2].empty() ? INT_MAX : std::stoi(row[2]);

            // Add the new Experience object to the objects vector
            objects.push_back(std::make_unique<Experience>(growthRate, level, exp));
        } catch (const std::invalid_argument& e) {
            std::cerr << "Invalid argument: " << e.what() << " for input string in Experience data." << std::endl;
        } catch (const std::out_of_range& e) {
            std::cerr << "Out of range: " << e.what() << " in Experience data." << std::endl;
        }
    }
}


void ExperienceList::printData() const {
    if (objects.empty()) {
        std::cerr << "No data to print." << std::endl;
        return;
    }

    // Custom headers for the ExperienceList
    std::vector<std::string> headers = {
        "Growth Rate", "Level", "Experience Points"
    };

    // Lambda function to handle INT_MAX values
    auto intToString = [](int value) -> std::string {
        return value == INT_MAX ? "" : std::to_string(value);
    };

    // Determine the maximum width needed for each column
    std::vector<std::size_t> columnWidths(headers.size(), 0);
    for (std::size_t i = 0; i < headers.size(); ++i) {
        columnWidths[i] = headers[i].length();
    }
    for (const auto& obj : objects) {
        const Experience* experience = dynamic_cast<const Experience*>(obj.get());
        if (experience) {
            std::vector<std::string> fields = {
                intToString(experience->getGrowthRate()),
                intToString(experience->getLevel()),
                intToString(experience->getExp())
            };
            for (std::size_t i = 0; i < fields.size(); ++i) {
                columnWidths[i] = std::max(columnWidths[i], fields[i].length());
            }
        }
    }

    // Print the headers with proper spacing
    for (std::size_t i = 0; i < headers.size(); ++i) {
        std::cout << std::left << std::setw(columnWidths[i] + 2) << headers[i]; // +2 for padding
    }
    std::cout << std::endl;

    // Print the data with aligned columns
    for (const auto& obj : objects) {
        const Experience* experience = dynamic_cast<const Experience*>(obj.get());
        if (experience) {
            std::cout << std::left << std::setw(columnWidths[0] + 2) << intToString(experience->getGrowthRate())
                      << std::left << std::setw(columnWidths[1] + 2) << intToString(experience->getLevel())
                      << std::left << std::setw(columnWidths[2] + 2) << intToString(experience->getExp())
                      << std::endl;
        }
    }
}








/*Type names stuff*/
TypeNames::TypeNames(int typeId, int localLanguageId, std::string name)
    : typeId(typeId), localLanguageId(localLanguageId), name(name) {}


// Override the print method for TypeNames
void TypeNames::printObject() const {
    std::cout << "Type ID: " << typeId << "\n"
              << "Local Language ID: " << localLanguageId << "\n"
              << "Name: " << name << std::endl;
}

//set the fields in the constructor 
TypeNamesList::TypeNamesList(const std::string& filename) : DataObjectList(filename) {
    // Now that the base class constructor has read the CSV data into `data`,
    // call setFields to initialize the objects based on that data.
    setFields(data);
}

// Override the pure virtual function setFields
void TypeNamesList::setFields(const std::vector<std::vector<std::string>>& newData) {
    bool firstRowIsHeader = true; // Assuming the first row is always a header
    for (const auto& row : newData) {
        if (firstRowIsHeader) {
            firstRowIsHeader = false;
            continue;
        }
        if (row.size() != 3) { // Ensure the row has the correct number of fields for a TypeNames object
            throw std::invalid_argument("Row does not contain the correct number of fields for a TypeNames object.");
        }
        try {
            // Check if localLanguageId is 9 before creating the object
            int localLanguageId = row[1].empty() ? INT_MAX : std::stoi(row[1]);
            if (localLanguageId == 9) {
                int typeId = row[0].empty() ? INT_MAX : std::stoi(row[0]);
                std::string name = row[2]; // No need to check for empty, it's a string

                // Add the new TypeNames object to the objects vector
                objects.push_back(std::make_unique<TypeNames>(typeId, localLanguageId, name));
            }
        } catch (const std::invalid_argument& e) {
            std::cerr << "Invalid argument: " << e.what() << " for input string in TypeNames data." << std::endl;
        } catch (const std::out_of_range& e) {
            std::cerr << "Out of range: " << e.what() << " in TypeNames data." << std::endl;
        }
    }
}

void TypeNamesList::printData() const {
    if (objects.empty()) {
        std::cerr << "No data to print." << std::endl;
        return;
    }

    // Custom headers for the TypeNamesList
    std::vector<std::string> headers = {
        "Type ID", "Local Language ID", "Name"
    };

    // Lambda function to handle INT_MAX values
    auto intToString = [](int value) -> std::string {
        return value == INT_MAX ? "" : std::to_string(value);
    };

    // Determine the maximum width needed for each column
    std::vector<std::size_t> columnWidths(headers.size(), 0);
    for (std::size_t i = 0; i < headers.size(); ++i) {
        columnWidths[i] = headers[i].length();
    }
    for (const auto& obj : objects) {
        const TypeNames* typeName = dynamic_cast<const TypeNames*>(obj.get());
        if (typeName) {
            std::vector<std::string> fields = {
                intToString(typeName->getTypeId()),
                intToString(typeName->getLocalLanguageId()),
                typeName->getName()
            };
            for (std::size_t i = 0; i < fields.size(); ++i) {
                columnWidths[i] = std::max(columnWidths[i], fields[i].length());
            }
        }
    }

    // Print the headers with proper spacing
    for (std::size_t i = 0; i < headers.size(); ++i) {
        std::cout << std::left << std::setw(columnWidths[i] + 2) << headers[i]; // +2 for padding
    }
    std::cout << std::endl;

    // Print the data with aligned columns
    for (const auto& obj : objects) {
        const TypeNames* typeName = dynamic_cast<const TypeNames*>(obj.get());
        if (typeName) {
            std::cout << std::left << std::setw(columnWidths[0] + 2) << intToString(typeName->getTypeId())
                      << std::left << std::setw(columnWidths[1] + 2) << intToString(typeName->getLocalLanguageId())
                      << std::left << std::setw(columnWidths[2] + 2) << typeName->getName()
                      << std::endl;
        }
    }
}

/*Pokemon Stats*/
// PokemonStats class constructor implementation
PokemonStats::PokemonStats(int pokemonId, int statId, int baseStat, int effort)
    : pokemonId(pokemonId), statId(statId), baseStat(baseStat), effort(effort) {}

// Override the print method for PokemonStats
void PokemonStats::printObject() const {
    std::cout << "Pokemon ID: " << pokemonId << "\n"
              << "Stat ID: " << statId << "\n"
              << "Base Stat: " << baseStat << "\n"
              << "Effort: " << effort << std::endl;
}

//set the fields in the constructor 
PokemonStatsList::PokemonStatsList(const std::string& filename) : DataObjectList(filename) {
    // Now that the base class constructor has read the CSV data into `data`,
    // call setFields to initialize the objects based on that data.
    setFields(data);
}

// PokemonStatsList setFields override
void PokemonStatsList::setFields(const std::vector<std::vector<std::string>>& newData) {
    bool firstRowIsHeader = true; // Assuming the first row is always a header
    for (const auto& row : newData) {
        if (firstRowIsHeader) {
            firstRowIsHeader = false;
            continue;
        }
        if (row.size() != 4) { // Ensure the row has the correct number of fields for a PokemonStats object
            throw std::invalid_argument("Row does not contain the correct number of fields for a PokemonStats object.");
        }
        try {
            // Convert each string to the appropriate type and create a new PokemonStats object
            int pokemonId = row[0].empty() ? INT_MAX : std::stoi(row[0]);
            int statId = row[1].empty() ? INT_MAX : std::stoi(row[1]);
            int baseStat = row[2].empty() ? INT_MAX : std::stoi(row[2]);
            int effort = row[3].empty() ? INT_MAX : std::stoi(row[3]);

            // Add the new PokemonStats object to the objects vector
            objects.push_back(std::make_unique<PokemonStats>(pokemonId, statId, baseStat, effort));
        } catch (const std::invalid_argument& e) {
            std::cerr << "Invalid argument: " << e.what() << " for input string in PokemonStats data." << std::endl;
        } catch (const std::out_of_range& e) {
            std::cerr << "Out of range: " << e.what() << " in PokemonStats data." << std::endl;
        }
    }
}

void PokemonStatsList::printData() const {
    if (objects.empty()) {
        std::cerr << "No data to print." << std::endl;
        return;
    }

    // Custom headers for the PokemonStatsList
    std::vector<std::string> headers = {
        "Pokemon ID", "Stat ID", "Base Stat", "Effort"
    };

    // Lambda function to handle INT_MAX values
    auto intToString = [](int value) -> std::string {
        return value == INT_MAX ? "" : std::to_string(value);
    };

    // Determine the maximum width needed for each column
    std::vector<std::size_t> columnWidths(headers.size(), 0);
    for (std::size_t i = 0; i < headers.size(); ++i) {
        columnWidths[i] = headers[i].length();
    }
    for (const auto& obj : objects) {
        const PokemonStats* pokemonStat = dynamic_cast<const PokemonStats*>(obj.get());
        if (pokemonStat) {
            std::vector<std::string> fields = {
                intToString(pokemonStat->getPokemonId()),
                intToString(pokemonStat->getStatId()),
                intToString(pokemonStat->getBaseStat()),
                intToString(pokemonStat->getEffort())
            };
            for (std::size_t i = 0; i < fields.size(); ++i) {
                columnWidths[i] = std::max(columnWidths[i], fields[i].length());
            }
        }
    }

    // Print the headers with proper spacing
    for (std::size_t i = 0; i < headers.size(); ++i) {
        std::cout << std::left << std::setw(columnWidths[i] + 2) << headers[i]; // +2 for padding
    }
    std::cout << std::endl;

    // Print the data with aligned columns
    for (const auto& obj : objects) {
        const PokemonStats* pokemonStat = dynamic_cast<const PokemonStats*>(obj.get());
        if (pokemonStat) {
            std::cout << std::left << std::setw(columnWidths[0] + 2) << intToString(pokemonStat->getPokemonId())
                      << std::left << std::setw(columnWidths[1] + 2) << intToString(pokemonStat->getStatId())
                      << std::left << std::setw(columnWidths[2] + 2) << intToString(pokemonStat->getBaseStat())
                      << std::left << std::setw(columnWidths[3] + 2) << intToString(pokemonStat->getEffort())
                      << std::endl;
        }
    }
}


/*Stats stuff*/
// Stats class constructor implementation
Stats::Stats(int id, int damageClassId, std::string identifier, int isBattleOnly, int gameIndex)
    : id(id), damageClassId(damageClassId), identifier(identifier), isBattleOnly(isBattleOnly), gameIndex(gameIndex) {}

// Override the print method for Stats
void Stats::printObject() const {
    std::cout << "ID: " << id << "\n"
              << "Damage Class ID: " << damageClassId << "\n"
              << "Identifier: " << identifier << "\n"
              << "Is Battle Only: " << isBattleOnly << "\n"
              << "Game Index: " << gameIndex << std::endl;
}

//set the fields in the constructor 
StatsList::StatsList(const std::string& filename) : DataObjectList(filename) {
    // Now that the base class constructor has read the CSV data into `data`,
    // call setFields to initialize the objects based on that data.
    setFields(data);
}

// StatsList setFields override
void StatsList::setFields(const std::vector<std::vector<std::string>>& newData) {
    bool firstRowIsHeader = true; // Assuming the first row is always a header
    for (const auto& row : newData) {
        if (firstRowIsHeader) {
            firstRowIsHeader = false;
            continue;
        }
        if (row.size() != 5) { // Ensure the row has the correct number of fields for a Stats object
            throw std::invalid_argument("Row does not contain the correct number of fields for a Stats object.");
        }
        try {
            // Convert each string to the appropriate type and create a new Stats object
            int id = row[0].empty() ? INT_MAX : std::stoi(row[0]);
            int damageClassId = row[1].empty() ? INT_MAX : std::stoi(row[1]);
            std::string identifier = row[2]; // No need to check for empty, it's a string
            int isBattleOnly = row[3].empty() ? INT_MAX : std::stoi(row[3]);
            int gameIndex = row[4].empty() ? INT_MAX : std::stoi(row[4]);

            // Add the new Stats object to the objects vector
            objects.push_back(std::make_unique<Stats>(id, damageClassId, identifier, isBattleOnly, gameIndex));
        } catch (const std::invalid_argument& e) {
            std::cerr << "Invalid argument: " << e.what() << " for input string in Stats data." << std::endl;
        } catch (const std::out_of_range& e) {
            std::cerr << "Out of range: " << e.what() << " in Stats data." << std::endl;
        }
    }
}

void StatsList::printData() const {
    if (objects.empty()) {
        std::cerr << "No data to print." << std::endl;
        return;
    }

    // Custom headers for the StatsList
    std::vector<std::string> headers = {
        "ID", "Damage Class ID", "Identifier", "Is Battle Only", "Game Index"
    };

    // Lambda function to handle INT_MAX values
    auto intToString = [](int value) -> std::string {
        return value == INT_MAX ? "" : std::to_string(value);
    };

    // Determine the maximum width needed for each column
    std::vector<std::size_t> columnWidths(headers.size(), 0);
    for (std::size_t i = 0; i < headers.size(); ++i) {
        columnWidths[i] = headers[i].length();
    }
    for (const auto& obj : objects) {
        const Stats* stat = dynamic_cast<const Stats*>(obj.get());
        if (stat) {
            std::vector<std::string> fields = {
                intToString(stat->getId()),
                intToString(stat->getDamageClassId()),
                stat->getIdentifier(),
                intToString(stat->getIsBattleOnly()),
                intToString(stat->getGameIndex())
            };
            for (std::size_t i = 0; i < fields.size(); ++i) {
                columnWidths[i] = std::max(columnWidths[i], fields[i].length());
            }
        }
    }

    // Print the headers with proper spacing
    for (std::size_t i = 0; i < headers.size(); ++i) {
        std::cout << std::left << std::setw(columnWidths[i] + 2) << headers[i]; // +2 for padding
    }
    std::cout << std::endl;

    // Print the data with aligned columns
    for (const auto& obj : objects) {
        const Stats* stat = dynamic_cast<const Stats*>(obj.get());
        if (stat) {
            std::cout << std::left << std::setw(columnWidths[0] + 2) << intToString(stat->getId())
                      << std::left << std::setw(columnWidths[1] + 2) << intToString(stat->getDamageClassId())
                      << std::left << std::setw(columnWidths[2] + 2) << stat->getIdentifier()
                      << std::left << std::setw(columnWidths[3] + 2) << intToString(stat->getIsBattleOnly())
                      << std::left << std::setw(columnWidths[4] + 2) << intToString(stat->getGameIndex())
                      << std::endl;
        }
    }
}





/*PokemonTypes stuff*/
// PokemonTypes class constructor implementation
PokemonTypes::PokemonTypes(int pokemonId, int typeId, int slot)
    : pokemonId(pokemonId), typeId(typeId), slot(slot) {}

// Override the print method for PokemonTypes
void PokemonTypes::printObject() const {
    std::cout << "Pokemon ID: " << pokemonId << "\n"
              << "Type ID: " << typeId << "\n"
              << "Slot: " << slot << std::endl;
}

//set the fields in the constructor 
PokemonTypesList::PokemonTypesList(const std::string& filename) : DataObjectList(filename) {
    // Now that the base class constructor has read the CSV data into `data`,
    // call setFields to initialize the objects based on that data.
    setFields(data);
}

// PokemonTypesList setFields override
void PokemonTypesList::setFields(const std::vector<std::vector<std::string>>& newData) {
    bool firstRowIsHeader = true; // Assuming the first row is always a header
    for (const auto& row : newData) {
        if (firstRowIsHeader) {
            firstRowIsHeader = false;
            continue;
        }
        if (row.size() != 3) { // Ensure the row has the correct number of fields for a PokemonTypes object
            throw std::invalid_argument("Row does not contain the correct number of fields for a PokemonTypes object.");
        }
        try {
            // Convert each string to the appropriate type and create a new PokemonTypes object
            int pokemonId = row[0].empty() ? INT_MAX : std::stoi(row[0]);
            int typeId = row[1].empty() ? INT_MAX : std::stoi(row[1]);
            int slot = row[2].empty() ? INT_MAX : std::stoi(row[2]);

            // Add the new PokemonTypes object to the objects vector
            objects.push_back(std::make_unique<PokemonTypes>(pokemonId, typeId, slot));
        } catch (const std::invalid_argument& e) {
            std::cerr << "Invalid argument: " << e.what() << " for input string in PokemonTypes data." << std::endl;
        } catch (const std::out_of_range& e) {
            std::cerr << "Out of range: " << e.what() << " in PokemonTypes data." << std::endl;
        }
    }
}

void PokemonTypesList::printData() const {
    if (objects.empty()) {
        std::cerr << "No data to print." << std::endl;
        return;
    }

    // Custom headers for the PokemonTypesList
    std::vector<std::string> headers = {
        "Pokemon ID", "Type ID", "Slot"
    };

    // Lambda function to handle INT_MAX values
    auto intToString = [](int value) -> std::string {
        return value == INT_MAX ? "" : std::to_string(value);
    };

    // Determine the maximum width needed for each column
    std::vector<std::size_t> columnWidths(headers.size(), 0);
    for (std::size_t i = 0; i < headers.size(); ++i) {
        columnWidths[i] = headers[i].length();
    }
    for (const auto& obj : objects) {
        const PokemonTypes* pokemonType = dynamic_cast<const PokemonTypes*>(obj.get());
        if (pokemonType) {
            std::vector<std::string> fields = {
                intToString(pokemonType->getPokemonId()),
                intToString(pokemonType->getTypeId()),
                intToString(pokemonType->getSlot())
            };
            for (std::size_t i = 0; i < fields.size(); ++i) {
                columnWidths[i] = std::max(columnWidths[i], fields[i].length());
            }
        }
    }

    // Print the headers with proper spacing
    for (std::size_t i = 0; i < headers.size(); ++i) {
        std::cout << std::left << std::setw(columnWidths[i] + 2) << headers[i]; // +2 for padding
    }
    std::cout << std::endl;

    // Print the data with aligned columns
    for (const auto& obj : objects) {
        const PokemonTypes* pokemonType = dynamic_cast<const PokemonTypes*>(obj.get());
        if (pokemonType) {
            std::cout << std::left << std::setw(columnWidths[0] + 2) << intToString(pokemonType->getPokemonId())
                      << std::left << std::setw(columnWidths[1] + 2) << intToString(pokemonType->getTypeId())
                      << std::left << std::setw(columnWidths[2] + 2) << intToString(pokemonType->getSlot())
                      << std::endl;
        }
    }
}





void printPokemonMovesList(const std::vector<PokemonMoves>& pokeMoves) {
    if (pokeMoves.empty()) {
        std::cerr << "No data to print." << std::endl;
        return;
    }

    // Custom headers for the PokemonMovesList
    std::vector<std::string> headers = {
        "Pokemon ID", "Version Group ID", "Move ID", "Pokemon Move Method ID", "Level", "Order"
    };

    
    // Lambda function to handle INT_MAX values
    auto intToString = [](int value) -> std::string {
        return value == INT_MAX ? "" : std::to_string(value);
    };

    // Determine the maximum width needed for each column
    std::vector<std::size_t> columnWidths(headers.size(), 0);
    for (std::size_t i = 0; i < headers.size(); ++i) {
        columnWidths[i] = headers[i].length();
    }
    for (const auto& pokemonMove : pokeMoves) {
        std::vector<std::string> fields = {
            intToString(pokemonMove.getPokemonId()),
            intToString(pokemonMove.getVersionGroupId()),
            intToString(pokemonMove.getMoveId()),
            intToString(pokemonMove.getPokemonMoveMethodId()),
            intToString(pokemonMove.getLevel()),
            intToString(pokemonMove.getOrder())
        };
        for (std::size_t i = 0; i < fields.size(); ++i) {
            columnWidths[i] = std::max(columnWidths[i], fields[i].length());
        }
    }

    // Print the headers with proper spacing
    for (std::size_t i = 0; i < headers.size(); ++i) {
        std::cout << std::left << std::setw(columnWidths[i] + 2) << headers[i]; // +2 for padding
    }
    std::cout << std::endl;

    // Print the data with aligned columns
    for (const auto& pokemonMove : pokeMoves) {
        std::cout << std::left << std::setw(columnWidths[0] + 2) << intToString(pokemonMove.getPokemonId())
                  << std::left << std::setw(columnWidths[1] + 2) << intToString(pokemonMove.getVersionGroupId())
                  << std::left << std::setw(columnWidths[2] + 2) << intToString(pokemonMove.getMoveId())
                  << std::left << std::setw(columnWidths[3] + 2) << intToString(pokemonMove.getPokemonMoveMethodId())
                  << std::left << std::setw(columnWidths[4] + 2) << intToString(pokemonMove.getLevel())
                  << std::left << std::setw(columnWidths[5] + 2) << intToString(pokemonMove.getOrder())
                  << std::endl;
    }
}