# NekoWM

NekoWM is a lightweight tiling window manager using sequential split-based tiling.

---
### How it works

Newly spawned windows recursively split the remaining available space in the current stack, halving the active region with each new window.

---
### Example

Spawn 2 windows:
```
window1: 50%
window2: 50%
```

Spawn another:
```
window1: 50%
window2: 25%
window3: 25%
```

Spawn one more:
```
window1: 50%
window2: 25%
window3: 12.5%
window4: 12.5%
```

# Philosophy

NekoWM is designed to be a single, self-contained binary with:
- No runtime dependencies (beyond X11 libraries)
- No config files
- No external daemons

NekoWM is supposed to only manage your windows, not more or less. This means:
- Basic messages for window operations like
  - Map window
  - Fullscreen window
  - Close window
- No reconfiguring on runtime
  - No changing monitor mapping
  - No adding or removing stacks

## Building
### Dependencies
- `libxcb` (`keysyms` and `randr`)
- `Xlib` (only for keysyms, see [Xlib note](#xlib))
- `gcc` (edit `Makefile` to use another compiler)
- `make`

### Compile
```sh
make
````
### Install
```sh
make install
```
### Uninstall
```sh
make uninstall
```
### Clean
```sh
make clean
```
### Session
This creates a desktop session entry
```sh
make session
```

## Xlib
Xlib is only used for the `keysym` macros (`XK_*`). You can avoid this dependency by replacing the macros with raw keycode values (e.g., `38`, `40`, etc.).

To disable Xlib entirely, edit `src/neko.h` and `Makefile`.

## Config
NekoWM customization is done by editing the [config file](src/headers/config.h) and compiling from source. Due to the [Philosophy](#Philosophy) of NekoWM there will not be any config file being read from any other place on the system. Customization is done by editing `config.h` and recompiling NekoWM.
### Options
| Name            | Function                                      |
|-----------------|----------------------------------------------|
| BAR_POSITION    | Position of the bar (0 = Bottom, 1 = Top)   |
| BAR_COLOR       | Background color of the bar                 |
| BAR_TEXT_COLOR  | Text color of the bar                       |
| BAR_HEIGHT      | Height of the bar in pixels                 |
| BAR_BORDER      | Width of the bar border                     |
| BAR_BORDER_COLOR| Color of the bar border                     |
| BORDER          | Width of each mapped window border          |
| FOCUSED         | Border color of the focused window          |
| UNFOCUSED       | Border color of unfocused windows           |
| GAP             | Gap between mapped windows                  |
| SOCKET          | Enable/disable UNIX socket support          |
| LOG             | Enable/disable logging                      |
| BAR             | Enable/disable bar rendering                |
| SOCKET_PATH     | Path for the UNIX socket                    |
| LOG_PATH        | Path of the log file                        |

### Mod Keys
- MOD1 = Alt
- MOD2 = Shift
- MOD3 = Control
- MOD4 = Super

### Keybinds
Example:
`{ MOD, Key, Command },`

**Note**
Each mod is a flag meaning you can stack them.
Example:
`{ MOD1 | MOD2, XK_Return, "dmenu_run" },`
This will create a keybind `alt + shift + return|enter` which will in response execute dmenu_run

## License
NekoWM is licensed under the [MIT License](LICENSE).
