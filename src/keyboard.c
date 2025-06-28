#include "headers/neko.h"

xcb_key_symbols_t *keysyms = NULL;

const uint16_t ignored_mods[] =
{
  0,
  XCB_MOD_MASK_LOCK,
	XCB_MOD_MASK_2,
  XCB_MOD_MASK_LOCK | XCB_MOD_MASK_2,
};

void neko_grab_keybinds()
{
  keysyms = xcb_key_symbols_alloc(connection);
	xcb_keycode_t *keycodes;
	for(size_t i = 0; i < sizeof(keybinds)/sizeof(neko_keybind); i++)
	{
		keycodes = xcb_key_symbols_get_keycode(keysyms, keybinds[i].key);
		for(xcb_keycode_t *code = keycodes; *code != 0; ++code)
		{
			for(size_t j = 0; j < sizeof(ignored_mods)/sizeof(uint16_t); j++)
			{
				xcb_grab_key(connection, 1, screen->root, keybinds[i].mod | ignored_mods[j], *code, XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC);
			}
		}
		free(keycodes);
	}
}

void neko_read_keybinds(xcb_key_press_event_t *e)
{
	xcb_keysym_t keysym = xcb_key_symbols_get_keysym(keysyms, e->detail, 0);
	for(size_t i = 0; i < sizeof(keybinds)/sizeof(neko_keybind); i++)
	{
		if((e->state & ~(XCB_MOD_MASK_LOCK | XCB_MOD_MASK_2)) == keybinds[i].mod && keysym == keybinds[i].key)
		{
			neko_spawn(keybinds[i].command);
		}
	}
}
