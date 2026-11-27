# KishiVisualNovel Plugin Guide

## Plugin overview

KishiVisualNovel is an Unreal Engine 4.27 plugin for dialogue and visual-novel scripting. It contains runtime script parsing and execution, Unreal-facing interfaces and components, and an editor importer and script viewer.

## Source layout

- `Source/KishiVisualNovel`: runtime visual-novel assets, XML parsing, instruction execution, sessions, save-state integration, and runtime interfaces. Files are grouped by feature, not by kind: a feature's interface, library, widget Base and component sit in the same folder under `Public/`, and `Private/` mirrors it.
  - `Script/`: script asset, instructions, props and script types; `Private/Script/Parsing` and `Private/Script/Instructions` hold the XML parser and one header per instruction.
  - `Session/`: `URpySession`, manager state types, `IRpyStateful*`.
  - `Player/`: `IRpyScriptInterpreter`, the base manager interface, the VN player and manager widget Bases.
  - `Managers/<Statement|Choice|Scene|Show|Audio>/`: one folder per script manager. New script-driven mechanics (a dialogue log, evidence, skill checks) get their own folder here, named for the mechanic, not for a game or style.
  - `Save/`, `Speakers/`, `Showables/`, `World/` (VN host and anchor components), `Interaction/`, `Animation/`, `Assets/` (shared asset base classes), `Settings/` (`UKishiVisualNovelSettings`, including the asset search folders).
  - `Deprecated/Actors/`: the old Actor Bases, kept unchanged for compatibility.
  - Moving a header changes only `#include` paths; Blueprints reference classes by module and name, so no redirect is needed. Renaming a class, struct or UFUNCTION needs a `[CoreRedirects]` entry in `Config/DefaultKishiVisualNovel.ini`, so the redirect ships with the plugin; a rename that only changes case needs none, since FName lookups ignore case.
- `Source/KishiVisualNovelImporter`: editor-only importers and asset editors. Keep editor dependencies and Slate editor widgets in this module.
- `Source/Kishi`: shared runtime helpers, including string and tagged-text utilities.
- `Scripts/xml`: current XML scripts; `tests/fixtures` holds per-feature regression fixtures, `tests/samples` full demos, `tests/fixtures/invalid` scripts that must fail to import.
- `Scripts/rpy`: legacy Ren'Py fixtures and migration inputs. XML is the runtime script format.
- `Tools`: script migration and related developer tools.

## Script format and runtime architecture

- Write scripts as `<visualNovel version="1">` XML. Commands are elements nested under labels, menus, choices, and conditionals. Read `Scripts/XML_FORMAT.md` before changing the format or parser.
- Dialogue supports ordered mixed content: real style tags are child elements with matching closing tags, while literal angle brackets must be encoded as XML entities.
- `<raw>` is unsupported. Do not add raw-command examples or claim that raw instructions can execute.
- Required instruction values belong in explicit required fields. Optional instruction values belong in `FRpyInstructionProps` and use its typed accessors.
- Instructions and the manager operations they call return `ERpyInstructionResult`: `Continue` when done, `Wait` when the manager reports completion later through the interpreter, `Failed` when they cannot run. Log the reason before returning `Failed`; the session then applies `FailedInstructionPolicy` from `UKishiVisualNovelSettings` (default: log and advance). Never return `Wait` for a failure, since nothing would complete it and the script would stall. An operation that returns `Wait` keeps the instruction `URpyScriptInterpreterBaseManagerLibrary::GetExecutingInstruction` returns and later passes it to `ReportInstructionComplete`, so a completion inside a `<together>` block counts for the right command and a late one is ignored. A `show` or `hide` with a transition returns `Wait` until the transition ends; a click outside dialogue reaches every manager's `OnSkipRequested`, which ends its transitions. A missing required implementation returns `Failed`.
- The XML parser and `UStringUtils` must preserve the difference between real rich-text tags and escaped literal text. Keep substring/typewriter output escaped so literal `&`, `<`, and `>` cannot become Rich Text markup.
- Runtime `GetNext(session)` selects the next instruction for the current execution state. Static graph analysis should use `GetPossibleNextInstructions()` and preserve all possible edges for branches, menus, jumps, calls, and returns.
- Instruction ownership and references must follow Unreal's UObject/GC rules. Avoid unmanaged `new` allocations for objects that need to live with an asset or appear in the editor; keep reflected UObject references in `UPROPERTY` fields when GC must retain them.

## Unreal Engine conventions

- This plugin targets UE 4.27 and its supported compiler/toolchain. Check engine headers for the exact API and include path before using newer Unreal APIs.
- Keep reflected types compatible with UnrealHeaderTool. A `USTRUCT` cannot contain `UFUNCTION`; put Blueprint functions in a `UBlueprintFunctionLibrary`. Each reflected header must include its own `.generated.h` as the final include.
- Use `UPROPERTY` for UObject references that need to be retained by the garbage collector. Do not manually `delete` UObjects.
- For Blueprint event functions declared on a `UINTERFACE`, dispatch through `IInterface::Execute_Function(UObject*, ...)`. Never call the event on an object/interface pointer or call another object's `_Implementation` directly; that bypasses Blueprint dispatch and can assert. Implementers should define or override `_Implementation` as the event body, while call sites use `Execute_*`.
- Never declare an interface event's `_Implementation` `= 0`, and declare one for every event so UHT does not generate a silent default. Hooks get an empty body; required operations get a fallback that logs an error and returns a safe default.
- Put a feature's shared logic in its `U*Library` (`URpyShowManagerLibrary` and so on, declared after the interface): `*_Default` functions are the default bodies of interface operations, take the implementer as `Target` and dispatch through `Execute_*`. Keep the Library free of a Base's storage and widgets, so widget Bases, components and Blueprint implementers can all call it; the Base keeps its state and hooks and calls the Library for the rest.
- An image (`FRpyImage`) is any asset. Resolve the showable that presents it through the showable registry in Project Settings (`URpyShowManagerLibrary::ResolveShowableClass`), and hand it the asset with `IShowable::Execute_SetShowableAsset`; do not switch on asset types in a manager. To support a new kind of asset, implement `SetShowableAsset` on a showable and add a registry row, and derive new plugin asset types from `UKishiShowableAsset`.
- Present widget showables through a `UKishiShowableLayer` (`Managers/Show`): it tracks showables by tag on one canvas, creates them, places and fits them (`BottomCenter` for characters, `Fill` for backgrounds), plays their transitions, and owns the waits and skip for them. The show and scene manager Bases each own one and keep only their script state, class resolution and hooks; a new presentation manager should own one too rather than copy that code.
- Treat `*Base` classes as optional proposed implementations of their interfaces. When behavior depends on an interface, check `UClass::ImplementsInterface` and dispatch through `IInterface::Execute_*`; do not cast to a plugin Base class. Put shared configuration operations, such as assigning an interpreter to a manager, on the relevant interface so direct implementers work too.
- Put world-side VN logic in components, in the matching feature folder (`World`, `Interaction`, `Showables`), so it attaches to any actor (Actor, Pawn, Character). The classes in `Public/Deprecated/Actors` are kept unchanged and get no new features; since Legacy was deleted no asset uses them, and they are due to be deprecated (see the open decisions in `ROADMAP.md`). Find a VN host or anchor on an actor through the lookup libraries (`URpyMainComponentLibrary`, `UPlayerAnchorLibrary`, `URpyCameraAnchorLibrary`), which check the actor first and then its components. See step 10 of `ROADMAP.md`.
- Do not give a component interface a UFUNCTION named like an existing `UActorComponent` / `USceneComponent` UFUNCTION (for example `IsActive`): `Execute_*` would find the engine function instead of the interface event.
- Keep editor-only classes and module dependencies in `KishiVisualNovelImporter`; runtime code must not depend on editor modules.
- Keep behavior-preserving changes localized. When making an intentional breaking format or API change, update the XML documentation and relevant fixtures in the same change.

## Build verification

After every code change, build the project using the UE 4.27 editor target. Run this command from the UE 4.27 installation directory (the folder containing `Engine`):

```powershell
Engine\Build\BatchFiles\Build.bat JailBrainEditor Win64 Development D:\Projects\Unreal\JailBrain\JailBrain.uproject -waitmutex
```

Wait for the build process to finish. Inspect its complete output and exit code for compiler errors, UnrealHeaderTool errors, and warnings that indicate a failed build. If the build fails, fix the reported issues and rerun the same command; report any remaining errors instead of claiming success. For documentation-only edits, a build is not required unless the edit changes code snippets that are compiled as part of the project.

## Validation fixtures

- Add XML regression fixtures under `Scripts/xml/tests/fixtures`: one feature per file, opening with a comment that names the feature and states the expected result, and named `<area>_<feature>.xml` with no number. Full playable demos go in `Scripts/xml/tests/samples`. Read `Scripts/xml/tests/README.md` before adding either.
- Scripts that must fail to import belong in `Scripts/xml/tests/fixtures/invalid`, listed in that folder's README with their expected error. Behaviour that only logs and still imports does not belong there; document it in the suite README instead. Note any prerequisites for project-specific custom instructions.
- When a code change affects parsing, instruction execution, serialization, or the editor, run the build above and inspect all output. Also exercise the relevant XML fixture in the editor when practical.
