#ifndef IO_H
# define IO_H
#include "data.h"
#include "prints.h"
class character;
class pc;
class npc;
struct PokeData;
typedef int16_t pair_t[2];

void io_init_terminal(void);
void io_reset_terminal(void);
void io_display(void);
void io_handle_input(pair_t dest);
void io_queue_message(const char *format, ...);
void io_battle(character *aggressor, character *defender);
void io_display_found_pokemon(pc* playerChar, PokeData& data);
void io_display_pokemon(const PokeData& data);
void io_display_choose_starter_pokemon(const std::vector<PokeData>& startingPokemons, character& c);
void io_pokemon_battle_screen(pc* playerCharacter, PokeData* playerPokemon, npc* nonPlayerCharacter, PokeData* nonPlayerPokemon);
void io_battle_moves(pc* playerCharacter, PokeData* playerPokemon, npc* nonPlayerCharacter, PokeData* nonPlayerPokemon);
void io_display_move_results(const Moves* pcMove, const Moves* npcMove, int npcDam, int pcDam, bool pcCrit, bool npcCrit, bool npcHit, bool pcHit);
PokeData* io_pokemon_list(character *aggressor);
void io_display_potion_results(int healthRegen, const Moves* npcMove, int npcDam, bool npcCrit, bool npcHit);
void io_show_bag_in_battle(pc *playerCharacter, PokeData *playerPokemon, npc *nonPlayerCharacter, PokeData *nonPlayerPokemon, int type);
PokeData *io_pokemon_fainted_list(character *aggressor);
void io_display_revive_results(const Moves* npcMove, int npcDam, bool npcCrit, bool npcHit);
void io_display_switch_results(const Moves *npcMove, int npcDam, bool npcCrit, bool npcHit);
#endif
