# Dependencies

- [`ncursesw`](https://invisible-island.net/ncurses/)
- [`SQLite 3`](https://sqlite.org/index.html)
- [`gcrypt20`](https://gnupg.org/software/libgcrypt/index.html)
- [`SDL2`](https://www.libsdl.org/)
- [`Python 3`](https://www.python.org/)

## DPKG

To install all dependencies on Debian/Ubuntu:

```bash
sudo apt update && sudo apt upgrade
sudo apt install libncursesw5-dev libsqlite3-dev libgcrypt20-dev libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev python3
```

## RPM

Change for Arch

## Pacman

To install all dependencies on Arch Linux/Manjaro:

```bash
sudo pacman -Syu ncurses sqlite libgcrypt sdl2 sdl2_image sdl2_ttf sdl2_mixer python
```

# Installation

Make sure you have installed all of the above dependencies before continuing.

To build without running:

```bash
make
```

To build and run client/server independently:

```bash
make run-server
make run-cli
make run-gui
```

<u>Note</u>: the server must be run before the client.

# Administrator

```
python3 admin.py
```

# Contributors

- qmagron
- Progitiel
- alfduque
- sebaarte
- lhalkin
- tlissenk
- marveree
