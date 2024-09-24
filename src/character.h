#ifndef CHARACTER_H
#define CHARACTER_H

#include <cstdint>

#include "pair.h"
#include "prints.h" // Include the header where PokeData is defined
#define DIJKSTRA_PATH_MAX (INT_MAX / 2)
#define NO_NPCS 50

class Potion
{
public:
  virtual ~Potion() {}
  virtual int getHealingAmount() const = 0;
  virtual std::string getSize() const = 0;
};

class SmallPotion : public Potion
{
public:
  int getHealingAmount() const override
  {
    return 20; // Example healing amount
  }
  std::string getSize() const override
  {
    return "Small";
  }
};

class MediumPotion : public Potion
{
public:
  int getHealingAmount() const override
  {
    return 50; // Example healing amount
  }
  std::string getSize() const override
  {
    return "Medium";
  }
};

class LargePotion : public Potion
{
public:
  int getHealingAmount() const override
  {
    return 100; // Example healing amount
  }
  std::string getSize() const override
  {
    return "Large";
  }
};

typedef enum __attribute__((__packed__)) movement_type
{
  move_hiker,
  move_rival,
  move_pace,
  move_wander,
  move_sentry,
  move_explore,
  move_swim,
  move_pc,
  num_movement_types
} movement_type_t;

typedef enum __attribute__((__packed__)) character_type
{
  char_pc,
  char_hiker,
  char_rival,
  char_swimmer,
  char_other,
  num_character_types
} character_type_t;

extern const char *char_type_name[num_character_types];

class character
{
public:
  virtual ~character()
  {
    for (auto potion : potions)
    {
      delete potion;
    }
  }
  pair_t pos;
  char symbol;
  int next_turn;
  int seq_num;
  std::vector<PokeData> pokemons;
  std::vector<Potion *> potions; // Vector to hold potions

  character()
  {
    // Initialize character with 5 potions
    potions.push_back(new SmallPotion());
    potions.push_back(new SmallPotion());
    potions.push_back(new SmallPotion());
    potions.push_back(new MediumPotion());
    potions.push_back(new LargePotion());
  }

  void addPokemon(const PokeData &pokemon)
  {
    pokemons.push_back(pokemon);
  }

  void removePokemon(size_t index)
  {
    if (index < pokemons.size())
    {
      pokemons.erase(pokemons.begin() + index);
    }
    else
    {
      throw std::out_of_range("Index out of range");
    }
  }

  const std::vector<PokeData> &getPokemons() const
  {
    return pokemons;
  }

  std::vector<PokeData> &getPokemons()
  {
    return pokemons;
  }

  // Method to replenish potions to their initial state
  void replenishPotions()
  {
    clearPotions();
    initializePotions();
  }

   std::vector<Potion*>& getPotions() {
        return potions;
    }

    const std::vector<Potion*>& getPotions() const {
        return potions;
    }

private:
  // Helper method to initialize potions
  void initializePotions()
  {
    potions.push_back(new SmallPotion());
    potions.push_back(new SmallPotion());
    potions.push_back(new SmallPotion());
    potions.push_back(new MediumPotion());
    potions.push_back(new LargePotion());
  }

  // Helper method to clear current potions
  void clearPotions()
  {
    for (auto potion : potions)
    {
      delete potion;
    }
    potions.clear();
  }
};

class npc : public character
{
public:
  character_type_t ctype;
  movement_type_t mtype;
  int defeated;
  pair_t dir;
};

class pc : public character
{
public:
  int currentPokeBalls;
    int initialPokeBalls;
    int currentRevives;
    int initialRevives;

  // Constructor to initialize the PC with a specific number of initial Pokéballs and Revives
    pc(int startPokeBalls = 5, int startRevives = 3) : currentPokeBalls(startPokeBalls), initialPokeBalls(startPokeBalls), currentRevives(startRevives), initialRevives(startRevives) {
        // Initialize other members as needed
    }

    // Existing methods for Pokéballs...

    // Method to use a revive, if available
    bool useRevive() {
        if (currentRevives > 0) {
            currentRevives--;
            return true; // Successfully used a revive
        }
        return false; // No revives left to use
    }

    // Method to replenish revives to the initial number
    void replenishRevives() {
        currentRevives = initialRevives;
    }

    // Method to get the current count of revives
    int getReviveCount() const {
        return currentRevives;
    }

  // Method to replenish Pokéballs to the initial number
  void replenishPokeBalls()
  {
    currentPokeBalls = initialPokeBalls;
  }

  // Method to use a Pokéball, if available
  bool usePokeball()
  {
    if (currentPokeBalls > 0)
    {
      currentPokeBalls--;
      return true; // Successfully used a Pokéball
    }
    return false; // No Pokéballs left to use
  }

  // Method to get the current count of Pokéballs
  int getPokeballCount() const
  {
    return currentPokeBalls;
  }

  void healAllPokemon()
  {
    auto findMaxHP = [](const std::vector<PokeStats> &stats) -> const PokeStats *
    {
      for (const auto &stat : stats)
      {
        if (stat.getStatIdentifier() == "maxHP")
        {
          return &stat;
        }
      }
      return nullptr; // maxHP stat not found
    };

    for (auto &pokemon : pokemons)
    {
      const PokeStats *maxHPStat = findMaxHP(pokemon.getStatsList());
      if (maxHPStat)
      {
        // Find the HP stat and update its rating to match maxHP's rating
        auto it = std::find_if(pokemon.getStatsList().begin(), pokemon.getStatsList().end(),
                               [](const PokeStats &stat)
                               { return stat.getStatIdentifier() == "hp"; });
        if (it != pokemon.getStatsList().end())
        {
          it->setStatRating(maxHPStat->getStatRating());
        }
      }
    }
  }

  // Method to revive a fainted Pokemon (if you have revives)
    void revivePokemon(PokeData* faintedPokemon) {
    if (useRevive()) {
        // Ensure the lambda functions return non-const pointers for modification
        auto findHP = [](std::vector<PokeStats>& stats) -> PokeStats* {
            for (auto& stat : stats) {
                if (stat.getStatIdentifier() == "hp") {
                    return &stat;
                }
            }
            return nullptr; // HP stat not found
        };

        auto findMaxHP = [](const std::vector<PokeStats>& stats) -> const PokeStats* {
            for (const auto& stat : stats) {
                if (stat.getStatIdentifier() == "maxHP") {
                    return &stat;
                }
            }
            return nullptr; // maxHP stat not found
        };

        // Use non-const getStatsList() for modification
        PokeStats* hpStat = findHP(faintedPokemon->getStatsList());
        const PokeStats* maxHPStat = findMaxHP(faintedPokemon->getStatsList());

        // Revive the Pokemon to half of its max HP
        if (hpStat && maxHPStat) {
            int revivedHP = maxHPStat->getStatRating() / 2;
            hpStat->setStatRating(revivedHP);
        }
    }
}
};



/* character is defined in poke327.h to allow an instance of character
 * in world without including character.h in poke327.h                 */

int32_t cmp_char_turns(const void *key, const void *with);
void delete_character(void *v);

extern void (*move_func[num_movement_types])(character *, pair_t);

int pc_move(char);
bool is_pc(character *c);

#endif
