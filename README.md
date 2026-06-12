# UnhideNPCs Vox

Ever been annoyed that enemies, bosses and NPCs disappear when many players are nearby?  
UnhideNPCs keeps them visible.

This is a maintained continuation of
[UnhideNPCs](https://github.com/server-imp/UnhideNPCs), originally created by
**server-imp**. This continuation is maintained by **JordiBrisbois** under the
**Vox** name.

Release builds are traceable: each public revision pins a reviewed commit of
the private client-integration layer, and the release workflow scans the exact
artifact before creating a draft release.

Using this, you could even put `Character Model Limit` to `Lowest` and still see all enemies/bosses/NPCs!

## Project Origin and Credits

- Original project and implementation:
  [server-imp/UnhideNPCs](https://github.com/server-imp/UnhideNPCs), by
  **server-imp** and its contributors.
- Maintained continuation, releases and issue tracking:
  [JordiBrisbois/UnhideNPCs-Vox](https://github.com/JordiBrisbois/UnhideNPCs-Vox),
  by **JordiBrisbois (Vox)**.
- This repository is currently an independent GitHub repository, not an
  official fork or an official release from the original author.

Changes that are generally useful and do not depend on the private
client-integration layer may be proposed upstream to the original project.

## Notes
UnhideNPCs does nothing while in PvP or WvW.

Tested on Windows 11 25H2 and Fedora Linux 43 (GE-Proton10-30).

## In-Game Configuration

This add-on includes an in-game configuration interface.

> **Note:** You can CTRL+Click the sliders if you prefer typing the numbers.
> 
> The images below may or may not show an older version of the plugin

<img src="https://github.com/user-attachments/assets/e8013d39-b235-4b42-b6af-809ffa2ddedb" width="33%">

## Installation

UnhideNPCs requires [Nexus](https://raidcore.gg/Nexus).

Install it directly from the in-game Nexus Library.

ArcDPS and standalone DLL proxy loading are not supported.

## Configuration

Hotkeys are saved at `<game folder>/addons/UnhideNPCs/hotkeys.json`.

Settings are saved at `<game folder>/addons/UnhideNPCs/settings.json`.

## License

This project is licensed under the **GNU General Public License v3.0 (GPLv3)**.  
You are free to use, modify and distribute this software under the terms of the GPLv3.  
See the [LICENSE](LICENSE) file for full details.

## Contact / Issues

If you encounter issues or have questions, please open an issue in this repository.  
When reporting a problem, include these if relevant:

- Steps to reproduce the issue.
- Relevant logs from `<game folder>/addons/UnhideNPCs/log.txt`.
