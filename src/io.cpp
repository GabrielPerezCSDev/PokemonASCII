#include <unistd.h>
#include <ncurses.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
#include <cstring>
#include <map>

#include "io.h"
#include "character.h"
#include "poke_main.h"
#include "battle.h"
#define TRAINER_LIST_FIELD_WIDTH 46

int displayHeight;

typedef struct io_message
{
  /* Will print " --more-- " at end of line when another message follows. *
   * Leave 10 extra spaces for that.                                      */
  char msg[71];
  struct io_message *next;
} io_message_t;

static io_message_t *io_head, *io_tail;

void io_init_terminal(void)
{
  initscr();
  raw();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);
  start_color();
  init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
  init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
  init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
  init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
  init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
  init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
}

void io_reset_terminal(void)
{
  endwin();

  while (io_head)
  {
    io_tail = io_head;
    io_head = io_head->next;
    free(io_tail);
  }
  io_tail = NULL;
}

void io_queue_message(const char *format, ...)
{
  io_message_t *tmp;
  va_list ap;

  if (!(tmp = (io_message_t *)malloc(sizeof(*tmp))))
  {
    perror("malloc");
    exit(1);
  }

  tmp->next = NULL;

  va_start(ap, format);

  vsnprintf(tmp->msg, sizeof(tmp->msg), format, ap);

  va_end(ap);

  if (!io_head)
  {
    io_head = io_tail = tmp;
  }
  else
  {
    io_tail->next = tmp;
    io_tail = tmp;
  }
}

static void io_print_message_queue(uint32_t y, uint32_t x)
{
  while (io_head)
  {
    io_tail = io_head;
    attron(COLOR_PAIR(COLOR_CYAN));
    mvprintw(y, x, "%-80s", io_head->msg);
    attroff(COLOR_PAIR(COLOR_CYAN));
    io_head = io_head->next;
    if (io_head)
    {
      attron(COLOR_PAIR(COLOR_CYAN));
      mvprintw(y, x + 70, "%10s", " --more-- ");
      attroff(COLOR_PAIR(COLOR_CYAN));
      refresh();
      getch();
    }
    free(io_tail);
  }
  io_tail = NULL;
}

/**************************************************************************
 * Compares trainer distances from the PC according to the rival distance *
 * map.  This gives the approximate distance that the PC must travel to   *
 * get to the trainer (doesn't account for crossing buildings).  This is  *
 * not the distance from the NPC to the PC unless the NPC is a rival.     *
 *                                                                        *
 * Not a bug.                                                             *
 **************************************************************************/
static int compare_trainer_distance(const void *v1, const void *v2)
{
  const character *const *c1 = (const character *const *)v1;
  const character *const *c2 = (const character *const *)v2;

  return (world.rival_dist[(*c1)->pos[dim_y]][(*c1)->pos[dim_x]] -
          world.rival_dist[(*c2)->pos[dim_y]][(*c2)->pos[dim_x]]);
}

static character *io_nearest_visible_trainer()
{
  character **c, *n;
  uint32_t x, y, count;

  c = (character **)malloc(world.cur_map->num_trainers * sizeof(*c));

  /* Get a linear list of trainers */
  for (count = 0, y = 1; y < MAP_Y - 1; y++)
  {
    for (x = 1; x < MAP_X - 1; x++)
    {
      if (world.cur_map->cmap[y][x] && world.cur_map->cmap[y][x] !=
                                           &world.pc)
      {
        c[count++] = world.cur_map->cmap[y][x];
      }
    }
  }

  /* Sort it by distance from PC */
  qsort(c, count, sizeof(*c), compare_trainer_distance);

  n = c[0];

  free(c);

  return n;
}

void io_display()
{
  uint32_t y, x;
  character *c;

  clear();
  for (y = 0; y < MAP_Y; y++)
  {
    for (x = 0; x < MAP_X; x++)
    {
      if (world.cur_map->cmap[y][x])
      {
        mvaddch(y + 1, x, world.cur_map->cmap[y][x]->symbol);
      }
      else
      {
        switch (world.cur_map->map[y][x])
        {
        case ter_boulder:
          attron(COLOR_PAIR(COLOR_MAGENTA));
          mvaddch(y + 1, x, BOULDER_SYMBOL);
          attroff(COLOR_PAIR(COLOR_MAGENTA));
          break;
        case ter_mountain:
          attron(COLOR_PAIR(COLOR_MAGENTA));
          mvaddch(y + 1, x, MOUNTAIN_SYMBOL);
          attroff(COLOR_PAIR(COLOR_MAGENTA));
          break;
        case ter_tree:
          attron(COLOR_PAIR(COLOR_GREEN));
          mvaddch(y + 1, x, TREE_SYMBOL);
          attroff(COLOR_PAIR(COLOR_GREEN));
          break;
        case ter_forest:
          attron(COLOR_PAIR(COLOR_GREEN));
          mvaddch(y + 1, x, FOREST_SYMBOL);
          attroff(COLOR_PAIR(COLOR_GREEN));
          break;
        case ter_path:
        case ter_bailey:
          attron(COLOR_PAIR(COLOR_YELLOW));
          mvaddch(y + 1, x, PATH_SYMBOL);
          attroff(COLOR_PAIR(COLOR_YELLOW));
          break;
        case ter_gate:
          attron(COLOR_PAIR(COLOR_YELLOW));
          mvaddch(y + 1, x, GATE_SYMBOL);
          attroff(COLOR_PAIR(COLOR_YELLOW));
          break;
        case ter_mart:
          attron(COLOR_PAIR(COLOR_BLUE));
          mvaddch(y + 1, x, POKEMART_SYMBOL);
          attroff(COLOR_PAIR(COLOR_BLUE));
          break;
        case ter_center:
          attron(COLOR_PAIR(COLOR_RED));
          mvaddch(y + 1, x, POKEMON_CENTER_SYMBOL);
          attroff(COLOR_PAIR(COLOR_RED));
          break;
        case ter_grass:
          attron(COLOR_PAIR(COLOR_GREEN));
          mvaddch(y + 1, x, TALL_GRASS_SYMBOL);
          attroff(COLOR_PAIR(COLOR_GREEN));
          break;
        case ter_clearing:
          attron(COLOR_PAIR(COLOR_GREEN));
          mvaddch(y + 1, x, SHORT_GRASS_SYMBOL);
          attroff(COLOR_PAIR(COLOR_GREEN));
          break;
        case ter_water:
          attron(COLOR_PAIR(COLOR_CYAN));
          mvaddch(y + 1, x, WATER_SYMBOL);
          attroff(COLOR_PAIR(COLOR_CYAN));
          break;
        default:
          /* Use zero as an error symbol, since it stands out somewhat, and it's *
           * not otherwise used.                                                 */
          attron(COLOR_PAIR(COLOR_CYAN));
          mvaddch(y + 1, x, ERROR_SYMBOL);
          attroff(COLOR_PAIR(COLOR_CYAN));
        }
      }
    }
  }

  mvprintw(23, 1, "PC position is (%2d,%2d) on map %d%cx%d%c.",
           world.pc.pos[dim_x],
           world.pc.pos[dim_y],
           abs(world.cur_idx[dim_x] - (WORLD_SIZE / 2)),
           world.cur_idx[dim_x] - (WORLD_SIZE / 2) >= 0 ? 'E' : 'W',
           abs(world.cur_idx[dim_y] - (WORLD_SIZE / 2)),
           world.cur_idx[dim_y] - (WORLD_SIZE / 2) <= 0 ? 'N' : 'S');
  mvprintw(22, 1, "%d known %s.", world.cur_map->num_trainers,
           world.cur_map->num_trainers > 1 ? "trainers" : "trainer");
  mvprintw(22, 30, "Nearest visible trainer: ");
  if ((c = io_nearest_visible_trainer()))
  {
    attron(COLOR_PAIR(COLOR_RED));
    mvprintw(22, 55, "%c at vector %d%cx%d%c.",
             c->symbol,
             abs(c->pos[dim_y] - world.pc.pos[dim_y]),
             ((c->pos[dim_y] - world.pc.pos[dim_y]) <= 0 ? 'N' : 'S'),
             abs(c->pos[dim_x] - world.pc.pos[dim_x]),
             ((c->pos[dim_x] - world.pc.pos[dim_x]) <= 0 ? 'W' : 'E'));
    attroff(COLOR_PAIR(COLOR_RED));
  }
  else
  {
    attron(COLOR_PAIR(COLOR_BLUE));
    mvprintw(22, 55, "NONE.");
    attroff(COLOR_PAIR(COLOR_BLUE));
  }

  io_print_message_queue(0, 0);

  refresh();
}

uint32_t io_teleport_pc(pair_t dest)
{
  /* Just for fun. And debugging.  Mostly debugging. */

  do
  {
    dest[dim_x] = rand_range(1, MAP_X - 2);
    dest[dim_y] = rand_range(1, MAP_Y - 2);
  } while (world.cur_map->cmap[dest[dim_y]][dest[dim_x]] ||
           move_cost[char_pc][world.cur_map->map[dest[dim_y]]
                                                [dest[dim_x]]] ==
               DIJKSTRA_PATH_MAX ||
           world.rival_dist[dest[dim_y]][dest[dim_x]] < 0);

  return 0;
}

static void io_scroll_trainer_list(char (*s)[TRAINER_LIST_FIELD_WIDTH],
                                   uint32_t count)
{
  uint32_t offset;
  uint32_t i;

  offset = 0;

  while (1)
  {
    for (i = 0; i < 13; i++)
    {
      mvprintw(i + 6, 19, " %-40s ", s[i + offset]);
    }
    switch (getch())
    {
    case KEY_UP:
      if (offset)
      {
        offset--;
      }
      break;
    case KEY_DOWN:
      if (offset < (count - 13))
      {
        offset++;
      }
      break;
    case 27:
      return;
    }
  }
}

static void io_list_trainers_display(npc **c, uint32_t count)
{
  uint32_t i;
  char(*s)[TRAINER_LIST_FIELD_WIDTH]; /* pointer to array of 40 char */

  s = (char(*)[TRAINER_LIST_FIELD_WIDTH])malloc(count * sizeof(*s));

  mvprintw(3, 19, " %-40s ", "");
  /* Borrow the first element of our array for this string: */
  snprintf(s[0], TRAINER_LIST_FIELD_WIDTH, "You know of %d trainers:", count);
  mvprintw(4, 19, " %-40s ", *s);
  mvprintw(5, 19, " %-40s ", "");

  for (i = 0; i < count; i++)
  {
    snprintf(s[i], TRAINER_LIST_FIELD_WIDTH, "%16s %c: %2d %s by %2d %s",
             char_type_name[c[i]->ctype],
             c[i]->symbol,
             abs(c[i]->pos[dim_y] - world.pc.pos[dim_y]),
             ((c[i]->pos[dim_y] - world.pc.pos[dim_y]) <= 0 ? "North" : "South"),
             abs(c[i]->pos[dim_x] - world.pc.pos[dim_x]),
             ((c[i]->pos[dim_x] - world.pc.pos[dim_x]) <= 0 ? "West" : "East"));
    if (count <= 13)
    {
      /* Handle the non-scrolling case right here. *
       * Scrolling in another function.            */
      mvprintw(i + 6, 19, " %-40s ", s[i]);
    }
  }

  if (count <= 13)
  {
    mvprintw(count + 6, 19, " %-40s ", "");
    mvprintw(count + 7, 19, " %-40s ", "Hit escape to continue.");
    while (getch() != 27 /* escape */)
      ;
  }
  else
  {
    mvprintw(19, 19, " %-40s ", "");
    mvprintw(20, 19, " %-40s ",
             "Arrows to scroll, escape to continue.");
    io_scroll_trainer_list(s, count);
  }

  free(s);
}

static void io_list_trainers()
{
  npc **c;
  uint32_t x, y, count;

  c = (npc **)malloc(world.cur_map->num_trainers * sizeof(*c));

  /* Get a linear list of trainers */
  for (count = 0, y = 1; y < MAP_Y - 1; y++)
  {
    for (x = 1; x < MAP_X - 1; x++)
    {
      if (world.cur_map->cmap[y][x] && world.cur_map->cmap[y][x] !=
                                           &world.pc)
      {
        c[count++] = dynamic_cast<npc *>(world.cur_map->cmap[y][x]);
      }
    }
  }

  /* Sort it by distance from PC */
  qsort(c, count, sizeof(*c), compare_trainer_distance);

  /* Display it */
  io_list_trainers_display(c, count);
  free(c);

  /* And redraw the map */
  io_display();
}

void io_pokemart(pc *playerCharacter)
{
  clear();
  mvprintw(0, 0, "Welcome to the Pokemart. Could I interest you in some Pokeballs and Potions?");
  mvprintw(1, 0, "Press ENTER to replenish your Pokeballs and Potions.");
  refresh();

  int ch = getch(); // Wait for user input
  while (ch != '\n')
  { // Keep waiting until the user presses ENTER
    ch = getch();
  }

  // Once ENTER is pressed, replenish Pokeballs and Potions
  playerCharacter->replenishPokeBalls();
  playerCharacter->replenishPotions();

  // Confirm that the Pokeballs and Potions have been replenished
  mvprintw(2, 0, "Your Pokeballs and Potions have been replenished!");
  mvprintw(3, 0, "Press any key to continue...");
  refresh();
  getch(); // Wait for any key to continue

  clear();
}

void io_pokemon_center(pc *playerCharacter)
{

  clear();
  mvprintw(0, 0, "Welcome to the Pokemon Center. How can Nurse Joy assist you?");
  mvprintw(1, 0, "Press ENTER to heal all your Pokemon.");
  refresh();

  int ch = getch(); // Wait for user input
  while (ch != '\n')
  { // Keep waiting until the user presses ENTER
    ch = getch();
  }

  // Once ENTER is pressed, heal all Pokemon
  playerCharacter->healAllPokemon();

  // Confirm that the Pokemon have been healed
  mvprintw(2, 0, "All your Pokemon have been healed!");
  mvprintw(3, 0, "Press any key to continue...");
  refresh();
  getch(); // Wait for any key to continue

  clear();
}

/*
void io_battle(character *aggressor, character *defender)
{
  npc *n = (npc *) ((aggressor == &world.pc) ? defender : aggressor);
  std::vector<PokeData>::size_type currentPokemonIndex = 0; // Start with the first Pokemon in the list

  // Ensure the NPC has at least one Pokemon before attempting to display it
  if (!n->getPokemons().empty()) {
    while (true) { // Loop to allow scrolling through the Pokemon list
      clear(); // Clear the screen before displaying new content
      io_display(); // Display the world or battle screen

      // Display the message at the top
      mvprintw(0, 0, "You have engaged in battle *scroll down to toggle trainers pokemon*");

      // Display the current Pokemon
      const PokeData& currentPokemon = n->getPokemons().at(currentPokemonIndex);
      io_display_pokemon(currentPokemon);

      // Display a message to hit escape to continue or scroll keys to toggle
      mvprintw(displayHeight+1, 19, "Use UP/DOWN to toggle. Hit escape to continue.");

      refresh(); // Refresh the screen to show changes

      // Wait for the user to press a key
      int ch = getch();
      if (ch == KEY_UP) {
        // Scroll up to the previous Pokemon, if not at the beginning
        if (currentPokemonIndex > 0) {
          currentPokemonIndex--;
        }
      } else if (ch == KEY_DOWN) {
        // Scroll down to the next Pokemon, if not at the end
        if (currentPokemonIndex < n->getPokemons().size() - 1) {
          currentPokemonIndex++;
        }
      } else if (ch == 27) {
        // Exit the loop if the escape key is pressed
        break;
      }
    }
  } else {
    // Fallback message if the NPC has no Pokemon
    io_display();
    mvprintw(0, 0, "This character has no Pokemon.");
    refresh();
    getch();
  }

  n->defeated = 1;
  if (n->ctype == char_hiker || n->ctype == char_rival) {
    n->mtype = move_wander;
  }
}
*/
void io_display_battle_options()
{
  clear();
  io_display(); // Assuming this function sets up the battle screen
  mvprintw(2, 0, "1.) Fight");
  mvprintw(3, 0, "2.) Flee");
  mvprintw(4, 0, "3.) Pokemon (choose active pokemon)");
  mvprintw(6, 0, "Choose an option: ");
  refresh();
}

void io_battle(character *aggressor, character *defender)
{
  int choice = 3;
  // Lambda to find the HP stat
  auto findHP = [](const std::vector<PokeStats> &stats) -> const PokeStats *
  {
    for (const auto &stat : stats)
    {
      if (stat.getStatIdentifier() == "hp")
      {
        return &stat;
      }
    }
    return nullptr; // HP stat not found
  };

  auto allPokemonsDefeated = [findHP](const std::vector<PokeData> &pokemons) -> bool
  {
    for (const auto &pokemon : pokemons)
    {
      const PokeStats *hpStat = findHP(pokemon.getStatsList());
      if (hpStat && hpStat->getStatRating() > 0)
      {
        return false; // At least one Pokémon is still standing
      }
    }
    return true; // All Pokémon are defeated
  };

  auto selectNextAvailablePokemon = [findHP](std::vector<PokeData> &pokemons) -> PokeData *
  {
    for (auto &pokemon : pokemons)
    {
      const PokeStats *hpStat = findHP(pokemon.getStatsList());
      if (hpStat && hpStat->getStatRating() > 0)
      {
        return &pokemon;
      }
    }
    return nullptr; // No valid Pokémon found
  };

  // Directly identify the PC and NPC based on the provided parameters.
  pc *playerCharacter = nullptr;
  npc *nonPlayerCharacter = nullptr;

  // Check if the aggressor is the PC
  if (aggressor == &world.pc)
  {
    playerCharacter = (pc *)aggressor;
    nonPlayerCharacter = (npc *)defender;
  }
  else if (defender == &world.pc)
  { // Or if the defender is the PC
    playerCharacter = (pc *)defender;
    nonPlayerCharacter = (npc *)aggressor;
  }

  // Ensure both PC and NPC are correctly identified before proceeding
  if (playerCharacter && nonPlayerCharacter)
  {
    srand(time(0)); // Seed the random number generator

    while (true)
    {
      if (allPokemonsDefeated(playerCharacter->getPokemons()) || allPokemonsDefeated(nonPlayerCharacter->getPokemons()))
      {
        // End the battle if all Pokémon from one side are defeated
        break;
      }

      // Select the next available Pokémon for the NPC using the lambda
      PokeData *npcPokemon = selectNextAvailablePokemon(nonPlayerCharacter->getPokemons());
      if (!npcPokemon)
      {
        // Handle the case where no valid NPC Pokémon is found
        mvprintw(0, 0, "All NPC Pokémon are defeated.");
        refresh();
        getch();
        break;
      }

      // bool fightSelected = false;
      PokeData *playerPokemon = nullptr; // Pointer to store the chosen Pokemon
      
      do {
            clear();
            io_display(); // Display the world or battle screen
            mvprintw(0, 0, "NPC has chosen: %s", npcPokemon->getPokemon().getIdentifier().c_str());
            io_display_battle_options();
            int playerChoice = getch() - '0'; // Capture player input and convert character code to integer
            
            if (playerChoice == 0) { // Check if the escape key was pressed
                return; // Exit the function immediately
            }
            choice = handle_init_battle_choice(playerChoice, playerCharacter, playerPokemon, npcPokemon, nonPlayerCharacter);
        } while (choice == 3); // Continue loop until a fight is selected or fleeing is successful

      if (choice == 0)
      {
        // Implement fight logic here using playerPokemon
        io_pokemon_battle_screen(playerCharacter, playerPokemon, nonPlayerCharacter, npcPokemon);
      }
    }
    if (choice == 0)
    {
      nonPlayerCharacter->defeated = 1;
      if (nonPlayerCharacter->ctype == char_hiker || nonPlayerCharacter->ctype == char_rival)
      {
        nonPlayerCharacter->mtype = move_wander;
      }
    }
  }
  else
  {
    // Handle the case where either PC or NPC could not be identified
    io_display();
    mvprintw(0, 0, "Error: Could not identify PC and NPC.");
    refresh();
    getch();
  }
}

void io_wild_battle(pc *playerCharacter, PokeData *wildPokemon)
{
  clear();
  io_display(); // Assuming this sets up the screen

  // Display the number of Pokéballs the player has
  mvprintw(0, 0, "You have %d Pokéballs.", playerCharacter->getPokeballCount());
  mvprintw(1, 0, "Press 'c' to throw a Pokéball and try to capture the wild Pokémon.");
  mvprintw(2, 0, "Press any other key to cancel.");
  refresh();

  int ch = getch(); // Get user input
  if (ch == 'c' || ch == 'C')
  {
    if (playerCharacter->getPokeballCount() > 0)
    {
      // Attempt to use a Pokéball to capture the wild Pokémon
      playerCharacter->usePokeball(); // Decrements the Pokéball count

      // Add the wild Pokémon to the player's collection
      playerCharacter->addPokemon(*wildPokemon);

      mvprintw(3, 0, "Wild Pokémon captured successfully!");
    }
    else
    {
      mvprintw(3, 0, "You have no Pokéballs left!");
    }
  }
  else
  {
    mvprintw(3, 0, "You decided not to throw a Pokéball.");
  }

  mvprintw(4, 0, "Press any key to continue...");
  refresh();
  getch(); // Wait for any key to continue

  clear();
}

void io_pokemon_battle_screen(pc *playerCharacter, PokeData *playerPokemon, npc *nonPlayerCharacter, PokeData *nonPlayerPokemon)
{
  bool battleOver = false;

  auto findHP = [](const std::vector<PokeStats> &stats) -> const PokeStats *
  {
    for (const auto &stat : stats)
    {
      if (stat.getStatIdentifier() == "hp")
      {
        return &stat;
      }
    }
    return nullptr; // HP stat not found
  };

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

  while (!battleOver)
  {
    const PokeStats *playerHPStat = findHP(playerPokemon->getStatsList());
    const PokeStats *npcHPStat = findHP(nonPlayerPokemon->getStatsList());
    const PokeStats *playerMaxHPStat = findMaxHP(playerPokemon->getStatsList());
    const PokeStats *npcMaxHPStat = findMaxHP(nonPlayerPokemon->getStatsList());
    if (playerHPStat->getStatRating() <= 0 || npcHPStat->getStatRating() <= 0)
    {
      battleOver = true;
      if (playerHPStat->getStatRating() <= 0)
      {
        mvprintw(9, 2, "Your Pokemon has been defeated. Press any key to continue.");
      }
      else
      {
        mvprintw(9, 2, "NPC's Pokemon has been defeated. Press any key to continue.");
      }
      refresh();
      getch(); // Wait for player input to continue
      break;   // Exit the loop
    }

    clear();
    io_display();

    // Display Player's Pokemon information on the left side
    mvprintw(3, 2, "Player's Pokemon:");
    mvprintw(4, 2, "Name: %s", playerPokemon->getPokemon().getIdentifier().c_str());
    mvprintw(5, 2, "Level: %d", playerPokemon->getLevel());
    if (playerHPStat && playerMaxHPStat)
    {
      mvprintw(6, 2, "HP: %d/%d", playerHPStat->getStatRating(), playerMaxHPStat->getStatRating());
    }
    else
    {
      mvprintw(6, 2, "HP: Unknown");
    }
    mvprintw(9, 2, "1.) Select a move");
    mvprintw(10, 2, "2.) Look at bag");
    mvprintw(11, 2, "3.) Switch pokemon");

    // Display NPC's Pokemon information on the right side
    mvprintw(3, 40, "NPC's Pokemon:");
    mvprintw(4, 40, "Name: %s", nonPlayerPokemon->getPokemon().getIdentifier().c_str());
    mvprintw(5, 40, "Level: %d", nonPlayerPokemon->getLevel());
    if (npcHPStat && npcMaxHPStat)
    {
      mvprintw(6, 40, "HP: %d/%d", npcHPStat->getStatRating(), npcMaxHPStat->getStatRating());
    }
    else
    {
      mvprintw(6, 40, "HP: Unknown");
    }

    // Optionally, display a separator line between the two Pokemon
    for (int i = 0; i < 9; i++)
    { // Adjust the loop limit based on your display needs
      mvaddch(3 + i, 35, '|');
    }

    refresh();

    // Check for player input
    int ch = getch();
    switch (ch)
    {
    case '1': // Player chooses to select a move
      io_battle_moves(playerCharacter, playerPokemon, nonPlayerCharacter, nonPlayerPokemon);
      break;
    case '2': // Player chooses to look at the bag
      io_show_bag_in_battle(playerCharacter, playerPokemon, nonPlayerCharacter, nonPlayerPokemon, 1);
      break;
    case '3': // Player chooses to switch pokemon
      // Implement switching functionality here
      playerPokemon = io_pokemon_list(playerCharacter);
      processNPCMoveSwitch(playerCharacter, playerPokemon, nonPlayerCharacter, nonPlayerPokemon);
      break;
    case 27: // ESC key, player chooses to flee or end the battle
      battleOver = true;
      break;
    default:
      // Handle invalid input
      break;
    }
  }
}

void io_battle_moves(pc *playerCharacter, PokeData *playerPokemon, npc *nonPlayerCharacter, PokeData *nonPlayerPokemon)
{
  bool moveChosen = false;
  size_t currentMoveIndex = 0; // Start with the first move in the list

  if (!playerPokemon->getMovesList().empty())
  {
    while (!moveChosen)
    {
      clear();
      io_display();

      // Display instructions at the top
      mvprintw(2, 19, "Choose a move:");
      mvprintw(3, 19, "Use UP/DOWN to toggle. Hit ENTER to choose. ESC to cancel.");

      // Loop through and display each move
      for (size_t i = 0; i < playerPokemon->getMovesList().size(); ++i)
      {
        const Moves &move = playerPokemon->getMovesList().at(i);
        // Highlight the current move
        if (i == currentMoveIndex)
        {
          attron(A_REVERSE);
        }
        mvprintw(5 + i, 19, "%s", move.getIdentifier().c_str());
        attroff(A_REVERSE);

        // Display additional move details for the currently highlighted move
        if (i == currentMoveIndex)
        {
          mvprintw(5 + playerPokemon->getMovesList().size() + 1, 19, "Power: %s", move.getPower() == 1 ? "Non-attack type (will do one damage for now)" : std::to_string(move.getPower()).c_str());
          mvprintw(5 + playerPokemon->getMovesList().size() + 2, 19, "PP: %d", move.getPp());
          mvprintw(5 + playerPokemon->getMovesList().size() + 3, 19, "Accuracy: %s", move.getAccuracy() == -1 ? "N/A" : (std::to_string(move.getAccuracy()) + "%").c_str());
        }
      }

      refresh();

      int ch = getch();
      switch (ch)
      {
      case KEY_UP:
        if (currentMoveIndex > 0)
        {
          currentMoveIndex--;
        }
        break;
      case KEY_DOWN:
        if (currentMoveIndex < playerPokemon->getMovesList().size() - 1)
        {
          currentMoveIndex++;
        }
        break;
      case '\n': // ENTER key
      {
        const Moves &chosenMove = playerPokemon->getMovesList().at(currentMoveIndex);
        processMove(playerCharacter, playerPokemon, nonPlayerCharacter, nonPlayerPokemon, &chosenMove);
        moveChosen = true;
      }
      break;
      case 27: // ESC key
        moveChosen = true;
        break;
      }
    }
  }
  else
  {
    io_display();
    mvprintw(0, 0, "This Pokemon has no moves.");
    refresh();
    getch();
  }
}

void io_show_bag_in_battle(pc *playerCharacter, PokeData *playerPokemon, npc *nonPlayerCharacter, PokeData *nonPlayerPokemon, int scenario)
{
  clear();
    io_display(); // Assuming this sets up the screen

    mvprintw(0, 0, "Your Bag:");
    int line = 1; // Start printing from the next line

    int optionNumber = 1;                 // Start option numbering
    std::vector<Potion *> indexedPotions; // To keep track of potions for selection

    // Show the number of Pokéballs (non-usable in NPC battles)
    mvprintw(line++, 0, "%d) Pokeballs: %d (Not usable in NPC battles)", optionNumber++, playerCharacter->getPokeballCount());

    // Show the number of revives
    mvprintw(line++, 0, "%d) Revives: %d", optionNumber++, playerCharacter->getReviveCount());

    // Show all potions
    int firstPotionIndex = optionNumber; // Save the starting index for potions
    for (auto potion : playerCharacter->potions) {
        indexedPotions.push_back(potion);
        mvprintw(line++, 0, "%d) %s Potion", optionNumber++, potion->getSize().c_str());
    }

    mvprintw(line++, 0, "Select an item to use or press any other key to cancel:");
    refresh();

    int ch = getch() - '0'; // Get user input and convert to integer
    if (ch == 1) {
        // Pokéballs selected (not usable in NPC battles)
        mvprintw(line++, 0, "Pokéballs cannot be used in NPC battles.");
    } else if (ch == 2) {
        // Revives selected
        processBagInGame(playerCharacter, playerPokemon, nonPlayerCharacter, nonPlayerPokemon, nullptr, 0, 1);
    } else if (ch >= firstPotionIndex && ch < firstPotionIndex + static_cast<int>(indexedPotions.size())) {
        // Potion selected
        Potion* selectedPotion = indexedPotions[ch - firstPotionIndex]; // Adjust index based on options shown
        processBagInGame(playerCharacter, playerPokemon, nonPlayerCharacter, nonPlayerPokemon, selectedPotion, 0, 0);
    } else {
        // Handle cancellation or invalid input
        mvprintw(line++, 0, "No action taken.");
    }

    mvprintw(line++, 0, "Press any key to continue...");
    refresh();
    getch(); // Wait for any key to continue

    clear();
}
void io_display_move_results(const Moves *pcMove, const Moves *npcMove, int npcDam, int pcDam, bool pcCrit, bool npcCrit, bool npcHit, bool pcHit)
{
  clear();
  io_display();

  // Display the header for the player's move results
  mvprintw(3, 2, "Your Move:");
  mvprintw(4, 2, "%s", pcMove->getIdentifier().c_str());

  // Display the player's move outcome
  if (pcHit)
  {
    mvprintw(5, 2, "Hit%s, Damage: %d", pcCrit ? " (Critical)" : "", pcDam);
  }
  else
  {
    mvprintw(5, 2, "Missed");
  }

  // Display the header for the NPC's move results
  mvprintw(3, 40, "Opponent's Move:");
  mvprintw(4, 40, "%s", npcMove->getIdentifier().c_str());

  // Display the NPC's move outcome
  if (npcHit)
  {
    mvprintw(5, 40, "Hit%s, Damage: %d", npcCrit ? " (Critical)" : "", npcDam);
  }
  else
  {
    mvprintw(5, 40, "Missed");
  }

  // Display a message to continue at the bottom
  mvprintw(8, 2, "Press ENTER to continue...");

  refresh();

  // Wait for the player to press ENTER
  int ch;
  do
  {
    ch = getch();
  } while (ch != '\n'); // Loop until ENTER is pressed
}

void io_display_switch_results(const Moves *npcMove, int npcDam, bool npcCrit, bool npcHit){
  clear();
  io_display();

  // Display the header for the potion results
  mvprintw(3, 2, "You have switched Pokemon!:");

  // Display the header for the NPC's move results
  mvprintw(3, 40, "Opponent's Move:");
  mvprintw(4, 40, "%s", npcMove->getIdentifier().c_str());

  // Display the NPC's move outcome
  if (npcHit)
  {
    mvprintw(5, 40, "Hit%s, Damage: %d", npcCrit ? " (Critical)" : "", npcDam);
  }
  else
  {
    mvprintw(5, 40, "Missed");
  }

  // Display a message to continue at the bottom
  mvprintw(8, 2, "Press ENTER to continue...");

  refresh();

  // Wait for the player to press ENTER
  int ch;
  do
  {
    ch = getch();
  } while (ch != '\n'); // Loop until ENTER is pressed
}

void io_display_potion_results(int healthRegen, const Moves *npcMove, int npcDam, bool npcCrit, bool npcHit)
{
  clear();
  io_display();

  // Display the header for the potion results
  mvprintw(3, 2, "Potion Used:");

  // Display the amount of healing done
  mvprintw(4, 2, "Healing: %d", healthRegen);

  // Display the header for the NPC's move results
  mvprintw(3, 40, "Opponent's Move:");
  mvprintw(4, 40, "%s", npcMove->getIdentifier().c_str());

  // Display the NPC's move outcome
  if (npcHit)
  {
    mvprintw(5, 40, "Hit%s, Damage: %d", npcCrit ? " (Critical)" : "", npcDam);
  }
  else
  {
    mvprintw(5, 40, "Missed");
  }

  // Display a message to continue at the bottom
  mvprintw(8, 2, "Press ENTER to continue...");

  refresh();

  // Wait for the player to press ENTER
  int ch;
  do
  {
    ch = getch();
  } while (ch != '\n'); // Loop until ENTER is pressed
}

void io_display_revive_results(const Moves *npcMove, int npcDam, bool npcCrit, bool npcHit)
{
  clear();
  io_display();

  // Display the header for the potion results
  mvprintw(3, 2, "You have revived a Pokemon!:");

  // Display the header for the NPC's move results
  mvprintw(3, 40, "Opponent's Move:");
  mvprintw(4, 40, "%s", npcMove->getIdentifier().c_str());

  // Display the NPC's move outcome
  if (npcHit)
  {
    mvprintw(5, 40, "Hit%s, Damage: %d", npcCrit ? " (Critical)" : "", npcDam);
  }
  else
  {
    mvprintw(5, 40, "Missed");
  }

  // Display a message to continue at the bottom
  mvprintw(8, 2, "Press ENTER to continue...");

  refresh();

  // Wait for the player to press ENTER
  int ch;
  do
  {
    ch = getch();
  } while (ch != '\n'); // Loop until ENTER is pressed
}

PokeData *io_pokemon_list(character *aggressor)
{
  npc *n = (npc *)(aggressor);
  if (!n || n->getPokemons().empty())
  {
    io_display();
    mvprintw(0, 0, "This character has no Pokemon or is not an NPC.");
    refresh();
    getch();
    return nullptr;
  }

  // Lambda to find the HP stat
  auto findHP = [](const std::vector<PokeStats> &stats) -> const PokeStats *
  {
    for (const auto &stat : stats)
    {
      if (stat.getStatIdentifier() == "hp")
      {
        return &stat;
      }
    }
    return nullptr; // HP stat not found
  };

  // Find the first Pokemon with HP > 0
  std::vector<PokeData>::size_type currentPokemonIndex = 0;
  bool foundActivePokemon = false;
  for (; currentPokemonIndex < n->getPokemons().size(); ++currentPokemonIndex)
  {
    const PokeStats *hpStat = findHP(n->getPokemons()[currentPokemonIndex].getStatsList());
    if (hpStat && hpStat->getStatRating() > 0)
    {
      foundActivePokemon = true;
      break; // Found a Pokemon with HP > 0, break out of the loop
    }
  }

  if (!foundActivePokemon)
  {
    io_display();
    mvprintw(0, 0, "All Pokemon have fainted.");
    refresh();
    getch();
    return nullptr;
  }

  // Display and selection loop
  while (true)
  {
    clear();
    io_display();

    // Display the current Pokemon
    const PokeData &currentPokemon = n->getPokemons()[currentPokemonIndex];
    io_display_pokemon(currentPokemon);

    mvprintw(displayHeight + 1, 19, "Use UP/DOWN to toggle. Hit enter to choose.");
    refresh();

    int ch = getch();
    switch (ch)
    {
    case KEY_UP:
      if (currentPokemonIndex > 0)
      {
        currentPokemonIndex--;
        const PokeStats *hpStat = findHP(n->getPokemons()[currentPokemonIndex].getStatsList());
        if (!(hpStat && hpStat->getStatRating() > 0))
        {
          currentPokemonIndex++; // Skip fainted Pokemon
        }
      }
      break;
    case KEY_DOWN:
      if (currentPokemonIndex < n->getPokemons().size() - 1)
      {
        currentPokemonIndex++;
        const PokeStats *hpStat = findHP(n->getPokemons()[currentPokemonIndex].getStatsList());
        if (!(hpStat && hpStat->getStatRating() > 0))
        {
          currentPokemonIndex--; // Skip fainted Pokemon
        }
      }
      break;
    case '\n':
      const PokeStats *hpStat = findHP(n->getPokemons()[currentPokemonIndex].getStatsList());
      if (hpStat && hpStat->getStatRating() > 0)
      {
        return &(n->getPokemons()[currentPokemonIndex]);
      }
      // If the selected Pokemon is fainted, do not return and allow another selection
      break;
    }
  }
}

PokeData *io_pokemon_fainted_list(character *aggressor)
{
  npc *n = (npc *)(aggressor);
  if (!n || n->getPokemons().empty())
  {
    io_display();
    mvprintw(0, 0, "This character has no Pokemon or is not an NPC.");
    refresh();
    getch();
    return nullptr;
  }

  // Lambda to find the HP stat
  auto findHP = [](const std::vector<PokeStats> &stats) -> const PokeStats *
  {
    for (const auto &stat : stats)
    {
      if (stat.getStatIdentifier() == "hp")
      {
        return &stat;
      }
    }
    return nullptr; // HP stat not found
  };

  std::vector<PokeData>::size_type currentPokemonIndex = 0;
  bool foundFaintedPokemon = false;

  // Find the first fainted Pokemon
  for (; currentPokemonIndex < n->getPokemons().size(); ++currentPokemonIndex)
  {
    const PokeStats *hpStat = findHP(n->getPokemons()[currentPokemonIndex].getStatsList());
    if (hpStat && hpStat->getStatRating() == 0)
    {
      foundFaintedPokemon = true;
      break; // Found a fainted Pokemon, break out of the loop
    }
  }

  if (!foundFaintedPokemon)
  {
    io_display();
    mvprintw(0, 0, "No fainted Pokemon found.");
    refresh();
    getch();
    return nullptr;
  }

  // Display and selection loop for fainted Pokemon
  while (true)
  {
    clear();
    io_display();

    // Display the current fainted Pokemon
    const PokeData &currentPokemon = n->getPokemons()[currentPokemonIndex];
    io_display_pokemon(currentPokemon);

    mvprintw(displayHeight + 1, 19, "Use UP/DOWN to toggle. Hit enter to choose.");
    refresh();

    int ch = getch();
    switch (ch)
    {
    case KEY_UP:
      if (currentPokemonIndex > 0)
      {
        currentPokemonIndex--;
        const PokeStats *hpStat = findHP(n->getPokemons()[currentPokemonIndex].getStatsList());
        if (!(hpStat && hpStat->getStatRating() == 0))
        {
          currentPokemonIndex++; // Skip non-fainted Pokemon
        }
      }
      break;
    case KEY_DOWN:
      if (currentPokemonIndex < n->getPokemons().size() - 1)
      {
        currentPokemonIndex++;
        const PokeStats *hpStat = findHP(n->getPokemons()[currentPokemonIndex].getStatsList());
        if (!(hpStat && hpStat->getStatRating() == 0))
        {
          currentPokemonIndex--; // Skip non-fainted Pokemon
        }
      }
      break;
    case '\n':
      const PokeStats *hpStat = findHP(n->getPokemons()[currentPokemonIndex].getStatsList());
      if (hpStat && hpStat->getStatRating() == 0)
      {
        return &(n->getPokemons()[currentPokemonIndex]);
      }
      // If the selected Pokemon is not fainted, do not return and allow another selection
      break;
    }
  }
}

uint32_t move_pc_dir(uint32_t input, pair_t dest)
{
  dest[dim_y] = world.pc.pos[dim_y];
  dest[dim_x] = world.pc.pos[dim_x];

  switch (input)
  {
  case 1:
  case 2:
  case 3:
    dest[dim_y]++;
    break;
  case 4:
  case 5:
  case 6:
    break;
  case 7:
  case 8:
  case 9:
    dest[dim_y]--;
    break;
  }
  switch (input)
  {
  case 1:
  case 4:
  case 7:
    dest[dim_x]--;
    break;
  case 2:
  case 5:
  case 8:
    break;
  case 3:
  case 6:
  case 9:
    dest[dim_x]++;
    break;
  case '>':
    if (world.cur_map->map[world.pc.pos[dim_y]][world.pc.pos[dim_x]] ==
        ter_mart)
    {
      io_pokemart(&world.pc);
    }
    if (world.cur_map->map[world.pc.pos[dim_y]][world.pc.pos[dim_x]] ==
        ter_center)
    {
      io_pokemon_center(&world.pc);
    }
    break;
  }

  if (world.cur_map->cmap[dest[dim_y]][dest[dim_x]])
  {
    if (dynamic_cast<npc *>(world.cur_map->cmap[dest[dim_y]][dest[dim_x]]) &&
        ((npc *)world.cur_map->cmap[dest[dim_y]][dest[dim_x]])->defeated)
    {
      // Some kind of greeting here would be nice
      return 1;
    }
    else if ((dynamic_cast<npc *>(world.cur_map->cmap[dest[dim_y]][dest[dim_x]])))
    {
      io_battle(&world.pc, world.cur_map->cmap[dest[dim_y]][dest[dim_x]]);
      // Not actually moving, so set dest back to PC position
      dest[dim_x] = world.pc.pos[dim_x];
      dest[dim_y] = world.pc.pos[dim_y];
    }
  }

  if (move_cost[char_pc][world.cur_map->map[dest[dim_y]][dest[dim_x]]] ==
      DIJKSTRA_PATH_MAX)
  {
    return 1;
  }

  return 0;
}

void io_teleport_world(pair_t dest)
{
  /* mvscanw documentation is unclear about return values.  I believe *
   * that the return value works the same way as scanf, but instead   *
   * of counting on that, we'll initialize x and y to out of bounds   *
   * values and accept their updates only if in range.                */
  int x = INT_MAX, y = INT_MAX;

  world.cur_map->cmap[world.pc.pos[dim_y]][world.pc.pos[dim_x]] = NULL;

  echo();
  curs_set(1);
  do
  {
    mvprintw(0, 0, "Enter x [-200, 200]:           ");
    refresh();
    mvscanw(0, 21, "%d", &x);
  } while (x < -200 || x > 200);
  do
  {
    mvprintw(0, 0, "Enter y [-200, 200]:          ");
    refresh();
    mvscanw(0, 21, "%d", &y);
  } while (y < -200 || y > 200);

  refresh();
  noecho();
  curs_set(0);

  x += 200;
  y += 200;

  world.cur_idx[dim_x] = x;
  world.cur_idx[dim_y] = y;

  new_map(1);
  io_teleport_pc(dest);
}

void io_handle_input(pair_t dest)
{
  uint32_t turn_not_consumed;
  int key;

  do
  {
    switch (key = getch())
    {
    case '7':
    case 'y':
    case KEY_HOME:
      turn_not_consumed = move_pc_dir(7, dest);
      break;
    case '8':
    case 'k':
    case KEY_UP:
      turn_not_consumed = move_pc_dir(8, dest);
      break;
    case '9':
    case 'u':
    case KEY_PPAGE:
      turn_not_consumed = move_pc_dir(9, dest);
      break;
    case '6':
    case 'l':
    case KEY_RIGHT:
      turn_not_consumed = move_pc_dir(6, dest);
      break;
    case '3':
    case 'n':
    case KEY_NPAGE:
      turn_not_consumed = move_pc_dir(3, dest);
      break;
    case '2':
    case 'j':
    case KEY_DOWN:
      turn_not_consumed = move_pc_dir(2, dest);
      break;
    case '1':
    case 'b':
    case KEY_END:
      turn_not_consumed = move_pc_dir(1, dest);
      break;
    case '4':
    case 'h':
    case KEY_LEFT:
      turn_not_consumed = move_pc_dir(4, dest);
      break;
    case '5':
    case ' ':
    case '.':
    case KEY_B2:
      dest[dim_y] = world.pc.pos[dim_y];
      dest[dim_x] = world.pc.pos[dim_x];
      turn_not_consumed = 0;
      break;
    case '>':
      turn_not_consumed = move_pc_dir('>', dest);
      break;
    case 'Q':
      dest[dim_y] = world.pc.pos[dim_y];
      dest[dim_x] = world.pc.pos[dim_x];
      world.quit = 1;
      turn_not_consumed = 0;
      break;
      break;
    case 't':
      io_list_trainers();
      turn_not_consumed = 1;
      break;
    case 'p':
      /* Teleport the PC to a random place in the map.              */
      io_teleport_pc(dest);
      turn_not_consumed = 0;
      break;
    case 'f':
      /* Fly to any map in the world.                                */
      io_teleport_world(dest);
      turn_not_consumed = 0;
      break;
    case 'q':
      /* Demonstrate use of the message queue.  You can use this for *
       * printf()-style debugging (though gdb is probably a better   *
       * option.  Not that it matters, but using this command will   *
       * waste a turn.  Set turn_not_consumed to 1 and you should be *
       * able to figure out why I did it that way.                   */
      io_queue_message("This is the first message.");
      io_queue_message("Since there are multiple messages, "
                       "you will see \"more\" prompts.");
      io_queue_message("You can use any key to advance through messages.");
      io_queue_message("Normal gameplay will not resume until the queue "
                       "is empty.");
      io_queue_message("Long lines will be truncated, not wrapped.");
      io_queue_message("io_queue_message() is variadic and handles "
                       "all printf() conversion specifiers.");
      io_queue_message("Did you see %s?", "what I did there");
      io_queue_message("When the last message is displayed, there will "
                       "be no \"more\" prompt.");
      io_queue_message("Have fun!  And happy printing!");
      io_queue_message("Oh!  And use 'Q' to quit!");

      dest[dim_y] = world.pc.pos[dim_y];
      dest[dim_x] = world.pc.pos[dim_x];
      turn_not_consumed = 0;
      break;
    default:
      /* Also not in the spec.  It's not always easy to figure out what *
       * key code corresponds with a given keystroke.  Print out any    *
       * unhandled key here.  Not only does it give a visual error      *
       * indicator, but it also gives an integer value that can be used *
       * for that key in this (or other) switch statements.  Printed in *
       * octal, with the leading zero, because ncurses.h lists codes in *
       * octal, thus allowing us to do reverse lookups.  If a key has a *
       * name defined in the header, you can use the name here, else    *
       * you can directly use the octal value.                          */
      mvprintw(0, 0, "Unbound key: %#o ", key);
      turn_not_consumed = 1;
    }
    refresh();
  } while (turn_not_consumed);
}

void io_display_found_pokemon(pc *playerCharacter, PokeData &data)
{

  clear();
  io_display();

  // Initialize maxWidth with the width of static text
  int maxWidth = static_cast<int>(strlen("Name: ") + data.getPokemon().getIdentifier().length());
  maxWidth = std::max(maxWidth, static_cast<int>(strlen("Level: ") + std::to_string(data.getLevel()).length()));

  mvprintw(2, 19, "You have found a pokemon!!");
  // Display Pokemon information
  mvprintw(3, 19, "Level: %d", data.getLevel());
  mvprintw(4, 19, "Name: %s", data.getPokemon().getIdentifier().c_str());

  // Display if the Pokemon is shiny
  const char *shinyStatus = data.getShiny() ? "Yes" : "No";
  mvprintw(5, 19, "Shiny: %s", shinyStatus);

  int maxHeight = 6; // Adjust based on the number of lines you will print, increased for shiny status

  // Check if there are moves to print
  if (data.getMovesList().empty())
  {
    mvprintw(7, 19, "No moves learned.");
    maxHeight += 1;
  }
  else
  {
    mvprintw(7, 19, "Moves:");
    int line = 8; // Adjusted line number for moves
    for (const auto &move : data.getMovesList())
    {
      maxWidth = std::max(maxWidth, static_cast<int>(19 + strlen(" - ") + move.getIdentifier().length()));
      mvprintw(line++, 19, " - %s", move.getIdentifier().c_str());
    }
    maxHeight += data.getMovesList().size() + 1;
  }

  // Check if there are stats to print
  if (!data.getStatsList().empty())
  {
    mvprintw(maxHeight + 3, 19, "Stats:");
    int line = maxHeight + 4; // Adjusted line number for stats
    for (const auto &stat : data.getStatsList())
    {
      maxWidth = std::max(maxWidth, static_cast<int>(19 + strlen(" - ") + stat.getStatIdentifier().length() + std::to_string(stat.getStatRating()).length()));
      mvprintw(line++, 19, " - %s: %d, IV: %d", stat.getStatIdentifier().c_str(), stat.getStatRating(), stat.getIV());
    }
    maxHeight += data.getStatsList().size() + 1;
  }

  // Adjust maxWidth for the border
  maxWidth += 20; // Adjust based on the starting position and desired padding

  // Draw the border
  int displayHeight = maxHeight + 3; // Adjusted display height for the border
  for (int i = 0; i <= displayHeight; i++)
  {
    mvaddch(1 + i, 18, '|');            // Left border
    mvaddch(1 + i, 18 + maxWidth, '|'); // Right border
  }
  for (int i = 0; i <= maxWidth; i++)
  {
    mvaddch(1, 18 + i, '-');                 // Top border
    mvaddch(2 + displayHeight, 18 + i, '-'); // Bottom border
  }

  // Add corners
  mvaddch(1, 18, '+');
  mvaddch(1, 18 + maxWidth, '+');
  mvaddch(2 + displayHeight, 18, '+');
  mvaddch(2 + displayHeight, 18 + maxWidth, '+');

  // Display a message to hit escape to continue
  mvprintw(displayHeight + 1, 19, "Hit Enter to engage in battle or escape to continue.");

  refresh();

  // Wait for the user to press Enter or escape
  int ch;
  while ((ch = getch()) != 27 /* escape */)
  {
    if (ch == '\n' /* Enter */)
    {
      // Call the io_wild_battle function to engage in battle with the wild Pokémon
      io_wild_battle(playerCharacter, &data);
      break; // Break out of the loop after the battle
    }
  }

  clear();
}

void io_display_pokemon(const PokeData &data)
{

  clear();
  io_display();

  // Initialize maxWidth with the width of static text
  int maxWidth = static_cast<int>(strlen("Name: ") + data.getPokemon().getIdentifier().length());
  maxWidth = std::max(maxWidth, static_cast<int>(strlen("Level: ") + std::to_string(data.getLevel()).length()));

  // Display Pokemon information
  mvprintw(3, 19, "Level: %d", data.getLevel());
  mvprintw(4, 19, "Name: %s", data.getPokemon().getIdentifier().c_str());

  // Display if the Pokemon is shiny
  const char *shinyStatus = data.getShiny() ? "Yes" : "No";
  mvprintw(5, 19, "Shiny: %s", shinyStatus);

  int maxHeight = 6; // Adjust based on the number of lines you will print, increased for shiny status

  // Check if there are moves to print
  if (data.getMovesList().empty())
  {
    mvprintw(7, 19, "No moves learned.");
    maxHeight += 1;
  }
  else
  {
    mvprintw(7, 19, "Moves:");
    int line = 8; // Adjusted line number for moves
    for (const auto &move : data.getMovesList())
    {
      maxWidth = std::max(maxWidth, static_cast<int>(19 + strlen(" - ") + move.getIdentifier().length()));
      mvprintw(line++, 19, " - %s", move.getIdentifier().c_str());
    }
    maxHeight += data.getMovesList().size() + 1;
  }

  // Check if there are stats to print
  if (!data.getStatsList().empty())
  {
    mvprintw(maxHeight + 3, 19, "Stats:"); // Adjusted position for stats
    int line = maxHeight + 4;              // Adjusted line number for stats
    for (const auto &stat : data.getStatsList())
    {
      maxWidth = std::max(maxWidth, static_cast<int>(19 + strlen(" - ") + stat.getStatIdentifier().length() + std::to_string(stat.getStatRating()).length()));
      mvprintw(line++, 19, " - %s: %d", stat.getStatIdentifier().c_str(), stat.getStatRating());
    }
    maxHeight += data.getStatsList().size() + 1;
  }

  // Adjust maxWidth for the border
  maxWidth += 20; // Adjust based on the starting position and desired padding

  // Draw the border
  int displayHeight = maxHeight + 1; // Adjusted display height for the border
  for (int i = 0; i <= displayHeight; i++)
  {
    mvaddch(2 + i, 18, '|');            // Left border
    mvaddch(2 + i, 18 + maxWidth, '|'); // Right border
  }
  for (int i = 0; i <= maxWidth; i++)
  {
    mvaddch(2, 18 + i, '-');                 // Top border
    mvaddch(2 + displayHeight, 18 + i, '-'); // Bottom border
  }

  // Add corners
  mvaddch(2, 18, '+');
  mvaddch(2, 18 + maxWidth, '+');
  mvaddch(2 + displayHeight, 18, '+');
  mvaddch(2 + displayHeight, 18 + maxWidth, '+');

  refresh();
}

void io_display_choose_starter_pokemon(const std::vector<PokeData> &startingPokemons, character &c)
{
  size_t currentPokemonIndex = 0; // Start with the first Pokemon in the list

  while (true)
  { // Loop to allow scrolling through the Pokemon list
    clear();
    // Display the welcome message at the top
    mvprintw(0, 0, "Welcome to the Pokemon world!!");

    // Display the message to choose a starter Pokemon
    mvprintw(1, 0, "Choose your starter Pokemon:");

    // Display the current Pokemon
    const PokeData &pokemon = startingPokemons[currentPokemonIndex];
    // Print the Pokemon's name, level, and shiny status
    const char *shinyStatus = pokemon.getShiny() ? "Yes" : "No";
    mvprintw(3, 0, "%zu - %s, Level: %d, Shiny: %s", currentPokemonIndex + 1, pokemon.getPokemon().getIdentifier().c_str(), pokemon.getLevel(), shinyStatus);

    // Print the Pokemon's moves
    mvprintw(4, 0, "Moves:");
    int line = 5;
    for (const auto &move : pokemon.getMovesList())
    {
      mvprintw(line++, 0, " - %s", move.getIdentifier().c_str());
    }

    // Print the Pokemon's stats and IVs
    mvprintw(line++, 0, "Stats:");
    for (const auto &stat : pokemon.getStatsList())
    {
      mvprintw(line++, 0, " - %s: %d, IV: %d", stat.getStatIdentifier().c_str(), stat.getStatRating(), stat.getIV());
    }

    // Display instructions for choosing a Pokemon
    mvprintw(line + 2, 0, "Use UP/DOWN to scroll. Press ENTER to choose this Pokemon.");

    refresh(); // Refresh the screen to show changes

    // Wait for the user to press a key
    int ch = getch();
    switch (ch)
    {
    case KEY_UP:
      if (currentPokemonIndex > 0)
      {
        currentPokemonIndex--;
      }
      break;
    case KEY_DOWN:
      if (currentPokemonIndex < startingPokemons.size() - 1)
      {
        currentPokemonIndex++;
      }
      break;
    case '\n': // ENTER key
      const PokeData &chosenPokemon = startingPokemons[currentPokemonIndex];
      io_display_pokemon(chosenPokemon);
      c.addPokemon(chosenPokemon); // Add the chosen Pokemon to the character
      return;                      // Exit the function after choosing a Pokemon
    }
  }
}