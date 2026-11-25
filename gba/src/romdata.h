/* romdata.h -- the names and base stats the editor shows, read from the
 * game cartridge itself, so the editor carries no game data.
 *
 * Each table is found by what its first entries hold, on a 4-byte boundary
 * (where the games' linkers put them), not at an address typed in per
 * version:
 *   species names  11-byte entries: "??????????", then "BULBASAUR"
 *   move names     13-byte entries: "-", then "POUND"
 *   items          44-byte entries (RS, FRLG and Emerald alike), the name
 *                  first: "????????", then "MASTER BALL"
 *   base stats     28-byte entries: species 1 is 45/49/49/45/65/65,
 *                  Grass/Poison; growth rate at byte 19
 *   battle moves   12-byte entries: none, then Pound (power 40, Normal,
 *                  100%, 35 PP), Karate Chop (50, Fighting, 100%, 25 PP);
 *                  base PP at byte 4
 * English games only, for the names; a table not found shows numbers.
 */
#ifndef ROMDATA_H
#define ROMDATA_H
#include <stdint.h>

typedef struct {
    const uint8_t *rom;
    uint32_t size;
    uint32_t species_names, move_names, items, base_stats, moves;   /* offsets; 0: not found */
    uint16_t items_max;           /* this game's last item: 0 when the item table is not found */
} romdata_t;

#define RD_SPECIES_MAX  411       /* internal species 1..411 (412 is the egg) */
#define RD_MOVES_MAX    354
#define RD_ITEMS_MAX    376       /* Emerald's last item; romdata_find reads each game's own */

/* Find the tables in rom[0..size). Returns how many were found (0..5). */
int romdata_find(romdata_t *rd, const uint8_t *rom, uint32_t size);
/* Names, decoded to font codes into out (12 bytes for species, 13 for
 * moves, 15 for items). A table not found gives "#123". */
void romdata_species_name(const romdata_t *rd, uint16_t species, char *out);
void romdata_move_name(const romdata_t *rd, uint16_t move, char *out);
void romdata_item_name(const romdata_t *rd, uint16_t item, char *out);
/* A species' base stats and growth rate; 0 when the table is not found. */
#include "growth.h"
int romdata_species_stats(const romdata_t *rd, uint16_t species, species_stats_t *out);
/* The item table's length is not stored: each entry holds its own number
 * (u16 at byte 14), or 0 for an unused one, so the table ends where an
 * entry holds neither. items_max is the last entry holding its number. */
/* Items a Pokemon may be given to hold, as the games' own GIVE allows: an
 * entry in use (it holds its own number), not important (byte 24: key items,
 * HMs). Not offered either, though the game allows them:
 *   - mail (items 121..132, the games' ItemIsMail): a letter goes with it,
 *     written in the game;
 *   - the Enigma Berry (175): its effect is the e-Reader berry saved with it.
 * Item 0 (nothing) is always allowed. */
#define ITEM_MAIL_FIRST    121
#define ITEM_MAIL_LAST     132
#define ITEM_ENIGMA_BERRY  175
int romdata_item_is_mail(uint16_t item);
int romdata_item_holdable(const romdata_t *rd, uint16_t item);
/* |delta| holdable items on from cur, wrapping through 0 (nothing). */
uint16_t romdata_next_holdable(const romdata_t *rd, uint16_t cur, int delta);
/* A move's base PP; 0 when the table is not found. */
uint8_t romdata_move_pp(const romdata_t *rd, uint16_t move);
#endif
