/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#pragma once

#include <QtGlobal>

namespace Core {
namespace Constants {

// Modes
const char MODE_WELCOME[]          = "Welcome";
const char MODE_EDIT[]             = "Edit";
const char MODE_DESIGN[]           = "Design";
const int  P_MODE_WELCOME          = 100;
const int  P_MODE_EDIT             = 90;
const int  P_MODE_DESIGN           = 89;

// Menubar
const char MENU_BAR[]              = "QtProject.MenuBar";

// Menus
const char M_FILE[]                = "QtProject.Menu.File";
const char M_FILE_RECENTFILES[]    = "QtProject.Menu.File.RecentFiles";
const char M_EDIT[]                = "QtProject.Menu.Edit";
const char M_EDIT_ADVANCED[]       = "QtProject.Menu.Edit.Advanced";
const char M_TOOLS[]               = "QtProject.Menu.Tools";
const char M_TOOLS_EXTERNAL[]      = "QtProject.Menu.Tools.External";
const char M_WINDOW[]              = "QtProject.Menu.Window";
const char M_WINDOW_PANES[]        = "QtProject.Menu.Window.Panes";
const char M_WINDOW_MODESTYLES[]   = "QtProject.Menu.Window.ModeStyles";
const char M_WINDOW_VIEWS[]        = "QtProject.Menu.Window.Views";
const char M_HELP[]                = "QtProject.Menu.Help";

// Contexts
const char C_GLOBAL[]              = "Global Context";
const char C_WELCOME_MODE[]        = "Core.WelcomeMode";
const char C_EDIT_MODE[]           = "Core.EditMode";
const char C_DESIGN_MODE[]         = "Core.DesignMode";
const char C_EDITORMANAGER[]       = "Core.EditorManager";
const char C_NAVIGATION_PANE[]     = "Core.NavigationPane";
const char C_PROBLEM_PANE[]        = "Core.ProblemPane";
const char C_GENERAL_OUTPUT_PANE[] = "Core.GeneralOutputPane";

// Default editor kind
const char K_DEFAULT_TEXT_EDITOR_DISPLAY_NAME[] = QT_TRANSLATE_NOOP("OpenWith::Editors", "Plain Text Editor");
const char K_DEFAULT_TEXT_EDITOR_ID[] = "Core.PlainTextEditor";
const char K_DEFAULT_BINARY_EDITOR_ID[] = "Core.BinaryEditor";

// actions
const char UNDO[]                  = "QtProject.Undo";
const char REDO[]                  = "QtProject.Redo";
const char COPY[]                  = "QtProject.Copy";
const char PASTE[]                 = "QtProject.Paste";
const char CUT[]                   = "QtProject.Cut";
const char SELECTALL[]             = "QtProject.SelectAll";

const char GOTO[]                  = "QtProject.Goto";
const char ZOOM_IN[]               = "QtProject.ZoomIn";
const char ZOOM_OUT[]              = "QtProject.ZoomOut";
const char ZOOM_RESET[]            = "QtProject.ZoomReset";

const char NEW[]                   = "QtProject.New";
const char OPEN[]                  = "QtProject.Open";
const char OPEN_WITH[]             = "QtProject.OpenWith";
const char REVERTTOSAVED[]         = "QtProject.RevertToSaved";
const char SAVE[]                  = "QtProject.Save";
const char SAVEAS[]                = "QtProject.SaveAs";
const char SAVEALL[]               = "QtProject.SaveAll";
const char PRINT[]                 = "QtProject.Print";
const char EXIT[]                  = "QtProject.Exit";

const char OPTIONS[]               = "QtProject.Options";
const char TOGGLE_LEFT_SIDEBAR[]   = "QtProject.ToggleLeftSidebar";
const char TOGGLE_RIGHT_SIDEBAR[]  = "QtProject.ToggleRightSidebar";
const char CYCLE_MODE_SELECTOR_STYLE[] =
                                     "QtProject.CycleModeSelectorStyle";
const char TOGGLE_FULLSCREEN[]     = "QtProject.ToggleFullScreen";
const char THEMEOPTIONS[]          = "QtProject.ThemeOptions";

const char TR_SHOW_LEFT_SIDEBAR[]  = QT_TRANSLATE_NOOP("Core", "Show Left Sidebar");
const char TR_HIDE_LEFT_SIDEBAR[]  = QT_TRANSLATE_NOOP("Core", "Hide Left Sidebar");

const char TR_SHOW_RIGHT_SIDEBAR[] = QT_TRANSLATE_NOOP("Core", "Show Right Sidebar");
const char TR_HIDE_RIGHT_SIDEBAR[] = QT_TRANSLATE_NOOP("Core", "Hide Right Sidebar");

const char MINIMIZE_WINDOW[]       = "QtProject.MinimizeWindow";
const char ZOOM_WINDOW[]           = "QtProject.ZoomWindow";
const char CLOSE_WINDOW[]           = "QtProject.CloseWindow";

const char SPLIT[]                 = "QtProject.Split";
const char SPLIT_SIDE_BY_SIDE[]    = "QtProject.SplitSideBySide";
const char SPLIT_NEW_WINDOW[]      = "QtProject.SplitNewWindow";
const char REMOVE_CURRENT_SPLIT[]  = "QtProject.RemoveCurrentSplit";
const char REMOVE_ALL_SPLITS[]     = "QtProject.RemoveAllSplits";
const char GOTO_PREV_SPLIT[]       = "QtProject.GoToPreviousSplit";
const char GOTO_NEXT_SPLIT[]       = "QtProject.GoToNextSplit";
const char CLOSE[]                 = "QtProject.Close";
const char CLOSE_ALTERNATIVE[]     = "QtProject.Close_Alternative"; // temporary, see QTCREATORBUG-72
const char CLOSEALL[]              = "QtProject.CloseAll";
const char CLOSEOTHERS[]           = "QtProject.CloseOthers";
const char CLOSEALLEXCEPTVISIBLE[] = "QtProject.CloseAllExceptVisible";
const char GOTONEXT[]              = "QtProject.GotoNext";
const char GOTOPREV[]              = "QtProject.GotoPrevious";
const char GOTONEXTINHISTORY[]     = "QtProject.GotoNextInHistory";
const char GOTOPREVINHISTORY[]     = "QtProject.GotoPreviousInHistory";
const char GO_BACK[]               = "QtProject.GoBack";
const char GO_FORWARD[]            = "QtProject.GoForward";
const char ABOUT_QTCREATOR[]       = "QtProject.AboutQtCreator";
const char ABOUT_PLUGINS[]         = "QtProject.AboutPlugins";
const char S_RETURNTOEDITOR[]      = "QtProject.ReturnToEditor";

// Default groups
const char G_DEFAULT_ONE[]         = "QtProject.Group.Default.One";
const char G_DEFAULT_TWO[]         = "QtProject.Group.Default.Two";
const char G_DEFAULT_THREE[]       = "QtProject.Group.Default.Three";

// Main menu bar groups
const char G_FILE[]                = "QtProject.Group.File";
const char G_EDIT[]                = "QtProject.Group.Edit";
const char G_VIEW[]                = "QtProject.Group.View";
const char G_TOOLS[]               = "QtProject.Group.Tools";
const char G_WINDOW[]              = "QtProject.Group.Window";
const char G_HELP[]                = "QtProject.Group.Help";

// File menu groups
const char G_FILE_NEW[]            = "QtProject.Group.File.New";
const char G_FILE_OPEN[]           = "QtProject.Group.File.Open";
const char G_FILE_PROJECT[]        = "QtProject.Group.File.Project";
const char G_FILE_SAVE[]           = "QtProject.Group.File.Save";
const char G_FILE_EXPORT[]         = "QtProject.Group.File.Export";
const char G_FILE_CLOSE[]          = "QtProject.Group.File.Close";
const char G_FILE_PRINT[]          = "QtProject.Group.File.Print";
const char G_FILE_OTHER[]          = "QtProject.Group.File.Other";

// Edit menu groups
const char G_EDIT_UNDOREDO[]       = "QtProject.Group.Edit.UndoRedo";
const char G_EDIT_COPYPASTE[]      = "QtProject.Group.Edit.CopyPaste";
const char G_EDIT_SELECTALL[]      = "QtProject.Group.Edit.SelectAll";
const char G_EDIT_ADVANCED[]       = "QtProject.Group.Edit.Advanced";

const char G_EDIT_FIND[]           = "QtProject.Group.Edit.Find";
const char G_EDIT_OTHER[]          = "QtProject.Group.Edit.Other";

// Advanced edit menu groups
const char G_EDIT_FORMAT[]         = "QtProject.Group.Edit.Format";
const char G_EDIT_COLLAPSING[]     = "QtProject.Group.Edit.Collapsing";
const char G_EDIT_TEXT[]           = "QtProject.Group.Edit.Text";
const char G_EDIT_BLOCKS[]         = "QtProject.Group.Edit.Blocks";
const char G_EDIT_FONT[]           = "QtProject.Group.Edit.Font";
const char G_EDIT_EDITOR[]         = "QtProject.Group.Edit.Editor";

const char G_TOOLS_OPTIONS[]       = "QtProject.Group.Tools.Options";

// Window menu groups
const char G_WINDOW_SIZE[]         = "QtProject.Group.Window.Size";
const char G_WINDOW_PANES[]        = "QtProject.Group.Window.Panes";
const char G_WINDOW_VIEWS[]        = "QtProject.Group.Window.Views";
const char G_WINDOW_SPLIT[]        = "QtProject.Group.Window.Split";
const char G_WINDOW_NAVIGATE[]     = "QtProject.Group.Window.Navigate";
const char G_WINDOW_LIST[]         = "QtProject.Group.Window.List";
const char G_WINDOW_OTHER[]        = "QtProject.Group.Window.Other";

// Help groups (global)
const char G_HELP_HELP[]           = "QtProject.Group.Help.Help";
const char G_HELP_SUPPORT[]        = "QtProject.Group.Help.Supprt";
const char G_HELP_ABOUT[]          = "QtProject.Group.Help.About";
const char G_HELP_UPDATES[]        = "QtProject.Group.Help.Updates";

const char WIZARD_CATEGORY_QT[] = "R.Qt";
const char WIZARD_TR_CATEGORY_QT[] = QT_TRANSLATE_NOOP("Core", "Qt");
const char WIZARD_KIND_UNKNOWN[] = "unknown";
const char WIZARD_KIND_PROJECT[] = "project";
const char WIZARD_KIND_FILE[] = "file";

const char SETTINGS_CATEGORY_CORE[] = "B.Core";
const char SETTINGS_ID_INTERFACE[] = "A.Interface";
const char SETTINGS_ID_SYSTEM[] = "B.Core.System";
const char SETTINGS_ID_SHORTCUTS[] = "C.Keyboard";
const char SETTINGS_ID_TOOLS[] = "D.ExternalTools";
const char SETTINGS_ID_MIMETYPES[] = "E.MimeTypes";

const char SETTINGS_DEFAULTTEXTENCODING[] = "General/DefaultFileEncoding";

const char SETTINGS_THEME[] = "Core/CreatorTheme";
const char DEFAULT_THEME[] = "flat";

const char TR_CLEAR_MENU[]         = QT_TRANSLATE_NOOP("Core", "Clear Menu");

const char DEFAULT_BUILD_DIRECTORY[] = "../%{JS: Util.asciify(\"build-%{CurrentProject:Name}-%{CurrentKit:FileSystemName}-%{CurrentBuild:Name}\")}";

const int MODEBAR_ICON_SIZE = 34;
const int MODEBAR_ICONSONLY_BUTTON_SIZE = MODEBAR_ICON_SIZE + 4;
const int DEFAULT_MAX_CHAR_COUNT = 10000000;

} // namespace Constants
} // namespace Core
