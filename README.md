# NekoWM

NekoWM is a lightweight, stack-like tiling window manager with split-based layouts.

---
### How it works

Newly spawned window splits space with the previously added window in the current stack. With each new window recursively halving the space of the one before it.

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

## Building
### Dependencies
- `libxcb`
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

## License
NekoWM is licensed under the [MIT License](LICENSE).
