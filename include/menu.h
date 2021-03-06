#pragma once
#include "font.h"
#include "timer.h"
#include "input_viewer.h"
#include "cheats.h"
#include "commands.h"
#include "libtp_c/include/utils.h"
#include "libtp_c/include/controller.h"

#define MAX_LIST_MEMBER_LENGTH 40
#define MAX_LIST_ITEMS 20
#define MAX_LINE_LENGTH 50
#define MAX_DESCRIPTION_LENGTH 100

#define MAX_MENU_RENDER_FLAGS 22

enum MenuIndex {
    MN_NONE_INDEX = -1,
    MN_MAIN_MENU_INDEX = 0,
    MN_INVENTORY_INDEX,
    MN_ITEM_WHELL_INDEX,
    MN_WARPING_INDEX,
    MN_MEMORY_INDEX,
    MN_WATCHES_INDEX,
    MN_MEMORY_EDITOR_INDEX,
    MN_FLAGS_INDEX,
    MN_GENERAL_FLAGS_INDEX,
    MN_DUNGEON_FLAGS_INDEX,
    MN_PORTAL_FLAGS_INDEX,
    MN_TEMP_FLAGS_INDEX,
    MN_PRACTICE_INDEX,
    MN_CHEAT_INDEX,
    MN_SCENE_INDEX,
    MN_SETTINGS_INDEX,
    MN_TOOLS_INDEX,
    MN_PAUSE_INDEX,
    MN_AMOUNTS_INDEX,
    MN_ANY_SAVES_INDEX,
    MN_HUNDO_SAVES_INDEX,
    MN_POS_SETTINGS_INDEX,
};

struct ListMember {
    char member[MAX_LIST_MEMBER_LENGTH];
};

struct Line {
    char line[MAX_LINE_LENGTH];
    const int idx;
    char description[MAX_DESCRIPTION_LENGTH];
    bool toggleable = false;
    bool* activation_flag;
    uint8_t max_y_cursor_options;
    char value[MAX_LINE_LENGTH] = {0};
};

class Menu;

namespace MenuRendering {
    void render_active_menus();
    void set_menu(MenuIndex idx);
    void close_active_menus();
    bool is_menu_open();
}  // namespace MenuRendering

class Menu {
   public:
    Menu() {}
};