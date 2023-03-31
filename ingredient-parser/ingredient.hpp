#pragma once
//
// Created by Simon on 26/01/2023.
//
#include <string>
#include <array>
#include <algorithm>

namespace CraftOpt {
    using value_t = int32_t;

    static constexpr auto s_Craft_Slot_Count = 6;

    // Fixed size map used for constexpr lookup
    template <typename Key, typename Value, std::size_t Size, bool reversed = false>
    struct Map {
        std::array<std::conditional_t<!reversed, std::pair<Key, Value>, std::pair<Value, Key>>, Size> data;

        [[nodiscard]] constexpr Value at(const Key& key) const {
            const auto itr =
                    std::find_if(begin(data), end(data), [&key](const auto& v) {
                        if constexpr (reversed) {
                            return v.second == key;
                        } else {
                            return v.first == key;
                        }
                    });
            if (itr != end(data)) {
                if constexpr (reversed) {
                    return itr->first;
                } else {
                    return itr->second;
                }
            } else {
                throw std::range_error("Key not found");
            }
        }
    };

    enum ProfessionType {
        SCRIBING = 1,
        JEWELING = 2,
        ALCHEMISM = 4,
        COOKING = 8,
        WEAPONSMITHING = 16,
        TAILORING = 32,
        WOODWORKING = 64,
        ARMOURING = 128
    };

    enum SkillType {
        Strength, Dexterity, Intelligence, Defence, Agility, Skill_Type_Count = Agility + 1
    };

    enum EffectivenessType {
        Pos_Empty, Pos_Left, Pos_Right, Pos_Above, Pos_Under, Pos_Touching, Pos_NotTouching, Pos_Type_Count = Pos_NotTouching
    };

    enum IDType {
        ID_Empty, // default initialized value. used to identify the end of id's
        SP_Strength, SP_Dexterity, SP_Intelligence, SP_Defence, SP_Agility,
        Walk_Speed,
        Sprint,
        Sprint_Regen,
        Jump_Height,
        Health_Bonus,
        Health_Regen,
        Health_Regen_Percent,
        Life_Steal,
        Mana_Regen,
        Mana_Steal,
        Poison,
        Exploding,
        Attack_Speed,
        Main_Attack_Damage,
        Main_Attack_Damage_Percent,
        Spell_Damage,
        Spell_Damage_Percent,
        Earth_Damage_Percent,
        Thunder_Damage_Percent,
        Water_Damage_Percent,
        Fire_Damage_Percent,
        Air_Damage_Percent,
        Earth_Defence_Percent,
        Thunder_Defence_Percent,
        Water_Defence_Percent,
        Fire_Defence_Percent,
        Air_Defence_Percent,
        Thorns,
        Reflection,
        Stealing,
        Xp_Bonus,
        Gather_Speed,
        Gather_Xp,
        Loot_Bonus,
        Loot_Quality,
        Soul_Point_Regen,

        ID_Types_Count = Soul_Point_Regen
    };

    // Sized template to reduce the size of the struct, when max amount of ids is known.
    template<std::size_t ID_COUNT = ID_Types_Count, std::size_t POS_COUNT = Pos_Type_Count>
    struct Ingredient {
        value_t id{};
        value_t tier{};
        value_t skills{};
        value_t level{};
        // IDs value range [min, max]
        Map<IDType, std::pair<value_t, value_t>, ID_COUNT> ids{};
        // ItemIDs
        value_t durability{};
        std::array<value_t, Skill_Type_Count> skill_requirements{};
        // ConsumableIDs
        value_t charges{};
        value_t length{};
        // PosMods
        Map<EffectivenessType, value_t, POS_COUNT> position_modifiers{};

        [[nodiscard]] std::size_t id_count() const {
            constexpr auto filter = [](const auto& entry) {return entry.first != ID_Empty;};
            return std::count_if(begin(ids.data), end(ids.data), filter);
        }

        [[nodiscard]] std::size_t effectiveness_count() const {
            constexpr auto filter = [](const auto& entry) {return entry.first != Pos_Empty;};
            return std::count_if(begin(position_modifiers.data), end(position_modifiers.data), filter);
        }
    };
}
