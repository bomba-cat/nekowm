#ifndef CONFIG_H
#define CONFIG_H

#define BORDER 					4
#define FOCUSED 				0xf200ff
#define UNFOCUSED 			0x440074
#define GAP 						15

#define TERM 						"kitty"
#define LAUNCHER 				"dmenu_run"

/*
 * Mod1 = Alt
 * Mod4 = Super
 */
#define MOD 						XCB_MOD_MASK_1
#define SPLIT_KEY 			XK_S
#define TERM_KEY 				XK_Return
#define LAUNCHER_KEY 		XK_D

#endif /* !CONFIG_H */
