#include "angband.h"

#include <assert.h>

void res_add(int which)
{
    assert(0 <= which && which < RES_MAX);
    p_ptr->resist[which]++;
}

void res_add_amt(int which, int amt)
{
    p_ptr->resist[which] += amt;
}

void res_add_all(void)
{
    int i;
    for (i = 0; i < RES_MAX; i++)
        res_add(i);
}

void res_add_immune(int which)
{
    p_ptr->resist[which] += 100;
}

void res_add_vuln(int which)
{
    p_ptr->resist[which]--;
}

bool res_is_high(int which)
{
    if (res_is_low(which))
        return FALSE;
    return TRUE;
}

bool res_is_low(int which)
{
    switch (which)
    {
    case RES_ACID:
    case RES_ELEC:
    case RES_FIRE:
    case RES_COLD:
    case RES_POIS:
        return TRUE;
    }
    return FALSE;
}


typedef struct {
    int flg;
    int res;
} _map_t;

static _map_t _resist_map[] = {
    { TR_RES_ACID, RES_ACID },
    { TR_RES_ELEC, RES_ELEC },
    { TR_RES_FIRE, RES_FIRE },
    { TR_RES_COLD, RES_COLD },
    { TR_RES_POIS, RES_POIS },
    { TR_RES_LITE, RES_LITE },
    { TR_RES_DARK, RES_DARK },
    { TR_RES_CONF, RES_CONF },
    { TR_RES_NETHER, RES_NETHER },
    { TR_RES_NEXUS, RES_NEXUS },
    { TR_RES_SOUND, RES_SOUND },
    { TR_RES_SHARDS, RES_SHARDS },
    { TR_RES_CHAOS, RES_CHAOS },
    { TR_RES_DISEN, RES_DISEN }, 
    { TR_RES_TIME, RES_TIME },
    { TR_RES_BLIND, RES_BLIND },
    { TR_RES_FEAR, RES_FEAR },
    { TR_NO_TELE, RES_TELEPORT },
    { -1, -1 }
};

static _map_t _immunity_map[] = {
    { TR_IM_ACID, RES_ACID },
    { TR_IM_ELEC, RES_ELEC },
    { TR_IM_FIRE, RES_FIRE },
    { TR_IM_COLD, RES_COLD },
    { -1, -1 }
};

static _map_t _vulnerability_map[] = {
    { TR_VULN_ACID, RES_ACID },
    { TR_VULN_ELEC, RES_ELEC },
    { TR_VULN_FIRE, RES_FIRE },
    { TR_VULN_COLD, RES_COLD },
    { TR_VULN_POIS, RES_POIS },
    { TR_VULN_FEAR, RES_FEAR },
    { TR_VULN_CONF, RES_CONF },
    { TR_VULN_SOUND, RES_SOUND },
    { TR_VULN_LITE, RES_LITE },
    { TR_VULN_DARK, RES_DARK },
    { TR_VULN_CHAOS, RES_CHAOS },
    { TR_VULN_DISEN, RES_DISEN }, 
    { TR_VULN_SHARDS, RES_SHARDS },
    { TR_VULN_NEXUS, RES_NEXUS },
    { TR_VULN_BLIND, RES_BLIND },
    { TR_VULN_NETHER, RES_NETHER },
    { -1, -1 }
};

void res_calc_bonuses(u32b flgs[TR_FLAG_SIZE])
{
    int i;
    for (i = 0; ; i++)
    {
        _map_t m = _resist_map[i];
        if (m.flg < 0) break;
        if (have_flag(flgs, m.flg))
            res_add(m.res);
    }
    for (i = 0; ; i++)
    {
        _map_t m = _immunity_map[i];
        if (m.flg < 0) break;
        if (have_flag(flgs, m.flg))
            res_add_immune(m.res);
    }
    for (i = 0; ; i++)
    {
        _map_t m = _vulnerability_map[i];
        if (m.flg < 0) break;
        if (have_flag(flgs, m.flg))
            res_add_vuln(m.res);
    }
}

int  res_get_object_flag(int which)
{
    int i;
    for (i = 0; ; i++)
    {
        _map_t m = _resist_map[i];
        if (m.flg < 0) break;
        if (m.res == which) return m.flg;
    }
    return -1;
}

static int _randomize(int pct)
{
    if (pct != 100)
    {
        if (pct > 0)
            pct = randnor(pct, pct/10);
        else if (pct < 0)
            pct = -randnor(-pct, -pct/10);
    }
    return pct;
}
int res_calc_dam(int which, int dam)
{
    int pct1 = res_pct(which);
    int pct2 = _randomize(pct1);
    int result = dam;

    result -= pct2 * dam / 100;
    if (result < 0)
        result = 0;

    return result;
}

void res_clear(void)
{
    int i;
    for (i = 0; i < RES_MAX; i++)
        p_ptr->resist[i] = 0;
}

static cptr _names[RES_MAX] = {
    "acid",
    "lightning",
    "fire",
    "cold",
    "poison",
    "light",
    "dark",
    "confusion",
    "nether",
    "nexus",
    "sound",
    "shards",
    "chaos",
    "disenchantment",
    "time",
    "blindness",
    "fear",
    "teleportation",
};

cptr res_name(int which)
{
    if (which >= 0 && which < RES_MAX)
        return _names[which];
    return "";
}

#define _MAX_PCTS 29
static int _lo_pcts[_MAX_PCTS] = {
   0, 50, 65, 72, 75,
  77, 78, 79, 80, 81,
  82, 83, 84, 85, 86,
  87, 88, 89, 90, 91,
  92, 93, 94, 95, 96,
  97, 98, 99, 100
};

static int _hi_pcts[_MAX_PCTS] = {
   0, 30, 40, 45, 47,
  48, 49, 50, 51, 52,
  53, 54, 55, 56, 57,
  58, 59, 60, 61, 62,
  63, 64, 65, 66, 67,
  68, 69, 70, 100
};
/* Note: I've decided to move back to a two-tiered resistance system.
 * While I like the simplicity of a single system, the fact is that it
 * forced the player to cover all resists, especially rather early on when
 * it is practically impossible to do so. This redesign is closer to the
 * original Hengband system. Also, you can no longer "shut-down" end game
 * high damage with multiple resists like before:
 * Attack  Hengband(1x) Old(3x) New(3x) New(1x)
 * ======= ============ ======= ======= =======
 * ROCKET           400     224     330     420
 * BR_CHAO          439     196     330     420
 * BR_NETH          356     182     302     385

 * These are the Serpent's big three attacks. As you can see, he could be
 * made quite tame with enough resists. But no longer ;)
 */


int  res_pct_aux(int which, int count)
{
    int result = 0;
    int idx = count;

    if (count < 0)
        idx *= -1;

    if (idx >= _MAX_PCTS)
        idx = _MAX_PCTS-1;

    if (res_is_low(which))
        result = _lo_pcts[idx];
    else
        result = _hi_pcts[idx];

    if (count < 0)
        result *= -1;
    else if (result < 100)
    {
        if (which == RES_CONF)
        {
            if (prace_is_(RACE_TONBERRY))
                result = (result + 1) / 2;
        }

        if (which == RES_LITE)
        {
            if (prace_is_(RACE_VAMPIRE) || prace_is_(RACE_MON_VAMPIRE) || prace_is_(MIMIC_VAMPIRE))
                result = (result + 1) / 2;
        }

        if (which == RES_FIRE)
        {
            if (prace_is_(RACE_ENT))
                result = result * 7 / 10;
        }

        if (which == RES_ELEC)
        {
            if (prace_is_(RACE_ANDROID))
                result = result * 7 / 10;
        }
    }
    return result;
}

int res_pct(int which)
{
    int ct = p_ptr->resist[which];
    return res_pct_aux(which, ct);
}

bool res_save(int which, int power)
{
    int pct = res_pct(which);
    int roll = randint0(power);
    if (roll < pct)
        return TRUE;
    return FALSE;
}

bool res_save_default(int which)
{
    int power = res_is_low(which) ? 55 : 35;
    return res_save(which, power);
}
bool res_can_ignore(int which)
{
    int power = res_is_low(which) ? 55 : 35;
    if (res_pct(which) >= power)
        return TRUE;
    return FALSE;
}

bool res_save_inventory(int which)
{
    int power = res_is_low(which) ? 66 : 41;
    return res_save(which, power);
}
