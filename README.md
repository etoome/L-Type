<h1 align="center">
    L-Type
</h1>

<p align="center">
    Multiplayer shoot them up inspired by <a href="https://en.wikipedia.org/wiki/R-Type">R-Type</a>
</p>

<div align="center">
  <img width="325" src="https://github.com/Progitiel/l-type/blob/main/screenshots/cli_menu.png">
  <img width="325" src="https://github.com/Progitiel/l-type/blob/main/screenshots/cli_game.png">
  <img width="325" src="https://github.com/Progitiel/l-type/blob/main/screenshots/cli_boss.png">
</div>

<div align="center">
  <img width="325" src="https://github.com/Progitiel/l-type/blob/main/screenshots/gui_menu.png">
  <img width="325" src="https://github.com/Progitiel/l-type/blob/main/screenshots/gui_game_storwors.png">
  <img width="325" src="https://github.com/Progitiel/l-type/blob/main/screenshots/gui_game_moinecroft.png">
  <img width="325" src="https://github.com/Progitiel/l-type/blob/main/screenshots/gui_level_editor.png">
  <img width="325" src="https://github.com/Progitiel/l-type/blob/main/screenshots/gui_packs.png">
</div>

# Play in the browser

```
docker compose up -d --build
```

Go to: https://localhost:1793/

- **User** : `kasm_user`
- **Password**: `password`

# Develop in local

## Dependencies

- [`ncursesw`](https://invisible-island.net/ncurses/)
- [`SQLite 3`](https://sqlite.org/index.html)
- [`gcrypt20`](https://gnupg.org/software/libgcrypt/index.html)
- [`SDL2`](https://www.libsdl.org/)
- [`Python 3`](https://www.python.org/)

### DPKG

To install all dependencies on Debian/Ubuntu:

```bash
sudo apt update && sudo apt upgrade
sudo apt install libncursesw5-dev libsqlite3-dev libgcrypt20-dev libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev python3
```

### RPM

Change for Arch

### Pacman

To install all dependencies on Arch Linux/Manjaro:

```bash
sudo pacman -Syu ncurses sqlite libgcrypt sdl2 sdl2_image sdl2_ttf sdl2_mixer python
```

## Installation

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

- **User** : `admin`
- **Password**: `password`

# Authors
* **[qmagron](https://github.com/qmagron)**
* **[etoome](https://github.com/etoome)**
* **[alfduque](https://github.com/alfduque)**
* **[sebaarte](https://github.com/sebaarte)**
* **[lhalkin](https://github.com/lhalkin)**
* **[tlissenk](https://github.com/tlissenk)**
* **[marveree](https://github.com/marveree)**
