#ifndef CONFIG_H
#define CONFIG_H

#include <X11/keysym.h>
#include <X11/keysymdef.h>

/*
 *  0 = Bottom
 *  1 = Top
 */
#define BAR_POSITION 0
#define BAR_COLOR 0x242428
#define BAR_TEXT_COLOR 0x546c84
#define BAR_HEIGHT 30
#define BAR_BORDER 1
#define BAR_BORDER_COLOR 0x9ea7aa
#define BORDER 4
#define FOCUSED 0x827ee0
#define UNFOCUSED 0x545084
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
    {MOD1, XK_Return, "echo \"Example1\""},
    {MOD1, XK_d, "echo \"Example2\""},

    /* Examples using nekowm messages */
    {MOD1 | MOD2, XK_e, "nekowm --exit-neko"},
    {MOD1, XK_space, "nekowm --split-toggle"},
    {MOD1, XK_t, "nekowm --close-focused"},
    {MOD1 | MOD3, XK_Right, "nekowm --next-stack"},
    {MOD1 | MOD3, XK_Left, "nekowm --previous-stack"},
    {MOD1, XK_Left, "nekowm --focus-previous"},
    {MOD1, XK_Right, "nekowm --focus-next"},
    {MOD1, XK_f, "nekowm --fullscreen"},
};

static const neko_startup_command startup[] = {
    {"echo \"Example running after 2 seconds\"", 2},
};

/* Comment these out to disable them */
#define SOCKET
#define LOG
#define BAR

#define SOCKET_PATH "/tmp/neko.sock"
#define LOG_PATH "/tmp/neko.log"

#endif /* !CONFIG_H */
