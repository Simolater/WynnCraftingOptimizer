//
// Created by Simon on 24/03/2023.
//
#pragma once
#include <nlohmann/json.hpp>
#include <fmt/format.h>
#include <vector>
#include <array>
#include <utility>
#include <algorithm>
#include "ingredient.hpp"

using json = nlohmann::json;
using namespace std::literals::string_view_literals;
namespace CraftOpt {

    static constexpr std::array<std::pair<std::string_view, IDType>, ID_Types_Count> id_table {{
        {"str"sv, SP_Strength}, {"dex"sv, SP_Dexterity}, {"int"sv, SP_Intelligence}, {"def"sv, SP_Defence}, {"agi"sv, SP_Agility},
        {"poison"sv, Poison}, {"spd"sv, Walk_Speed}, {"sprint"sv, Sprint}, {"sprintReg"sv, Sprint_Regen}, {"jh"sv, Jump_Height},
        {"hpBonus"sv, Health_Bonus}, {"hprRaw"sv, Health_Regen}, {"hprPct"sv, Health_Regen_Percent}, {"ls"sv, Life_Steal},
        {"sdRaw"sv, Spell_Damage}, {"sdPct"sv, Spell_Damage_Percent},
        {"mdRaw"sv, Main_Attack_Damage}, {"mdPct"sv, Main_Attack_Damage_Percent}, {"atkTier"sv, Attack_Speed},
        {"mr"sv, Mana_Regen}, {"ms"sv, Mana_Steal},
        {"eDefPct"sv, Earth_Defence_Percent}, {"tDefPct"sv, Thunder_Defence_Percent},
        {"wDefPct"sv, Water_Defence_Percent}, {"fDefPct"sv, Fire_Defence_Percent},
        {"aDefPct"sv, Air_Defence_Percent},
        {"eDamPct"sv, Earth_Damage_Percent}, {"tDamPct"sv, Thunder_Damage_Percent},
        {"wDamPct"sv, Water_Damage_Percent}, {"fDamPct"sv, Fire_Damage_Percent},
        {"aDamPct"sv, Air_Damage_Percent},
        {"thorns"sv, Thorns}, {"expd"sv, Exploding}, {"lb"sv, Loot_Bonus}, {"lq"sv, Loot_Quality}, {"ref"sv, Reflection},
        {"xpb"sv, Xp_Bonus}, {"eSteal"sv, Stealing}, {"spRegen"sv, Soul_Point_Regen},
        {"gXp"sv, Gather_Xp}, {"gSpd"sv, Gather_Speed},
        }};

    static constexpr std::array<std::pair<std::string_view, ProfessionType>, 8> profession_table{{
        {"SCRIBING"sv, SCRIBING}, {"JEWELING"sv, JEWELING}, {"ALCHEMISM"sv, ALCHEMISM}, {"COOKING"sv, COOKING},
        {"WEAPONSMITHING"sv, WEAPONSMITHING}, {"TAILORING"sv, TAILORING}, {"WOODWORKING"sv, WOODWORKING}, {"ARMOURING"sv, ARMOURING}
    }};

    static constexpr std::array<std::pair<std::string_view, EffectivenessType>, Pos_Type_Count> effectiveness_table {{
        {"left"sv, Pos_Left}, {"right"sv, Pos_Right}, {"above"sv, Pos_Above}, {"under"sv, Pos_Under}, {"touching"sv, Pos_Touching}, {"notTouching"sv, Pos_NotTouching}
    }};

    [[nodiscard]] auto ingredient_from_json(const json &data) {
        static constexpr auto id_map = Map<std::string_view, IDType, id_table.size()>{{id_table}};
        static constexpr auto profession_map = Map<std::string_view, ProfessionType, profession_table.size()>{{profession_table}};
        static constexpr auto effectiveness_map = Map<std::string_view, EffectivenessType, effectiveness_table.size()>{{effectiveness_table}};

        Ingredient result;
        //result.name = data["name"];
        result.id = data["id"];
        result.tier = data["tier"];
        result.level = data["lvl"];

        result.skills = 0;
        for (const auto &prof: data["skills"]) {
            result.skills = static_cast<value_t>(result.skills | profession_map.at(std::string{prof}));
        }

        std::vector<std::pair<IDType, std::pair<value_t, value_t>>> ids{};
        for (const auto &[key, value] : data["ids"].items()) {
            ids.emplace_back(id_map.at(key), std::make_pair(value["minimum"], value["maximum"]));
        }
        std::copy(begin(ids), end(ids), begin(result.ids.data));

        const auto &itemIDs = data["itemIDs"];
        result.durability = itemIDs["dura"];
        result.skill_requirements[Strength] = itemIDs["strReq"];
        result.skill_requirements[Dexterity] = itemIDs["dexReq"];
        result.skill_requirements[Intelligence] = itemIDs["intReq"];
        result.skill_requirements[Defence] = itemIDs["defReq"];
        result.skill_requirements[Agility] = itemIDs["agiReq"];

        result.charges = data["consumableIDs"]["charges"];
        result.length = data["consumableIDs"]["dura"];

        std::vector<std::pair<EffectivenessType, value_t>> posMods{};
        for (const auto &[key, value] : data["posMods"].items()) {
            if (value == 0) continue;
            posMods.emplace_back(effectiveness_map.at(key), value);
        }
        std::copy(begin(posMods), end(posMods), begin(result.position_modifiers.data));

        return result;
    }

    [[nodiscard]] auto ingredients_from_json(const json &data) -> std::vector<Ingredient<>> {
        std::vector<Ingredient<>> ingredients;
        for (const auto &ingredient_data: data) {
            ingredients.emplace_back(CraftOpt::ingredient_from_json(ingredient_data));
        }
        return ingredients;
    }

}