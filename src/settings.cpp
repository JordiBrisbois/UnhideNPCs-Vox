#include "settings.hpp"

void forceVisibility(fw::SettingRefVariant&)
{
    ++re::forceVisibility;
}

std::unique_ptr<fw::Settings> Settings::createChild(const std::string& name)
{
    return std::make_unique<SettingsProfile>(this, name);
}

SettingsProfile& Settings::profile() const
{
    const auto idx = _children.empty() ? 0 : ActiveProfile.get() % _children.size();
    return *dynamic_cast<SettingsProfile*>(_children[idx].get());
}

Settings::Settings(const std::filesystem::path& filePath) : fw::Settings(nullptr, "", filePath)
{
    addChild<SettingsProfile>(this, "Default");

    if (!std::filesystem::exists(filePath))
    {
        save(true);
        load();
        return;
    }

    load();
    if (_children.empty())
    {
        addChild<SettingsProfile>(this, "Default");
    }

    for (auto& child : _children)
    {
        if (auto* profile = dynamic_cast<SettingsProfile*>(child.get()))
        {
            profile->resolveConflicts();
        }
    }
    save(true);
}

std::recursive_mutex& Settings::profilesMutex()
{
    return ProfilesMutex;
}

bool Settings::profileExists(const std::string& name) const
{
    return std::any_of(_children.begin(), _children.end(), [&](const auto& child)
    {
        return child->name() == name;
    });
}

std::string Settings::uniqueProfileName(const std::string& baseName) const
{
    auto name   = baseName;
    int  suffix = 1;
    while (profileExists(name))
    {
        name = fmt::format("{} {}", baseName, suffix++);
    }
    return name;
}

void Settings::addProfile(const std::string& name)
{
    std::lock_guard lock(ProfilesMutex);
    if (name.empty() || profileExists(name))
        return;

    addChild<SettingsProfile>(this, name);
    needSave();
}

void Settings::duplicateProfile(const std::string& sourceName, const std::string& newName)
{
    std::lock_guard lock(ProfilesMutex);
    if (sourceName.empty() || newName.empty() || profileExists(newName))
        return;

    const auto source = std::find_if(_children.begin(), _children.end(), [&](const auto& candidate)
    {
        return candidate->name() == sourceName;
    });
    if (source == _children.end())
        return;

    auto* sourceProfile = dynamic_cast<SettingsProfile*>(source->get());
    if (!sourceProfile)
        return;

    auto& duplicate = addChild<SettingsProfile>(this, newName);
    duplicate.copyFrom(*sourceProfile);
    needSave();
}

void Settings::removeProfile(const std::string& name)
{
    std::lock_guard lock(ProfilesMutex);
    if (name.empty() || _children.size() <= 1)
        return;

    const auto child = std::find_if(_children.begin(), _children.end(), [&](const auto& candidate)
    {
        return candidate->name() == name;
    });
    if (child == _children.end())
        return;

    _children.erase(child);
    needSave();
}

void Settings::renameProfile(const std::string& oldName, const std::string& newName)
{
    std::lock_guard lock(ProfilesMutex);
    if (oldName.empty() || newName.empty() || profileExists(newName))
        return;

    for (auto& child : _children)
    {
        if (child->name() == oldName)
        {
            child->setName(newName);
            return;
        }
    }
}
