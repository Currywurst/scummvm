/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * Original game code copyright (c) 1993-2001 respective authors
 * (see individual files for details).
 * Portions copyright (c) 2005 Vasco Alexandre da Silva Costa
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "gameplay/gp.h"
#include "gameplay/gp_app.h"
#include "organisa/organisa.h"
#include "platform/tc_fs.h"
#include "platform/tc_debug.h"

void tcSaveTheClou(void)
{
    char line[TXT_KEY_LENGTH];
    char location[TXT_KEY_LENGTH];
    char date[TXT_KEY_LENGTH];
    char pathname[DSK_PATH_MAX];
    LIST *games = CreateList();
    NODE *game;
    uword activ;
    Player player = (Player)dbGetObject(Player_Player_1);

    /* in welche Datei ?? */
    ShowMenuBackground();
    txtGetFirstLine(THECLOU_TXT, "SaveGame", line);

    player->CurrScene = film->act_scene->EventNr;
    player->CurrDay = GetDay;
    player->CurrMinute = GetMinute;
    player->CurrLocation = GetLocation;

    dskBuildPathName(DISK_CHECK_FILE, DATADISK, GAMES_LIST_TXT, pathname);
    if (ReadList(games, 0L, pathname)) {
	inpTurnESC(1);
	inpTurnFunctionKey(0);
	activ = (uword) Menu(games, 15L, 0, nullptr, 0L);
	inpTurnFunctionKey(1);

	/* Name erstellen */

	if (activ != GET_OUT) {
	    strcpy(location, GetCurrLocName());
	    BuildDate(GetDay, date);

	    tcCutName(location, (ubyte) ' ', 15);

	    strcat(location, ", ");
	    strcat(location, date);

	    snprintf(date, sizeof(date), "(%d)", activ + 1);
	    strcat(location, date);

	    /* Games.list abspeichern */

	    game = (NODE *)CreateNode(0L, 0L, location);

	    ReplaceNode(games, NODE_NAME(GetNthNode(games, activ)), game);

	    ShowMenuBackground();
	    txtGetFirstLine(THECLOU_TXT, "SAVING", line);
	    PrintStatus(line);

	    WriteList(games, pathname);

	    /* Speichern von tcMain */
            snprintf(line, sizeof(line), "%s%d%s", MAIN_DATA_NAME, activ, GAME_DATA_EXT);
	    dskBuildPathName(DISK_CHECK_DIR, DATADISK, line, pathname);
	    dbSaveAllObjects(pathname, DB_tcMain_OFFSET, DB_tcMain_SIZE, 0);

            snprintf(line, sizeof(line), "%s%d%s", MAIN_DATA_NAME, activ, GAME_REL_EXT);
	    dskBuildPathName(DISK_CHECK_DIR, DATADISK, line, pathname);
	    SaveRelations(pathname, DB_tcMain_OFFSET, DB_tcMain_SIZE, 0);

	    /* Speichern von tcBuild */
            snprintf(line, sizeof(line), "%s%d%s", BUILD_DATA_NAME, activ, GAME_DATA_EXT);
	    dskBuildPathName(DISK_CHECK_DIR , DATADISK, line, pathname);
	    dbSaveAllObjects(pathname, (U32) (DB_tcBuild_OFFSET),
			     (U32) (DB_tcBuild_SIZE), 0);

            snprintf(line, sizeof(line), "%s%d%s", BUILD_DATA_NAME, activ, GAME_REL_EXT);
	    dskBuildPathName(DISK_CHECK_DIR, DATADISK, line, pathname);
	    SaveRelations(pathname, (U32) DB_tcBuild_OFFSET, (U32) DB_tcBuild_SIZE,
			  0);

	    /* Speichern der Story */
            snprintf(line, sizeof(line), "%s%d%s", STORY_DATA_NAME, activ, GAME_DATA_EXT);
	    dskBuildPathName(DISK_CHECK_DIR, DATADISK, line, pathname);
	    tcSaveChangesInScenes(pathname);
	}
    }

    RemoveList(games);
}

ubyte tcLoadIt(char activ)
{
    char pathname[DSK_PATH_MAX];
    char line[TXT_KEY_LENGTH];
    ubyte loaded = 0;

    /* Pre-check: verify the main save file actually exists on disk.
     * GAMES.LST can become stale (e.g. after a crash during save) and show a
     * slot as "saved" even though the data files were never written.
     * Without this guard, dbLoadAllObjects() fails, loaded stays 0, and
     * tcRefreshAfterLoad(0) would call ErrorMsg → tc_QuitGame(). */
    snprintf(line, sizeof(line), "%s%d%s", MAIN_DATA_NAME, (int) activ, GAME_DATA_EXT);
    if (!dskBuildPathName(DISK_CHECK_FILE, DATADISK, line, pathname)
        || !tc_fs_exists(pathname)) {
        tc_warning("TheClou: tcLoadIt slot %d: save data not found ('%s') — aborting load",
                   (int) activ, pathname);
        return 0;
    }

    ShowMenuBackground();
    txtGetFirstLine(THECLOU_TXT, "LOADING", line);
    PrintStatus(line);

    /* alte Daten löschen */

    tcResetOrganisation();

    RemRelations((U32) DB_tcMain_OFFSET, (U32) DB_tcMain_SIZE);
    RemRelations((U32) DB_tcBuild_OFFSET, (U32) DB_tcBuild_SIZE);

    dbDeleteAllObjects((U32) DB_tcMain_OFFSET, (U32) DB_tcMain_SIZE);
    dbDeleteAllObjects((U32) DB_tcBuild_OFFSET, (U32) DB_tcBuild_SIZE);

    /* neue Daten laden ! */

    txtReset(OBJECTS_TXT);

    snprintf(line, sizeof(line), "%s%d%s", MAIN_DATA_NAME, (int) activ, GAME_DATA_EXT);
    dskBuildPathName(DISK_CHECK_FILE, DATADISK, line, pathname);
    if (dbLoadAllObjects(pathname, 0)) {
        snprintf(line, sizeof(line), "%s%d%s", BUILD_DATA_NAME, (int) activ, GAME_DATA_EXT);
        dskBuildPathName(DISK_CHECK_FILE, DATADISK, line, pathname);

	if (dbLoadAllObjects(pathname, 0)) {
            snprintf(line, sizeof(line), "%s%d%s", MAIN_DATA_NAME, (int) activ, GAME_REL_EXT);
            dskBuildPathName(DISK_CHECK_FILE, DATADISK, line, pathname);

	    if (LoadRelations(pathname, 0)) {
                snprintf(line, sizeof(line), "%s%d%s", BUILD_DATA_NAME, (int) activ, GAME_REL_EXT);
                dskBuildPathName(DISK_CHECK_FILE, DATADISK, line, pathname);

		if (LoadRelations(pathname, 0)) {
                    snprintf(line, sizeof(line), "%s%d%s", STORY_DATA_NAME, (int) activ, GAME_DATA_EXT);
                    dskBuildPathName(DISK_CHECK_FILE, DATADISK, line, pathname);

		    if (tcLoadChangesInScenes(pathname))
			loaded = 1;
		}
	    }
	}
    }

    return loaded;
}

ubyte tcLoadTheClou(void)
{
    char line[TXT_KEY_LENGTH];
    ubyte loaded = 0;
    LIST *games = CreateList();
    LIST *origin = CreateList();
    U32 activ;
    Player player;
    char pathname1[DSK_PATH_MAX];
    char pathname2[DSK_PATH_MAX];

    dskBuildPathName(DISK_CHECK_FILE, DATADISK, GAMES_LIST_TXT, pathname1);
    dskBuildPathName(DISK_CHECK_FILE, DATADISK, GAMES_ORIG_TXT, pathname2);

    if (ReadList(games, 0L, pathname1) && ReadList(origin, 0L, pathname2)) {
	ShowMenuBackground();
	txtGetFirstLine(THECLOU_TXT, "LoadAGame", line);

	inpTurnFunctionKey(0);
	inpTurnESC(1);
	activ = (U32) Menu(games, 15L, 0, nullptr, 0L);
	inpTurnFunctionKey(1);

	if ((activ != GET_OUT)
	    &&
	    (strcmp
	     (NODE_NAME(GetNthNode(games, (S32) activ)),
	      NODE_NAME(GetNthNode(origin, (S32) activ))))) {
	    loaded = tcLoadIt((ubyte) activ);
	} else {
	    ShowMenuBackground();

	    txtGetFirstLine(THECLOU_TXT, "NOT_LOADING", line);
	    PrintStatus(line);
	    inpWaitFor(INP_LBUTTONP);

	    ShowMenuBackground();
	    SetLocation(-1);

	    RemoveList(games);
	    RemoveList(origin);

	    if ((player = (Player)dbGetObject(Player_Player_1))) {	/* MOD 04-02 */
		player->CurrScene = film->act_scene->EventNr;

		SceneArgs.ReturnValue = film->act_scene->EventNr;
	    }

	    return 0;
	}
    } else
	ErrorMsg(Disk_Defect, ERROR_MODULE_LOADSAVE, 1);

    tcRefreshAfterLoad(loaded);

    RemoveList(games);
    RemoveList(origin);

    return loaded;
}

void tcRefreshAfterLoad(ubyte loaded)
{
    Player player = (Player)dbGetObject(Player_Player_1);	/* muss hier geholt werden -> sonst alte Adresse */

    if (!loaded) {
	/* Do NOT call ErrorMsg here — it calls tc_QuitGame() which would
	 * terminate the whole engine.  The pre-check in tcLoadIt aborts
	 * before any game state is modified, so the previously loaded game
	 * (e.g. slot 1) is still fully intact.
	 *
	 * Mirror what tcLoadTheClou does for the "not saved / GET_OUT" case:
	 *   - show the NOT_LOADING status message
	 *   - force a location refresh via SetLocation(-1)
	 *   - restore CurrScene and SceneArgs.ReturnValue from the live
	 *     film state so the scene loop continues correctly.
	 *
	 * Crucially: do NOT set player->CurrScene = 0 — that would corrupt
	 * the intact slot-1 state and cause Data errors on the next scene. */
	tc_warning("TheClou: tcRefreshAfterLoad: load failed, returning to game without crashing");
	{
	    char line[TXT_KEY_LENGTH];
	    ShowMenuBackground();
	    txtGetFirstLine(THECLOU_TXT, "NOT_LOADING", line);
	    PrintStatus(line);
	    inpWaitFor(INP_LBUTTONP);
	    ShowMenuBackground();
	    SetLocation(-1);
	}
	if (player && film && film->act_scene) {
	    player->CurrScene       = film->act_scene->EventNr;
	    SceneArgs.ReturnValue   = film->act_scene->EventNr;
	}
    } else {
	if (player) {
	    SetDay(player->CurrDay);
	    SetTime(player->CurrMinute);
	    SetLocation(-1);	/* auf alle Faelle ein Refresh! */

	    SceneArgs.ReturnValue = GetLocScene(player->CurrLocation)->EventNr;
	}
    }
}

ubyte tcSaveChangesInScenes(char *fileName)
{
    U32 i;
    ubyte back = 0;
    TC_FILE *file;

    if ((file = dskOpen(fileName, "wb"))) {
	tc_fprintf(file, "%" PRIu32 "\r\n", film->EnabledChoices);

	for (i = 0; i < film->AmountOfScenes; i++) {
	    tc_fprintf(file, "%" PRIu32 "\r\n", film->gameplay[i].EventNr);
	    tc_fprintf(file, "%" PRIu16 "\r\n", film->gameplay[i].Geschehen);
	}

	dskClose(file);
	back = 1;
    }

    return (back);
}

ubyte tcLoadChangesInScenes(char *fileName)
{
    U32 i;
    ubyte back = 1;
    U32 eventNr, choice;
    U16 count;
    TC_FILE *file;
    struct Scene *sc;

    if ((file = dskOpen(fileName, "rb"))) {
	tc_fscanf(file, "%" SCNu32 "\r\n", &choice);
	SetEnabledChoices(choice);

	for (i = 0; i < film->AmountOfScenes; i++) {
	    tc_fscanf(file, "%" SCNu32 "\r\n", &eventNr);
	    tc_fscanf(file, "%" SCNu16 "\r\n", &count);

	    if ((sc = GetScene(eventNr)))
		sc->Geschehen = count;
	    else
		back = 0;
	}
	dskClose(file);
    } else
	back = 0;

    return (back);
}
