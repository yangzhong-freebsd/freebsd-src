#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <dialog.h>
#include <stdlib.h>
#include <unistd.h>

int luaopen_dialog_wrap(lua_State *);

static int
getopt_wrap(lua_State *L)
{
	const char *optstring;
	int argc;
	char **argv;
	char ch;
	size_t len;

	//stack : argc argv optstring

	argc = (int)lua_tonumber(L, 1);
	optstring = lua_tostring(L, 3);

	argv = malloc((argc + 1) * sizeof(const char*));
	argv[0] = malloc(7 * sizeof(char));
	strcpy(argv[0], "FILLER");

	for (int i = 1; i <= argc; ++i) {
		lua_rawgeti(L, 2, i);
		//stack : argc argv optstring argv[i]
		const char *str = lua_tolstring(L, -1, &len);
		argv[i] = malloc((len + 1) * sizeof(char));
		strncpy(argv[i], str, (len + 1));
		lua_pop(L, 1);
	}

	lua_newtable(L);
	//stack : argc argv optstring []
	int i = 1;
	while ((ch = getopt(argc + 1, argv, optstring)) != -1) {
		lua_newtable(L);
		// stack : ... [... ] {}
		lua_pushstring(L, "ch");
		lua_pushlstring(L, &ch, 1);
		// stack : ... [... ] {} "ch" ch
		lua_rawset(L, -3);
		// stack : ... [... ] {ch = ch}

		lua_pushstring(L, "optarg");
		lua_pushstring(L, optarg);
		lua_settable(L, -3);
		// stack : ... [... ] {ch = ch, optarg = optarg}

		lua_pushstring(L, "optind");
		lua_pushnumber(L, optind);
		lua_settable(L, -3);
		// stack : ... [... ] {ch = ch, optarg = optarg, optind = optind}

		lua_rawseti(L, -2, i);
		// stack : ... [... {ch = ch, optarg = optarg, optind = optind}]
		++i;
	}

	return 1;
}

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
	const char *title, *cprompt;
	int height = 0;
	int width = 0;
	size_t title_len, cprompt_len;

	//stack : title cprompt height width

	title = lua_tolstring(L, 1, &title_len);
	cprompt = lua_tolstring(L, 2, &cprompt_len);
	height = lua_tonumber(L, 3);
	width = lua_tonumber(L, 4);

	int result = dialog_yesno(title, cprompt, height, width);

	lua_pushnumber(L, result);
	return 1;
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
	lua_register(L, "getOpt", getopt_wrap);
	lua_register(L, "dialogYesNo", dialog_yesno_wrap);
	return 0;
}
