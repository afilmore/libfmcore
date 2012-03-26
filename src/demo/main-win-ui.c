/*
 *      main-win-ui.c
 *      
 *      Copyright 2009 PCMan <pcman.tw@gmail.com>
 *      
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *      
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *      
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */

/* this file is included by main-win.c */

static const char main_menu_xml[] = 
"<menubar>"
  "<menu action='FileMenu'>"
    "<menuitem action='New'/>"
    "<menuitem action='Close'/>"
  "</menu>"
  "<menu action='EditMenu'>"
    "<menuitem action='Cut'/>"
    "<menuitem action='Copy'/>"
    "<menuitem action='Paste'/>"
    "<menuitem action='Del'/>"
    "<separator/>"
    "<menuitem action='Rename'/>"
    "<menuitem action='Link'/>"
    "<menuitem action='MoveTo'/>"
    "<menuitem action='CopyTo'/>"
    "<separator/>"
    "<menuitem action='SelAll'/>"
    "<menuitem action='InvSel'/>"
    "<separator/>"
    "<menuitem action='Pref'/>"
  "</menu>"
  "<menu action='GoMenu'>"
    "<menuitem action='Prev'/>"
    "<menuitem action='Next'/>"
    "<menuitem action='Up'/>"
    "<separator/>"
    "<menuitem action='Home'/>"
    "<menuitem action='Desktop'/>"
    "<menuitem action='Computer'/>"
    "<menuitem action='Trash'/>"
    "<menuitem action='Network'/>"
    "<menuitem action='Apps'/>"
  "</menu>"
  "<menu action='BookmarksMenu'>"
    "<menuitem action='AddBookmark'/>"
  "</menu>"
  "<menu action='ViewMenu'>"
    "<menuitem action='ShowHidden'/>"
    "<separator/>"
    "<menuitem action='IconView'/>"
    "<menuitem action='CompactView'/>"
    "<menuitem action='ThumbnailView'/>"
    "<menuitem action='ListView'/>"
    "<separator/>"
    "<menu action='Sort'>"
      "<menuitem action='Desc'/>"
      "<menuitem action='Asc'/>"
      "<separator/>"
      "<menuitem action='ByName'/>"
      "<menuitem action='ByMTime'/>"
    "</menu>"
  "</menu>"
  "<menu action='HelpMenu'>"
    "<menuitem action='About'/>"
  "</menu>"
"</menubar>"
"<toolbar>"
    "<toolitem action='New'/>"
    "<toolitem action='Prev'/>"
    "<toolitem action='Up'/>"
    "<toolitem action='Home'/>"
    "<toolitem action='Go'/>"
"</toolbar>"
"<popup>"
  "<menu action='CreateNew'>"
    "<menuitem action='NewFolder'/>"
    "<menuitem action='NewBlank'/>"
  "</menu>"
  "<separator/>"
  "<menuitem action='Paste'/>"
  "<menu action='Sort'>"
    "<menuitem action='Desc'/>"
    "<menuitem action='Asc'/>"
    "<separator/>"
    "<menuitem action='ByName'/>"
    "<menuitem action='ByMTime'/>"
  "</menu>"
  "<menuitem action='ShowHidden'/>"
  "<separator/>"
  "<menuitem action='Prop'/>"
"</popup>"
"<accelerator action='Location'/>"
"<accelerator action='Location2'/>";

static GtkActionEntry main_win_actions[]=
{
    {"FileMenu", NULL, N_("_File"), NULL, NULL, NULL},
        {"New", GTK_STOCK_NEW, N_("_New Window"), "<Ctrl>N", NULL, G_CALLBACK(on_new_win)},
        {"Close", GTK_STOCK_CLOSE, N_("_Close Window"), "<Ctrl>W", NULL, G_CALLBACK(on_close_win)},
    {"EditMenu", NULL, N_("_Edit"), NULL, NULL, NULL},
        {"Cut", GTK_STOCK_CUT, NULL, NULL, NULL, G_CALLBACK(on_cut)},
        {"Copy", GTK_STOCK_COPY, NULL, NULL, NULL, G_CALLBACK(on_copy)},
        {"Paste", GTK_STOCK_PASTE, NULL, NULL, NULL, G_CALLBACK(on_paste)},
        {"Del", GTK_STOCK_DELETE, NULL, NULL, NULL, G_CALLBACK(on_del)},
        {"Rename", NULL, N_("Rename"), "F2", NULL, G_CALLBACK(on_rename)},
        {"Link", NULL, N_("Create Symlink"), NULL, NULL, NULL},
        {"MoveTo", NULL, N_("Move To..."), NULL, NULL, G_CALLBACK(on_move_to)},
        {"CopyTo", NULL, N_("Copy To..."), NULL, NULL, G_CALLBACK(on_copy_to)},
        {"SelAll", GTK_STOCK_SELECT_ALL, NULL, NULL, NULL, G_CALLBACK(on_select_all)},
        {"InvSel", NULL, N_("Invert Selection"), NULL, NULL, G_CALLBACK(on_invert_select)},
        {"Pref", GTK_STOCK_PREFERENCES, NULL, NULL, NULL, NULL},
    {"ViewMenu", NULL, N_("_View"), NULL, NULL, NULL},
        {"Sort", NULL, N_("_Sort Files"), NULL, NULL, NULL},
    {"HelpMenu", NULL, N_("_Help"), NULL, NULL, NULL},
        {"About", GTK_STOCK_ABOUT, NULL, NULL, NULL, G_CALLBACK(on_about)},
    {"GoMenu", NULL, N_("_Go"), NULL, NULL, NULL},
        {"Prev", GTK_STOCK_GO_BACK, N_("Previous Folder"), "<Alt>Left", N_("Previous Folder"), G_CALLBACK(on_go_back)},
        {"Next", GTK_STOCK_GO_FORWARD, N_("Next Folder"), "<Alt>Right", N_("Next Folder"), G_CALLBACK(on_go_forward)},
        {"Up", GTK_STOCK_GO_UP, N_("Parent Folder"), "<Alt>Up", N_("Go to parent Folder"), G_CALLBACK(on_go_up)},
        {"Home", "user-home", N_("Home Folder"), "<Alt>Home", N_("Home Folder"), G_CALLBACK(on_go_home)},
        {"Desktop", "user-desktop", N_("Desktop"), NULL, N_("Desktop Folder"), G_CALLBACK(on_go_desktop)},
        {"Computer", "computer", N_("My Computer"), NULL, NULL, G_CALLBACK(on_go_computer)},
        {"Trash", "user-trash", N_("Trash Can"), NULL, NULL, G_CALLBACK(on_go_trash)},
        {"Network", GTK_STOCK_NETWORK, N_("Network Drives"), NULL, NULL, G_CALLBACK(on_go_network)},
        {"Apps", "system-software-install", N_("Applications"), NULL, N_("Installed Applications"), G_CALLBACK(on_go_apps)},
        {"Go", GTK_STOCK_JUMP_TO, NULL, NULL, NULL, G_CALLBACK(on_go)},
    {"BookmarksMenu", NULL, N_("_Bookmarks"), NULL, NULL, NULL},
        {"AddBookmark", GTK_STOCK_ADD, N_("Add To Bookmarks"), NULL, N_("Add To Bookmarks"), NULL},
    /* for accelerators */
    {"Location", NULL, NULL, "<Alt>d", NULL, G_CALLBACK(on_location)},
    {"Location2", NULL, NULL, "<Ctrl>L", NULL, G_CALLBACK(on_location)},
    /* for popup menu */
    {"CreateNew", GTK_STOCK_NEW, NULL, NULL, NULL, NULL},
    {"NewFolder", "folder", N_("Folder"), NULL, NULL, G_CALLBACK(on_create_new)},
    {"NewBlank", "text-x-generic", N_("Blank FIle"), NULL, NULL, G_CALLBACK(on_create_new)},
    {"Prop", GTK_STOCK_PROPERTIES, NULL, NULL, NULL, G_CALLBACK(on_prop)}
};

static GtkToggleActionEntry main_win_toggle_actions[]=
{
    {"ShowHidden", NULL, N_("Show _Hidden"), "<Ctrl>H", NULL, G_CALLBACK(on_show_hidden), FALSE}
};

static GtkRadioActionEntry main_win_mode_actions[]=
{
    {"IconView", NULL, N_("_Icon View"), NULL, NULL, FM_FV_ICON_VIEW},
    {"CompactView", NULL, N_("_Compact View"), NULL, NULL, FM_FV_COMPACT_VIEW},
    {"ThumbnailView", NULL, N_("Thumbnail View"), NULL, NULL, FM_FV_THUMBNAIL_VIEW},
    {"ListView", NULL, N_("Detailed _List View"), NULL, NULL, FM_FV_LIST_VIEW},
};

static GtkRadioActionEntry main_win_sort_type_actions[]=
{
    {"Asc", GTK_STOCK_SORT_ASCENDING, NULL, NULL, NULL, GTK_SORT_ASCENDING},
    {"Desc", GTK_STOCK_SORT_DESCENDING, NULL, NULL, NULL, GTK_SORT_DESCENDING},
};

static GtkRadioActionEntry main_win_sort_by_actions[]=
{
    {"ByName", NULL, N_("By _Name"), NULL, NULL, COL_FILE_NAME},
    {"ByMTime", NULL, N_("By _Modification Time"), NULL, NULL, COL_FILE_MTIME}
};


static const char folder_menu_xml[]=
"<popup>"
  "<placeholder name='ph1'>"
    /* "<menuitem action='NewTab'/>" */
    "<menuitem action='NewWin'/>"
    /* "<menuitem action='Search'/>" */
  "</placeholder>"
"</popup>";

/* Action entries for pupup menus */
static GtkActionEntry folder_menu_actions[]=
{
    /* {"NewTab", GTK_STOCK_NEW, N_("Open in New Tab"), NULL, NULL, G_CALLBACK(on_open_in_new_tab)}, */
    {"NewWin", GTK_STOCK_NEW, N_("Open in New Window"), NULL, NULL, G_CALLBACK(on_open_in_new_win)},
    {"Search", GTK_STOCK_FIND, NULL, NULL, NULL, NULL}
};

