#ifndef CONFIG_H
#define CONFIG_H

#define BORDER 					4
#define FOCUSED 				0xf200ff
#define UNFOCUSED 			0x440074
#define GAP 						15

/*
 * MOD1 = Alt
 * MOD4 = Super
 */
#define MOD1 						XCB_MOD_MASK_1
#define MOD2						XCB_MOD_MASK_SHIFT
#define MOD3						XCB_MOD_MASK_CONTROL
#define MOD4						XCB_MOD_MASK_4
static const neko_keybind keybinds[] =
{
	{ MOD1, XK_Return, "kitty" },
	{ MOD1 | MOD2, XK_Return, "dmenu_run" },
};

#endif /* !CONFIG_H */
