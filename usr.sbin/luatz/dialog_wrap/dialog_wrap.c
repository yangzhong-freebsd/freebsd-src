#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <dialog.h>
#include <stdlib.h>
#include <unistd.h>

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
dialog_yesno_wrap(lua_State *L)
{
	int n, result;
	int height, width;
	const char *title, *cprompt;

	n = lua_gettop(L);
	luaL_argcheck(L, n == 4, n > 4 ? 5 : n,
	    "dialogYesNo takes exactly four arguments");

	//stack : title cprompt height width

	title = luaL_checkstring(L, 1);
	cprompt = luaL_checkstring(L, 2);
	height = luaL_checknumber(L, 3);
	width = luaL_checknumber(L, 4);

	result = dialog_yesno(title, cprompt, height, width);

	lua_pushnumber(L, result);
	return 1;
}

static int
dialog_menu_wrap(lua_State *L)
{
	const char *title, *cprompt;
	int height, width, menu_height, item_no;
	DIALOG_LISTITEM *items;
	size_t len;
	const char *selected, *str;
	int n, current_item, result;

	n = lua_gettop(L);
	luaL_argcheck(L, n == 9, n > 9 ? 10 : n,
	    "dialogMenu takes exactly nine arguments");

	//stack : title cprompt height width menu_height item_no current_item keys vals
	
	title = luaL_checkstring(L, 1);
	cprompt = luaL_checkstring(L, 2);
	height = luaL_checknumber(L, 3);
	width = luaL_checknumber(L, 4);
	menu_height = luaL_checknumber(L, 5);
	item_no = luaL_checknumber(L, 6);
	current_item = luaL_checknumber(L, 7);

	items = dlg_calloc(DIALOG_LISTITEM, item_no + 1);

	for (int i = 1; i <= item_no; ++i) {
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

	dialog_vars.default_item = items[current_item].name;

	result = dlg_menu(title, cprompt, height, width, menu_height,
		item_no, items, &current_item, NULL);

	selected = items[current_item].name;

	lua_pushnumber(L, result);
	lua_pushstring(L, selected);

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
	lua_register(L, "dialogYesNo", dialog_yesno_wrap);
	return 0;
}
