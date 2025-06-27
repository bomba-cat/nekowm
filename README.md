# nekowm
NekoWM is a lightweight, stack-like tiling window manager with split-based layouts.

---
**Idea is following:**

If you add a window to the stack, it will take the previous window and split it's space with that window.

---
**Example:**
Spawn 2 windows:
```
window1: 50%
window2: 50%
```
Spawn another window:
```
window1: 50%
window2: 25%
window3: 25%
```
Spawn one more window:
```
window1: 50%
window2: 25%:
window3: 17.5%
window4: 17.5%
```

# Building
## Dependencies
- libxcb
- gcc (Change `Makefile` for a different compiler)
- make
## Compile
```bash
make
```
## Install
```bash
make install
```
## Uninstall
```bash
make uninstall
```
## Clean
```bash
make clean
```

# License
NekoWM uses the [MIT License](LICENSE)
