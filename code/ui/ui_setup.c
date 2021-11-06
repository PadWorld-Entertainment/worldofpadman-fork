/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Quake III Arena source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/
//
/*
=======================================================================

SETUP MENU

=======================================================================
*/

#include "ui_local.h"

#define ID_CUSTOMIZEPLAYER 10
#define ID_CUSTOMIZECONTROLS 11
#define ID_SYSTEMCONFIG 12
#define ID_GAME 13
#define ID_DEFAULTS 14
#define ID_BACK 15

#define PLAYER0 "menu/buttons/player0"
#define PLAYER1 "menu/buttons/player1"
#define CONTROLS0 "menu/buttons/controls0"
#define CONTROLS1 "menu/buttons/controls1"
#define SYSTEM0 "menu/buttons/system0"
#define SYSTEM1 "menu/buttons/system1"
#define OPTIONS0 "menu/buttons/game0"
#define OPTIONS1 "menu/buttons/game1"
#define DEFAULTS0 "menu/buttons/defaults0"
#define DEFAULTS1 "menu/buttons/defaults1"
#define BACK0 "menu/buttons/back0"
#define BACK1 "menu/buttons/back1"

typedef struct {
	menuframework_s menu;
	menubitmap_s Player;
	menubitmap_s Controls;
	menubitmap_s System;
	menubitmap_s Options;
	menubitmap_s Defaults;
	menubitmap_s Back;
} setupMenuInfo_t;

static setupMenuInfo_t setupMenuInfo;

/*
####################### ####################### ####################### ####################### #######################

  new defaults-menu

####################### ####################### ####################### ####################### #######################
*/

#define ID_DEFAULT_YES 10
#define ID_DEFAULT_NO 11
#define ID_DEFAULT_BACK 12

#define YES0 "menu/buttons/yes0"
#define YES1 "menu/buttons/yes1"
#define NO0 "menu/buttons/no0"
#define NO1 "menu/buttons/no1"
// BACK0, BACK1 used from setup menu

typedef struct {
	menuframework_s menu;
	menubitmap_s Yes;
	menubitmap_s No;
	menubitmap_s Back;
} setupDefaultMenu_t;

static setupDefaultMenu_t setupDefaultMenu;

/*
#######################
SetupDefaultMenu_Event
#######################
*/
static void SetupDefaultMenu_Event(void *ptr, int event) {
	if (event != QM_ACTIVATED)
		return;

	switch (((menucommon_s *)ptr)->id) {
	case ID_DEFAULT_YES:
		//		UI_PopMenu();
		trap_Cmd_ExecuteText(EXEC_APPEND, "exec default.cfg\n");
		trap_Cmd_ExecuteText(EXEC_APPEND, "cvar_restart\n");
		trap_Cmd_ExecuteText(EXEC_APPEND, "vid_restart\n");
		break;
	case ID_DEFAULT_NO:
	case ID_DEFAULT_BACK:
		UI_PopMenu();
		break;
	}
}

static void SetupDefaultMenu_Draw(void) {
	UI_DrawStringNS(320 - 145, 380, "This will reset *^1ALL^3* options", UI_LEFT, 20, color_yellow);
	UI_DrawStringNS(320, 380 + 22, "to their default values.", UI_CENTER, 20, color_yellow);
	Menu_Draw(&setupDefaultMenu.menu);
}

/*
#######################
SetupDefaultMenu_Init
#######################
*/
static void SetupDefaultMenu_Init(void) {
	SetupDefaultMenu_Cache();

	memset(&setupDefaultMenu, 0, sizeof(setupDefaultMenu));
	setupDefaultMenu.menu.draw = SetupDefaultMenu_Draw;
	setupDefaultMenu.menu.wrapAround = qtrue;
	setupDefaultMenu.menu.fullscreen = qtrue;
	setupDefaultMenu.menu.bgparts = BGP_DEFAULTSBG | BGP_SIMPLEBG;

	setupDefaultMenu.Yes.generic.type = MTYPE_BITMAP;
	setupDefaultMenu.Yes.generic.name = YES0;
	setupDefaultMenu.Yes.generic.flags = QMF_LEFT_JUSTIFY | QMF_HIGHLIGHT_IF_FOCUS;
	setupDefaultMenu.Yes.generic.x = 380; // 349;
	setupDefaultMenu.Yes.generic.y = 284; // 241;
	setupDefaultMenu.Yes.generic.id = ID_DEFAULT_YES;
	setupDefaultMenu.Yes.generic.callback = SetupDefaultMenu_Event;
	setupDefaultMenu.Yes.width = 80;
	setupDefaultMenu.Yes.height = 40;
	setupDefaultMenu.Yes.focuspic = YES1;
	setupDefaultMenu.Yes.focuspicinstead = qtrue;
	Menu_AddItem(&setupDefaultMenu.menu, &setupDefaultMenu.Yes);

	setupDefaultMenu.No.generic.type = MTYPE_BITMAP;
	setupDefaultMenu.No.generic.name = NO0;
	setupDefaultMenu.No.generic.flags = QMF_LEFT_JUSTIFY | QMF_HIGHLIGHT_IF_FOCUS;
	setupDefaultMenu.No.generic.x = 497; // 449;
	setupDefaultMenu.No.generic.y = 284; // 241;
	setupDefaultMenu.No.generic.id = ID_DEFAULT_NO;
	setupDefaultMenu.No.generic.callback = SetupDefaultMenu_Event;
	setupDefaultMenu.No.width = 40;
	setupDefaultMenu.No.height = 40;
	setupDefaultMenu.No.focuspic = NO1;
	setupDefaultMenu.No.focuspicinstead = qtrue;
	Menu_AddItem(&setupDefaultMenu.menu, &setupDefaultMenu.No);

	setupDefaultMenu.Back.generic.type = MTYPE_BITMAP;
	setupDefaultMenu.Back.generic.name = BACK0;
	setupDefaultMenu.Back.generic.flags = QMF_LEFT_JUSTIFY | QMF_PULSEIFFOCUS;
	setupDefaultMenu.Back.generic.x = 8;
	setupDefaultMenu.Back.generic.y = 440;
	setupDefaultMenu.Back.generic.id = ID_DEFAULT_BACK;
	setupDefaultMenu.Back.generic.callback = SetupDefaultMenu_Event;
	setupDefaultMenu.Back.width = 80;
	setupDefaultMenu.Back.height = 40;
	setupDefaultMenu.Back.focuspic = BACK1;
	setupDefaultMenu.Back.focuspicinstead = qtrue;
	Menu_AddItem(&setupDefaultMenu.menu, &setupDefaultMenu.Back);

	UI_PushMenu(&setupDefaultMenu.menu);
}

/*
#######################
SetupDefaultMenu_Cache
#######################
*/
void SetupDefaultMenu_Cache(void) {
	trap_R_RegisterShaderNoMip(YES0);
	trap_R_RegisterShaderNoMip(YES1);
	trap_R_RegisterShaderNoMip(NO0);
	trap_R_RegisterShaderNoMip(NO1);
}

/*
===============
UI_SetupMenu_Event
===============
*/
static void UI_SetupMenu_Event(void *ptr, int event) {
	if (event != QM_ACTIVATED) {
		return;
	}

	switch (((menucommon_s *)ptr)->id) {
	case ID_CUSTOMIZEPLAYER:
		UI_PlayerSettingsMenu();
		break;

	case ID_CUSTOMIZECONTROLS:
		UI_ControlsMenu();
		break;

	case ID_SYSTEMCONFIG:
		UI_GraphicsOptionsMenu();
		break;

	case ID_GAME:
		UI_PreferencesMenu();
		break;

	case ID_DEFAULTS:
		SetupDefaultMenu_Init();
		break;

	case ID_BACK:
		UI_PopMenu();
		break;
	}
}

/*
===============
UI_SetupMenu_Init
===============
*/
static void UI_SetupMenu_Init(void) {
	UI_SetupMenu_Cache();

	memset(&setupMenuInfo, 0, sizeof(setupMenuInfo));
	setupMenuInfo.menu.wrapAround = qtrue;
	setupMenuInfo.menu.fullscreen = qtrue;

	setupMenuInfo.menu.noPushSelect = qtrue;
	setupMenuInfo.menu.bgparts = BGP_SETUPBG | BGP_SIMPLEBG;

	setupMenuInfo.Player.generic.type = MTYPE_BITMAP;
	setupMenuInfo.Player.generic.name = PLAYER0;
	setupMenuInfo.Player.generic.flags = QMF_LEFT_JUSTIFY | QMF_HIGHLIGHT_IF_FOCUS;
	setupMenuInfo.Player.generic.x = 59;
	setupMenuInfo.Player.generic.y = 46;
	setupMenuInfo.Player.generic.id = ID_CUSTOMIZEPLAYER;
	setupMenuInfo.Player.generic.callback = UI_SetupMenu_Event;
	setupMenuInfo.Player.width = 120;
	setupMenuInfo.Player.height = 40;
	setupMenuInfo.Player.focuspic = PLAYER1;
	setupMenuInfo.Player.focuspicinstead = qtrue;
	Menu_AddItem(&setupMenuInfo.menu, &setupMenuInfo.Player);

	setupMenuInfo.Controls.generic.type = MTYPE_BITMAP;
	setupMenuInfo.Controls.generic.name = CONTROLS0;
	setupMenuInfo.Controls.generic.flags = QMF_LEFT_JUSTIFY | QMF_HIGHLIGHT_IF_FOCUS;
	setupMenuInfo.Controls.generic.x = 40;
	setupMenuInfo.Controls.generic.y = 87;
	setupMenuInfo.Controls.generic.id = ID_CUSTOMIZECONTROLS;
	setupMenuInfo.Controls.generic.callback = UI_SetupMenu_Event;
	setupMenuInfo.Controls.width = 160;
	setupMenuInfo.Controls.height = 40;
	setupMenuInfo.Controls.focuspic = CONTROLS1;
	setupMenuInfo.Controls.focuspicinstead = qtrue;
	Menu_AddItem(&setupMenuInfo.menu, &setupMenuInfo.Controls);

	setupMenuInfo.System.generic.type = MTYPE_BITMAP;
	setupMenuInfo.System.generic.name = SYSTEM0;
	setupMenuInfo.System.generic.flags = QMF_LEFT_JUSTIFY | QMF_HIGHLIGHT_IF_FOCUS;
	setupMenuInfo.System.generic.x = 42;
	setupMenuInfo.System.generic.y = 127;
	setupMenuInfo.System.generic.id = ID_SYSTEMCONFIG;
	setupMenuInfo.System.generic.callback = UI_SetupMenu_Event;
	setupMenuInfo.System.width = 120;
	setupMenuInfo.System.height = 40;
	setupMenuInfo.System.focuspic = SYSTEM1;
	setupMenuInfo.System.focuspicinstead = qtrue;
	Menu_AddItem(&setupMenuInfo.menu, &setupMenuInfo.System);

	setupMenuInfo.Options.generic.type = MTYPE_BITMAP;
	setupMenuInfo.Options.generic.name = OPTIONS0;
	setupMenuInfo.Options.generic.flags = QMF_LEFT_JUSTIFY | QMF_HIGHLIGHT_IF_FOCUS;
	setupMenuInfo.Options.generic.x = 60;
	setupMenuInfo.Options.generic.y = 167;
	setupMenuInfo.Options.generic.id = ID_GAME;
	setupMenuInfo.Options.generic.callback = UI_SetupMenu_Event;
	setupMenuInfo.Options.width = 80;
	setupMenuInfo.Options.height = 40;
	setupMenuInfo.Options.focuspic = OPTIONS1;
	setupMenuInfo.Options.focuspicinstead = qtrue;
	Menu_AddItem(&setupMenuInfo.menu, &setupMenuInfo.Options);

	if (!trap_Cvar_VariableValue("cl_paused")) {
		setupMenuInfo.Defaults.generic.type = MTYPE_BITMAP;
		setupMenuInfo.Defaults.generic.name = DEFAULTS0;
		setupMenuInfo.Defaults.generic.flags = QMF_LEFT_JUSTIFY | QMF_HIGHLIGHT_IF_FOCUS;
		setupMenuInfo.Defaults.generic.x = 32;
		setupMenuInfo.Defaults.generic.y = 207;
		setupMenuInfo.Defaults.generic.id = ID_DEFAULTS;
		setupMenuInfo.Defaults.generic.callback = UI_SetupMenu_Event;
		setupMenuInfo.Defaults.width = 160;
		setupMenuInfo.Defaults.height = 40;
		setupMenuInfo.Defaults.focuspic = DEFAULTS1;
		setupMenuInfo.Defaults.focuspicinstead = qtrue;
		Menu_AddItem(&setupMenuInfo.menu, &setupMenuInfo.Defaults);
	}

	setupMenuInfo.Back.generic.type = MTYPE_BITMAP;
	setupMenuInfo.Back.generic.name = BACK0;
	setupMenuInfo.Back.generic.flags = QMF_LEFT_JUSTIFY | QMF_PULSEIFFOCUS;
	setupMenuInfo.Back.generic.x = 8;
	setupMenuInfo.Back.generic.y = 440;
	setupMenuInfo.Back.generic.id = ID_BACK;
	setupMenuInfo.Back.generic.callback = UI_SetupMenu_Event;
	setupMenuInfo.Back.width = 80;
	setupMenuInfo.Back.height = 40;
	setupMenuInfo.Back.focuspic = BACK1;
	setupMenuInfo.Back.focuspicinstead = qtrue;
	Menu_AddItem(&setupMenuInfo.menu, &setupMenuInfo.Back);
}

/*
=================
UI_SetupMenu_Cache
=================
*/
void UI_SetupMenu_Cache(void) {
	trap_R_RegisterShaderNoMip(PLAYER0);
	trap_R_RegisterShaderNoMip(PLAYER1);
	trap_R_RegisterShaderNoMip(CONTROLS0);
	trap_R_RegisterShaderNoMip(CONTROLS1);
	trap_R_RegisterShaderNoMip(SYSTEM0);
	trap_R_RegisterShaderNoMip(SYSTEM1);
	trap_R_RegisterShaderNoMip(DEFAULTS0);
	trap_R_RegisterShaderNoMip(DEFAULTS1);
	trap_R_RegisterShaderNoMip(OPTIONS0);
	trap_R_RegisterShaderNoMip(OPTIONS1);
	trap_R_RegisterShaderNoMip(BACK0);
	trap_R_RegisterShaderNoMip(BACK1);
}

/*
===============
UI_SetupMenu
===============
*/
void UI_SetupMenu(void) {
	UI_SetupMenu_Init();
	UI_PushMenu(&setupMenuInfo.menu);
}
