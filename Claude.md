# KishiVisualNovel Plugin Guide

## Plugin overview

KishiVisualNovel is an Unreal Engine 4.27 plugin for dialogue and visual-novel scripting. It contains runtime script parsing and execution, Unreal-facing interfaces and components, and an editor importer and script viewer.

## Source layout

- `Source/KishiVisualNovel`: runtime visual-novel assets, XML parsing, instruction execution, sessions, save-state integration, and runtime interfaces.
- `Source/KishiVisualNovelImporter`: editor-only importers and asset editors. Keep editor dependencies and Slate editor widgets in this module.
- `Source/Kishi`: shared runtime helpers, including string and tagged-text utilities.
- `Scripts/xml`: current XML scripts and test fixtures.
- `Scripts/rpy`: legacy Ren'Py fixtures and migration inputs. XML is the runtime script format.
- `Tools`: script migration and related developer tools.

## Script format and runtime architecture

- Write scripts as `<visualNovel version="1">` XML. Commands are elements nested under labels, menus, choices, and conditionals. Read `Scripts/xml/XML_FORMAT.md` before changing the format or parser.
- Dialogue supports ordered mixed content: real style tags are child elements with matching closing tags, while literal angle brackets must be encoded as XML entities.
- `<raw>` is unsupported. Do not add raw-command examples or claim that raw instructions can execute.
- Required instruction values belong in explicit required fields. Optional instruction values belong in `FRpyInstructionProps` and use its typed accessors.
- The XML parser and `UStringUtils` must preserve the difference between real rich-text tags and escaped literal text. Keep substring/typewriter output escaped so literal `&`, `<`, and `>` cannot become Rich Text markup.
- Runtime `GetNext(session)` selects the next instruction for the current execution state. Static graph analysis should use `GetPossibleNextInstructions()` and preserve all possible edges for branches, menus, jumps, calls, and returns.
- Instruction ownership and references must follow Unreal's UObject/GC rules. Avoid unmanaged `new` allocations for objects that need to live with an asset or appear in the editor; keep reflected UObject references in `UPROPERTY` fields when GC must retain them.

## Unreal Engine conventions

- This plugin targets UE 4.27 and its supported compiler/toolchain. Check engine headers for the exact API and include path before using newer Unreal APIs.
- Keep reflected types compatible with UnrealHeaderTool. A `USTRUCT` cannot contain `UFUNCTION`; put Blueprint functions in a `UBlueprintFunctionLibrary`. Each reflected header must include its own `.generated.h` as the final include.
- Use `UPROPERTY` for UObject references that need to be retained by the garbage collector. Do not manually `delete` UObjects.
- For Blueprint event functions declared on a `UINTERFACE`, dispatch through `IInterface::Execute_Function(UObject*, ...)`. Never call the event on an object/interface pointer or call another object's `_Implementation` directly; that bypasses Blueprint dispatch and can assert. Implementers should define or override `_Implementation` as the event body, while call sites use `Execute_*`.
- Treat `*Base` classes as optional proposed implementations of their interfaces. When behavior depends on an interface, check `UClass::ImplementsInterface` and dispatch through `IInterface::Execute_*`; do not cast to a plugin Base class. Put shared configuration operations, such as assigning an interpreter to a manager, on the relevant interface so direct implementers work too.
- Keep editor-only classes and module dependencies in `KishiVisualNovelImporter`; runtime code must not depend on editor modules.
- Keep behavior-preserving changes localized. When making an intentional breaking format or API change, update the XML documentation and relevant fixtures in the same change.

## Build verification

After every code change, build the project using the UE 4.27 editor target. Run this command from the UE 4.27 installation directory (the folder containing `Engine`):

```powershell
Engine\Build\BatchFiles\Build.bat JailBrainEditor Win64 Development D:\Projects\Unreal\JailBrain\JailBrain.uproject -waitmutex
```

Wait for the build process to finish. Inspect its complete output and exit code for compiler errors, UnrealHeaderTool errors, and warnings that indicate a failed build. If the build fails, fix the reported issues and rerun the same command; report any remaining errors instead of claiming success. For documentation-only edits, a build is not required unless the edit changes code snippets that are compiled as part of the project.

## Validation fixtures

- Add XML regression fixtures under `Scripts/xml/tests` and document any prerequisites for project-specific custom instructions.
- Keep intentionally invalid parser/schema fixtures clearly marked and separate from scripts expected to import successfully.
- When a code change affects parsing, instruction execution, serialization, or the editor, run the build above and inspect all output. Also exercise the relevant XML fixture in the editor when practical.
