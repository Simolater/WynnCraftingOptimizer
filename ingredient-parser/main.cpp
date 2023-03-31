//
// Created by Simon on 22/03/2023.
//
#include <iostream>
#include <fstream>
#include <vector>
#include <ranges>
#include "parser.hpp"

namespace CraftOpt {

    [[nodiscard]] static auto ingredient_to_string(const Ingredient<>& ing) {
        auto id_data = ing.ids.data |
                std::views::filter([](const auto& value) {return value.first != ID_Empty;}) |
                std::views::transform([](const auto& value) {
            return fmt::format("std::make_pair(static_cast<CraftOpt::IDType>({}),std::make_pair({},{}))", value.first, value.second.first, value.second.first);
        });
        auto pos_data = ing.position_modifiers.data |
                std::views::filter([](const auto& value) {return value.first != Pos_Empty;}) |
                std::views::transform([](const auto& value) {
                    return fmt::format("std::make_pair(static_cast<CraftOpt::EffectivenessType>({}),{})", value.first, value.second);
                });
        const auto ids = fmt::format("{{{{{}}}}}", fmt::join(id_data, ","));
        return fmt::format("{{{},{},{},{},{},{},{{{}}},{},{},{{{}}}}}",
                           ing.id, ing.tier, ing.skills, ing.level, ids, ing.durability, fmt::join(ing.skill_requirements, ","), ing.charges, ing.length, fmt::join(pos_data, ","));
    }

    [[nodiscard]] static auto ingredients_to_string(std::string_view name, const std::vector<CraftOpt::Ingredient<>>& ingredients) {
        const auto id_count = std::max_element(begin(ingredients), end(ingredients), [](const auto& i1, const auto& i2){return i1.id_count() < i2.id_count();})->id_count();
        const auto effectiveness_count = std::max_element(begin(ingredients), end(ingredients), [](const auto& i1, const auto& i2){return i1.effectiveness_count() < i2.effectiveness_count();})->effectiveness_count();
        return fmt::format("static constexpr std::array<Ingredient<{}, {}>, {}> {} = {{{{{}}}}};",
                           id_count, effectiveness_count, ingredients.size(), name,
                           fmt::join(ingredients | std::views::transform(ingredient_to_string), ","));
    }

    [[nodiscard]] static auto ingredients_profession_to_string(const std::vector<Ingredient<>>& ingredients_normal, const std::vector<Ingredient<>>& ingredients_effectiveness, ProfessionType professionType) {
        static constexpr auto profession_map = Map<ProfessionType, std::string_view, profession_table.size(), true>{{profession_table}};
        std::vector<Ingredient<>> filtered_normal;
        std::copy_if(begin(ingredients_normal), end(ingredients_normal), std::back_inserter(filtered_normal), [professionType](const auto& ing) {return ing.skills & static_cast<value_t>(professionType);});
        std::vector<Ingredient<>> filtered_effectiveness;
        std::copy_if(begin(ingredients_effectiveness), end(ingredients_effectiveness), std::back_inserter(filtered_effectiveness), [professionType](const auto& ing) {return ing.skills & static_cast<value_t>(professionType);});
        return fmt::format(
                R""(template<>
struct Ingredients<{}> {{
    {}
    {}
}};
)"",
                profession_map.at(professionType),
                ingredients_to_string("ingredients", filtered_normal),
                ingredients_to_string("ingredients_effectiveness", filtered_effectiveness));
    };

    static constexpr auto file_template = R"(#pragma once
#include <array>
#include <utility>
#include "ingredient.hpp"

namespace CraftOpt {{

template<ProfessionType>
struct Ingredients {{}};

{}
}}

)"sv;

    static auto generate_header(const std::vector<CraftOpt::Ingredient<>>& ingredients) {
        std::vector<CraftOpt::Ingredient<>> ingredient_effectiveness;
        std::vector<CraftOpt::Ingredient<>> ingredient_normal;
        for (const auto& ing : ingredients) {
            if (ing.effectiveness_count() > 0) {
                ingredient_effectiveness.emplace_back(ing);
            } else {
                ingredient_normal.emplace_back(ing);
            }
        }
        auto data = CraftOpt::profession_table | std::views::transform(
                [&](const auto& value) {
                    return CraftOpt::ingredients_profession_to_string(ingredient_normal, ingredient_effectiveness, value.second);
                });
        return fmt::format(file_template, fmt::join(data, "\n"));
    }
}




int main(int argc, char* argv[]) {
    if (argc != 3) return 1;
    std::ifstream inputFile(argv[1]);
    std::ofstream outputFile(argv[2]);
    const auto ingredients = CraftOpt::ingredients_from_json(json::parse(inputFile));
    outputFile << CraftOpt::generate_header(ingredients);
    outputFile.close();
    return 0;
}
