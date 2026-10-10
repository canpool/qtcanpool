# A module that cannot run (not built)

This directory is a **sample, not a target**. `src/modules/CMakeLists.txt` does
not add it, and it must stay that way.

`broken.json.in` declares

```json
"Dependencies" : [ { "Name" : "nonexistent", "Version" : "..." } ]
```

so the plugin manager can resolve nothing for it. That is the point: it is the
module to look at when you want to see what a failure looks like.

Build it by adding `add_subdirectory(broken)` next to the real modules and
looking at the plugin manager dialog, or by running the samples with it in the
plugin directory:

- the dialog lists it as **Missing dependency**, with the reason string the
  manager produced, and the host keeps starting;
- `QxPluginManager::hasError()` becomes true and `errorString()` carries the
  line `<id>: plugin '<id>' is missing required dependency 'nonexistent'`;
- `initialize()` is never called - the manager sets the module aside before it
  instantiates anything.

Keeping it out of the build matters: it would otherwise fail on every run of
the samples, and a demo that always reports an error teaches the wrong thing.
