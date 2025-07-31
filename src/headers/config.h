#ifndef CONFIG_H
#define CONFIG_H

#define BORDER 4
#define FOCUSED 0x9ea7aa
#define UNFOCUSED 0x2a2c2e
#define GAP 15

/*
 * MOD1 = Alt
 * MOD2 = Shift
 * MOD3 = Control
 * MOD4 = Super
 */
#define MOD1 XCB_MOD_MASK_1
#define MOD2 XCB_MOD_MASK_SHIFT
#define MOD3 XCB_MOD_MASK_CONTROL
#define MOD4 XCB_MOD_MASK_4
static const neko_keybind keybinds[] = {
    {MOD1, XK_Return, "kitty"},
    {MOD1 | MOD2, XK_Return, "dmenu_run"},
    {MOD1 | MOD2, XK_r, "nekowm --exit-neko"},
    {MOD1, XK_t, "nekowm --split-toggle"},
};

/* Comment these out to disable them */
#define SOCKET
#define LOG

#define SOCKET_PATH "/tmp/neko.sock"
#define LOG_PATH "/tmp/neko.log"

#endif /* !CONFIG_H */
