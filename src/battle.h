#ifndef BATTLE_H
#define BATTLE_H

#include "data.h"
#include "io.h"
#include "character.h"
#include <random>
bool hasTriedToFlee = false;

int handle_init_battle_choice(int playerChoice, pc* playerCharacter, PokeData*& playerPokemon, PokeData* npcPokemon, npc* nonPlayerCharacter) {
     switch (playerChoice) {
        case 1: // Fight
            if (playerPokemon && !playerPokemon->getPokemon().getIdentifier().empty()) {
                hasTriedToFlee = false; //reset for next encounter
                return 0; // Fight selected
            } else {
                mvprintw(8, 0, "You have not chosen a pokemon!! Press a key to remove this message.");
                refresh();
                getch();
                return 3; // No fight selected, continue loop
            }
        case 2: // Flee
            if (!hasTriedToFlee) {
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> dis(1, 100);
                int chance = dis(gen);
                if (chance <= 20) { // 20% chance to flee successfully
                    hasTriedToFlee = false; //reset for next encounter
                    return 1; // Fleeing successful
                } else {
                    hasTriedToFlee = true;
                    mvprintw(8, 0, "Failed to flee! Press a key to remove this message.");
                    refresh();
                    getch();
                    return 3; // Fleeing failed, continue loop
                }
            }else{
                mvprintw(8, 0, "Already tried to flee! Press a key to remove this message.");
            }
        case 3: // Revive or choose a Pokemon
            playerPokemon = io_pokemon_list(playerCharacter); // Choose a Pokemon
            return 3; // No fight selected, continue loop
        default:
            mvprintw(8, 0, "Invalid option. Please choose again. Press a key to remove this message.");
            refresh();
            getch();
            return 3; // Invalid option, continue loop
    }
}

// Helper function to check if a Pokemon has fainted (HP is zero)
bool isFainted(PokeData* pokeData) {
    // Lambda to find the HP stat
    auto findHP = [](const std::vector<PokeStats>& stats) -> const PokeStats* {
        for (const auto& stat : stats) {
            if (stat.getStatIdentifier() == "hp") {
                return &stat;
            }
        }
        return nullptr; // HP stat not found
    };
    const PokeStats* hpStat = findHP(pokeData->getStatsList());
    return hpStat && hpStat->getStatRating() <= 0;
}

void applyDamage(PokeData* poke, int damage){

     // Lambda to find the HP stat
    auto findHP = [](const std::vector<PokeStats>& stats) -> const PokeStats* {
        for (const auto& stat : stats) {
            if (stat.getStatIdentifier() == "hp") {
                return &stat;
            }
        }
        return nullptr; // HP stat not found
    };

    // Find HP stat for the Pokemon
    const PokeStats* hpStat = findHP(poke->getStatsList());

    if (hpStat != nullptr) {
        // Calculate new HP, ensuring it doesn't go below 0
        int newHP = std::max(0, hpStat->getStatRating() - damage);
        // Set the new HP using modifyStat
        poke->modifyStat("hp", newHP);
    }


}

bool isCritical(){
    //random chance (for now 50% chance)
    return true;
}

int processMoveDamage(PokeData* pokeData, const Moves* move, bool isCritical){
    //exp1=(2*level)/5 +2
    //exp2 = exp1 * power * (attack/defense)
    //exp3 = (exp2/50 + 2) * critical * random 
    //where random is uniformially distributed number [85-100]
    // critical is 1.5 if ture of 1 if not

    // Placeholder values for level, attack, and defense
    int level = 50; // Example level
    int attack = 100; // Example attack stat
    int defense = 100; // Example defense stat

    int exp1 = (2 * level) / 5 + 2;
    int exp2 = exp1 * move->getPower() * (attack / defense);
    double critical = isCritical ? 1.5 : 1.0;
    int randomFactor = (rand() % (100 - 85 + 1)) + 85; // Random number between 85 and 100
    int exp3 = static_cast<int>(((exp2 / 50 + 2) * critical * randomFactor) / 100.0);

    return exp3;

}

//for when the player chooses to use a move
void processMove(pc* playerCharacter, PokeData* playerPokemon, npc* nonPlayerCharacter, PokeData* nonPlayerPokemon, const Moves* playerMove){
    
        // Choose a move for the NPC (randomly from their move set in their PokeData)
    const std::vector<Moves>& npcMoves = nonPlayerPokemon->getMovesList();
    const Moves* npcMove = &npcMoves[rand() % npcMoves.size()];

    // Determine which move goes first by which has a higher priority
    bool playerGoesFirst = playerMove->getPriority() >= npcMove->getPriority();

    // Placeholder variables for damage and critical hit flags
    int npcDamage = 0, playerDamage = 0;
    bool pcCrit = false, npcCrit = false, npcHit = false, pcHit = false;

    // Process the moves based on priority
    if (playerGoesFirst) {
        // Process the player's move
        pcHit = playerMove->getAccuracy() == -1 || (rand() % 100) <= playerMove->getAccuracy();
        if (pcHit) {
            pcCrit = isCritical();
            playerDamage = processMoveDamage(playerPokemon, playerMove, pcCrit);
            applyDamage(nonPlayerPokemon, playerDamage);
        }
        // Check if the NPC's Pokemon has fainted
        if (!isFainted(nonPlayerPokemon)) {
            // Process the NPC's move
            npcHit = (rand() % 100) <= npcMove->getAccuracy();
            if (npcHit) {
                npcCrit = isCritical();
                npcDamage = processMoveDamage(nonPlayerPokemon, npcMove, npcCrit);
                applyDamage(playerPokemon, npcDamage);
            }
        }
    } else {
        // Process the NPC's move
         npcHit = npcMove->getAccuracy() == -1 || (rand() % 100) <= npcMove->getAccuracy();
        if (npcHit) {
            npcCrit = isCritical();
            npcDamage = processMoveDamage(nonPlayerPokemon, npcMove, npcCrit);
            applyDamage(playerPokemon, npcDamage);
        }
        // Check if the player's Pokemon has fainted
        if (!isFainted(playerPokemon)) {
            // Process the player's move
            pcHit = (rand() % 100) <= playerMove->getAccuracy();
            if (pcHit) {
                pcCrit = isCritical();
                playerDamage = processMoveDamage(playerPokemon, playerMove, pcCrit);
                applyDamage(nonPlayerPokemon, playerDamage);
            }
        }
    }

    io_display_move_results(playerMove, npcMove, npcDamage, playerDamage, pcCrit, npcCrit, npcHit, pcHit);
}

void processNPCMovePotion(pc* playerCharacter, PokeData* playerPokemon, npc* nonPlayerCharacter, PokeData* nonPlayerPokemon, int healthRegen){
        // Choose a move for the NPC (randomly from their move set in their PokeData)
    const std::vector<Moves>& npcMoves = nonPlayerPokemon->getMovesList();
    const Moves* npcMove = &npcMoves[rand() % npcMoves.size()];
    // Placeholder variables for damage and critical hit flags
    int npcDamage = 0;
    bool npcCrit = false, npcHit = false;

     // Process the NPC's move
         npcHit = npcMove->getAccuracy() == -1 || (rand() % 100) <= npcMove->getAccuracy();
        if (npcHit) {
            npcCrit = isCritical();
            npcDamage = processMoveDamage(nonPlayerPokemon, npcMove, npcCrit);
            applyDamage(playerPokemon, npcDamage);
        }
      

    io_display_potion_results(healthRegen, npcMove, npcDamage, npcCrit, npcHit);
}

void processNPCMoveRevive(pc* playerCharacter, PokeData* playerPokemon, npc* nonPlayerCharacter, PokeData* nonPlayerPokemon){
        // Choose a move for the NPC (randomly from their move set in their PokeData)
    const std::vector<Moves>& npcMoves = nonPlayerPokemon->getMovesList();
    const Moves* npcMove = &npcMoves[rand() % npcMoves.size()];
    // Placeholder variables for damage and critical hit flags
    int npcDamage = 0;
    bool npcCrit = false, npcHit = false;

     // Process the NPC's move
         npcHit = npcMove->getAccuracy() == -1 || (rand() % 100) <= npcMove->getAccuracy();
        if (npcHit) {
            npcCrit = isCritical();
            npcDamage = processMoveDamage(nonPlayerPokemon, npcMove, npcCrit);
            applyDamage(playerPokemon, npcDamage);
        }
      

    io_display_revive_results(npcMove, npcDamage, npcCrit, npcHit);
}

void processNPCMoveSwitch(pc* playerCharacter, PokeData* playerPokemon, npc* nonPlayerCharacter, PokeData* nonPlayerPokemon){
        // Choose a move for the NPC (randomly from their move set in their PokeData)
    const std::vector<Moves>& npcMoves = nonPlayerPokemon->getMovesList();
    const Moves* npcMove = &npcMoves[rand() % npcMoves.size()];
    // Placeholder variables for damage and critical hit flags
    int npcDamage = 0;
    bool npcCrit = false, npcHit = false;

     // Process the NPC's move
         npcHit = npcMove->getAccuracy() == -1 || (rand() % 100) <= npcMove->getAccuracy();
        if (npcHit) {
            npcCrit = isCritical();
            npcDamage = processMoveDamage(nonPlayerPokemon, npcMove, npcCrit);
            applyDamage(playerPokemon, npcDamage);
        }
      

    io_display_switch_results(npcMove, npcDamage, npcCrit, npcHit);
}

void usePokeBall(character* player, PokeData* nonPlayerPokemon){
     pc* playerCharacter = dynamic_cast<pc*>(player);
    if (playerCharacter) {
        // Add the nonPlayerPokemon to the player's list of Pokemon
        playerCharacter->addPokemon(*nonPlayerPokemon);

        // Decrement the player's number of Pokeballs
        playerCharacter->usePokeball(); // Assumes usePokeball() decrements the count and returns true if successful
    } else {
        // Handle the error if the cast fails (player is not a pc)
        // This could happen if usePokeBall is mistakenly called with an npc instead of a pc
    }
}




int usePotion(pc* playerCharacter, PokeData* playerPokemon, Potion* potion){
    if (!playerCharacter || !playerPokemon || !potion) {
        return 0; // Invalid parameters, no potion used
    }

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

    // Apply the potion's healing effect
    PokeStats* hpStat = findHP(playerPokemon->getStatsList());
    const PokeStats* maxHPStat = findMaxHP(playerPokemon->getStatsList());
    if (hpStat && maxHPStat) {
        int newHP = std::min(hpStat->getStatRating() + potion->getHealingAmount(), maxHPStat->getStatRating());
        hpStat->setStatRating(newHP);
    }

    // Find and remove the used potion from the character's potions list
    auto& potions = playerCharacter->getPotions();
    auto it = std::find(potions.begin(), potions.end(), potion);
    if (it != potions.end()) {
        potions.erase(it); // Remove the pointer from the list
        // Do not delete the potion here; let the character destructor handle it
    }

    return potion->getHealingAmount(); // Return the amount of health restored
}


//for when the player chooses to use the bag in battle ()
void processBagInGame(pc* playerCharacter, PokeData* playerPokemon, npc* nonPlayerCharacter, PokeData* nonPlayerPokemon, Potion* potion, int pokeBall, int revive){

   // If pokeBall > 0, then player has chosen a Pokeball
    if (pokeBall > 0 && playerCharacter->getPokeballCount() > 0) {
        usePokeBall(playerCharacter, nonPlayerPokemon);
        //processNPCMove(playerCharacter, playerPokemon, nonPlayerCharacter, nonPlayerPokemon); (pokemon do not attack when capturing)
        //io_display_capture_results(nonPlayerPokemon); (this will be implmented in another screen to show that the player has captured this pokemon)
    }

    // If potion is not nullptr, then player has chosen a potion
    if (potion != nullptr) {
        int healthRegen = usePotion(playerCharacter ,playerPokemon, potion);
        processNPCMovePotion(playerCharacter, playerPokemon, nonPlayerCharacter, nonPlayerPokemon, healthRegen);
        //io_display_potion_results(healthRegen, npcMove, npcDamage, npcCrit, npcHit);
    }

     // If revive > 0, then player has chosen a revive
    if (revive > 0 && playerCharacter->getReviveCount() > 0) {
        // Let the player select a fainted Pokemon to revive
        PokeData* faintedPokemon = io_pokemon_fainted_list(playerCharacter);
        if (faintedPokemon) {
            playerCharacter->revivePokemon(faintedPokemon); // Revive the selected Pokemon
            processNPCMoveRevive(playerCharacter, playerPokemon, nonPlayerCharacter, nonPlayerPokemon);
            // Additional logic for reviving...
        }
    }

}

//void processPokemonSwitchMove()

//forr when the player chooses to switch pokemon in battle 
//void processPokemonSwitch();


#endif