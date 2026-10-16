# shell

The smallest `QxAppShell` that is still a complete application: one call to
`createStandardShell()` gives you the Home page, the Layout group (Save and
Restore, wired to `saveLayout()`/`restoreLayout()` and the status bar), the
"Ready" line and the layout persisted on close. The window only adds the groups
that are its own.

Shows the 4.5 convenience added by D1 (`addLayoutGroup()`) and D2
(`createStandardShell()`).
