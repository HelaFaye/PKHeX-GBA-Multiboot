/* edit.h -- the edits the editor offers, kept consistent as the game keeps
 * them: a level is experience, and a party Pokemon's level and stats follow
 * its experience, IVs, EVs and nature. */
#ifndef EDIT_H
#define EDIT_H
#include "growth.h"
#include "mon.h"

#define EV_TOTAL_MAX 510

/* Recompute a party Pokemon's level and stats, and move its HP as the game
 * does when the maximum changes. Nothing for a box Pokemon. */
void edit_refresh(mon_t *m, const species_stats_t *sp);
/* The level a Pokemon is at (a box record keeps only experience). */
int edit_level(const mon_t *m, const species_stats_t *sp);
/* Set the level: the least experience for it. */
void edit_set_level(mon_t *m, const species_stats_t *sp, int level);
/* Set an IV (0..31) or EV (0..255, total at most 510): the value is
 * clamped, and the value set is returned. */
int edit_set_iv(mon_t *m, int stat, int v);
int edit_set_ev(mon_t *m, int stat, int v);
/* The move |delta| steps on from a slot's, among those it may take: a
 * Gen 3 move (1..354), not Struggle (165, only ever used, never known), not
 * one another slot has (no Pokemon knows a move twice); 0 (an empty slot)
 * only while another slot keeps a move (a Pokemon knows at least one). */
#define MOVE_STRUGGLE 165
uint16_t edit_next_move(const mon_t *m, int slot, int delta);
/* What the game would object to in a slot's move, or 0: known twice,
 * Struggle, or no move left at all. The editor allows each, and says so. */
const char *edit_move_issue(const mon_t *m, int slot);
/* Names of Japanese Pokemon use the Japanese character set, which the editor
 * neither shows nor writes: the record's language (byte 18) is 1. */
#define LANG_JAPANESE 1
int edit_name_is_japanese(const mon_t *m);
/* Replace a move, with its base PP; move 0 clears the slot. */
void edit_set_move(mon_t *m, int slot, uint16_t move, uint8_t base_pp);
/* Close the gaps a cleared move leaves: the games keep a Pokemon's moves
 * first, with no empty slot before a used one. Each move keeps its PP and
 * PP Ups. */
void edit_compact_moves(mon_t *m);
/* Set the nickname from game characters (stopping at EOS), padded. */
void edit_set_nickname(mon_t *m, const uint8_t name[10]);
#endif
