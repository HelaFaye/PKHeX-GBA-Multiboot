// oracle dump <file.sav>: what PKHeX.Core reads from a Generation 3 save, as
// JSON. The editor's tests compare it with what the editor wrote.
using System.Text.Json;
using PKHeX.Core;

if (args.Length != 2 || args[0] != "dump") { Console.Error.WriteLine("oracle dump <file.sav>"); return 2; }
var sav = SaveUtil.GetSaveFile(File.ReadAllBytes(args[1]));
if (sav is not SAV3 s3) { Console.Error.WriteLine("not a Generation 3 save"); return 1; }

static object Mon(PK3 p) => new {
    species = p.Species, nickname = p.Nickname, is_nicknamed = p.IsNicknamed, egg = p.IsEgg,
    level = p.CurrentLevel, exp = p.EXP, item = p.HeldItem, pid = p.PID, tid = p.TID16, sid = p.SID16,
    ot = p.OriginalTrainerName, ot_gender = p.OriginalTrainerGender, nature = (int)p.Nature, ability_bit = p.AbilityBit,
    friendship = p.OriginalTrainerFriendship, pokerus_strain = p.PokerusStrain, pokerus_days = p.PokerusDays,
    met_location = p.MetLocation, met_level = p.MetLevel, version = (int)p.Version, ball = p.Ball,
    fateful = p.FatefulEncounter, language = (int)p.Language, markings = p.MarkingValue,
    moves = new[] { p.Move1, p.Move2, p.Move3, p.Move4 },
    pp = new[] { p.Move1_PP, p.Move2_PP, p.Move3_PP, p.Move4_PP },
    pp_ups = new[] { p.Move1_PPUps, p.Move2_PPUps, p.Move3_PPUps, p.Move4_PPUps },
    ivs = new[] { p.IV_HP, p.IV_ATK, p.IV_DEF, p.IV_SPE, p.IV_SPA, p.IV_SPD },
    evs = new[] { p.EV_HP, p.EV_ATK, p.EV_DEF, p.EV_SPE, p.EV_SPA, p.EV_SPD },
    contest = new[] { p.ContestCool, p.ContestBeauty, p.ContestCute, p.ContestSmart, p.ContestTough, p.ContestSheen },
    ribbon_count = p.RibbonCount, shiny = p.IsShiny,
    stats = new[] { p.Stat_HPMax, p.Stat_ATK, p.Stat_DEF, p.Stat_SPE, p.Stat_SPA, p.Stat_SPD }, hp = p.Stat_HPCurrent,
    checksum_ok = p.Valid, legal = new LegalityAnalysis(p).Valid,
};

var party = s3.PartyData.Select(p => Mon((PK3)p)).ToArray();
var boxes = new List<object>();
for (int b = 0; b < s3.BoxCount; b++)
    for (int k = 0; k < s3.BoxSlotCount; k++) {
        var p = (PK3)s3.GetBoxSlotAtIndex(b, k);
        if (p.Species != 0) boxes.Add(new { box = b, slot = k, mon = Mon(p) });
    }
var pouches = s3.Inventory.Pouches.Select(x => new {
    type = x.Type.ToString(), items = x.Items.Where(i => i.Index != 0).Select(i => new[] { i.Index, i.Count }).ToArray() }).ToArray();
var json = new {
    game = s3.Version.ToString(), checksums_ok = s3.ChecksumsValid,
    trainer = new { name = s3.OT, gender = s3.Gender, tid = s3.TID16, sid = s3.SID16, money = s3.Money, coins = s3.Coin,
        hours = s3.PlayedHours, minutes = s3.PlayedMinutes, seconds = s3.PlayedSeconds },
    box_names = Enumerable.Range(0, s3.BoxCount).Select(s3.GetBoxName).ToArray(),
    dex_seen = Enumerable.Range(1, s3.MaxSpeciesID).Count(i => s3.GetSeen((ushort)i)),
    dex_caught = Enumerable.Range(1, s3.MaxSpeciesID).Count(i => s3.GetCaught((ushort)i)),
    party, boxes, pouches,
};
Console.WriteLine(JsonSerializer.Serialize(json, new JsonSerializerOptions { WriteIndented = true }));
return 0;
