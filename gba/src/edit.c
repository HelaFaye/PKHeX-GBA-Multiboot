/* edit.c -- see edit.h. */
#include "edit.h"

int edit_level(const mon_t *m, const species_stats_t *sp) {
    return growth_level(sp->growth, mon_exp(m));
}

void edit_refresh(mon_t *m, const species_stats_t *sp) {
    if (!m->party) return;
    uint8_t iv[6], ev[6];
    for (int s = 0; s < 6; s++) { iv[s] = mon_iv(m, s); ev[s] = mon_ev(m, s); }
    const int level = edit_level(m, sp);
    uint16_t st[6];
    growth_stats(sp, mon_species(m), level, mon_nature(m), iv, ev, st);
    const uint16_t hp = growth_new_hp(mon_species(m), mon_hp(m), mon_stat(m, STAT_HP), st[0]);
    mon_set_party_stats(m, (uint8_t)level, st, hp > st[0] ? st[0] : hp);
}

void edit_set_level(mon_t *m, const species_stats_t *sp, int level) {
    if (level < 1) level = 1;
    if (level > 100) level = 100;
    mon_set_exp(m, growth_exp(sp->growth, level));
    edit_refresh(m, sp);
}

int edit_set_iv(mon_t *m, int stat, int v) {
    if (v < 0) v = 0;
    if (v > 31) v = 31;
    mon_set_iv(m, stat, (uint8_t)v);
    return v;
}

int edit_set_ev(mon_t *m, int stat, int v) {
    if (v < 0) v = 0;
    if (v > 255) v = 255;
    const int others = mon_ev_total(m) - mon_ev(m, stat);
    if (others + v > EV_TOTAL_MAX) v = EV_TOTAL_MAX - others;
    mon_set_ev(m, stat, (uint8_t)v);
    return v;
}

void edit_set_move(mon_t *m, int slot, uint16_t move, uint8_t base_pp) {
    mon_set_move(m, slot, move, move ? base_pp : 0);
}

void edit_compact_moves(mon_t *m) {
    uint16_t mv[4];
    uint8_t pp[4], ups[4];
    int n = 0;
    const uint8_t bonuses = mon_pp_bonuses(m);
    for (int k = 0; k < 4; k++) {
        if (!mon_move(m, k)) continue;
        mv[n] = mon_move(m, k);
        pp[n] = mon_pp(m, k);
        ups[n] = (uint8_t)(bonuses >> (2 * k) & 3);
        n++;
    }
    uint8_t packed = 0;
    for (int k = 0; k < 4; k++) {
        mon_set_move(m, k, k < n ? mv[k] : 0, k < n ? pp[k] : 0);
        if (k < n) packed |= (uint8_t)(ups[k] << (2 * k));
    }
    mon_set_pp_bonuses(m, packed);
}

void edit_set_nickname(mon_t *m, const uint8_t name[10]) {
    uint8_t *n = mon_nickname(m);
    int end = 0;
    for (int k = 0; k < 10; k++) {
        if (!end && name[k] == 0xFF) end = 1;
        n[k] = end ? 0xFF : name[k];
    }
}

static int move_allowed(const mon_t *m, int slot, int move) {
    if (move == MOVE_STRUGGLE || move > 354) return 0;
    int others = 0;
    for (int k = 0; k < 4; k++) {
        if (k == slot || !mon_move(m, k)) continue;
        others++;
        if (move && mon_move(m, k) == move) return 0;
    }
    return move != 0 || others > 0;
}

uint16_t edit_next_move(const mon_t *m, int slot, int delta) {
    const int n = 355, step = delta < 0 ? -1 : 1;
    int v = mon_move(m, slot);
    for (int left = delta < 0 ? -delta : delta; left > 0; left--) {
        int tries = n;
        do { v = (v + step + n) % n; } while (!move_allowed(m, slot, v) && --tries);
    }
    return (uint16_t)v;
}

int edit_name_is_japanese(const mon_t *m) { return m->b[18] == LANG_JAPANESE; }

const char *edit_move_issue(const mon_t *m, int slot) {
    const int mv = mon_move(m, slot);
    if (mv == MOVE_STRUGGLE) return "Struggle: never a known move";
    int known = 0;
    for (int k = 0; k < 4; k++) {
        if (!mon_move(m, k)) continue;
        known++;
        if (k != slot && mv && mon_move(m, k) == mv) return "Known twice";
    }
    return known ? 0 : "No moves: battles break";
}
