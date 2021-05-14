#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <dialog.h>
#include <stdlib.h>

int luaopen_dialog_wrap(lua_State *);

static int
init_dialog_wrap(lua_State *L)
{
	(void)L;
	init_dialog(stdin, stdout);
	return 0;
}

static int
dialog_clear_wrap(lua_State *L)
{
	(void)L;
	dlg_clear();
	return 0;
}

static int
end_dialog_wrap(lua_State *L)
{
	(void)L;
	end_dialog();
	return 0;
}

static int
dialog_menu_wrap(lua_State *L)
{
	const char *title, *cprompt;
	int height = 0;
	int width = 0;
	int menu_height = 0;
	int item_no = 0;
	DIALOG_LISTITEM *items;
	size_t title_len, cprompt_len, len;
	const char *str;
	int choice = 0;
	int result = 0;

	//stack : title cprompt height width menu_height item_no choice keys vals
	
	title = lua_tolstring(L, 1, &title_len);
	cprompt = lua_tolstring(L, 2, &cprompt_len);
	height = (int)lua_tonumber(L, 3);
	width = (int)lua_tonumber(L, 4);
	menu_height = (int)lua_tonumber(L, 5);
	item_no = (int)lua_tonumber(L, 6);
	choice = (int)lua_tonumber(L, 7);

	items = dlg_calloc(DIALOG_LISTITEM, item_no + 1);

	for (int i = 1; i <= (int)item_no; ++i) {
		//stack : ... keys vals
		lua_rawgeti(L, 8, i);
		//stack : ... keys vals keys[i]
		str = lua_tolstring(L, -1, &len);
		items[i-1].name = malloc((len + 1) * sizeof(char));
		strncpy(items[i-1].name, str, (len + 1));
		lua_pop(L, 1);
		//stack : ... keys vals

		lua_rawgeti(L, 9, i);
		//stack : ... keys vals vals[i]
		str = lua_tolstring(L, -1, &len);
		items[i-1].text = malloc((len + 1) * sizeof(char));
		strncpy(items[i-1].text, str, (len + 1));
		lua_pop(L, 1);
	}

	dialog_vars.default_item = items[choice].name;

	result = dlg_menu(title, cprompt, height, width, menu_height,
		item_no, items, &choice, NULL);

	str = items[choice].name;

	lua_pushnumber(L, result);
	lua_pushstring(L, str);


	for(int i = 0; i < item_no; ++i) {
		free(items[i].name);
		free(items[i].text);
	}
	free(items);

	return 2;
}

int
luaopen_dialog_wrap(lua_State *L)
{
	lua_register(L, "dialogMenu", dialog_menu_wrap);
	lua_register(L, "initDialog", init_dialog_wrap);
	lua_register(L, "dialogClear", dialog_clear_wrap);
	lua_register(L, "endDialog", end_dialog_wrap);
	return 0;
}
