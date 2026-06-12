#ifndef UNHIDENPCS_CHECKBOX_GROUP_HPP
#define UNHIDENPCS_CHECKBOX_GROUP_HPP
#pragma once

#include <vector>

struct CheckboxGroupEntry
{
    const char* label {};
    const char* tooltip {};
    bool*       value {};
    bool*       changed {};
};

bool checkboxGroup(const char* label, const char* tooltip, const std::vector<CheckboxGroupEntry>& items);

#endif //UNHIDENPCS_CHECKBOX_GROUP_HPP
