# DiscordDrive Firmware

DiscordDrive is an open-source brushless motor driver with FOC control.

# Building

DiscordDrive is built with [PlatformIO](https://github.com/platformio/platformio-core/).

To build, enter the root directory and type:
> platformio run

Since there are not any specialized boards for DiscordDrive currently, you must upload the build to an STM32F4 Discovery board or similar:
> platformio run -t upload

# Discord server

https://discord.gg/UTTVQDv
