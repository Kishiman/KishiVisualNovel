# KishiVisualNovel: Summary and Roadmap

Source: the audit doc [KishiVisualNovel Plugin Audit & Proposed Architecture](https://claude.ai/artifact/Dpi71tF9rdyjpQGQTNEppg) (2026-11-01). That doc also has the full findings table and the architecture diagram.

## Summary

The XML script pipeline and the interface layer are in good shape. The C++ port is about half done, and the asset layer (`FRpyImage`) is the main thing blocking a clean showable abstraction.

- **Done in C++:** XML parser, instruction registry, `URpySession`, the five manager interfaces, `UKishiVNPlayerWidgetBase`, five manager widget Bases, `ARpyMainActorBase`, and the `IShowable` / `IAnimatable` widgets.
- **Still Blueprint-only in use:** VN2D uses the C++ player, main actor and statement manager. Audio, choice, scene and show still go through the Legacy `WGT_RPY*Manager` widgets, which derive directly from `UserWidget`.
- **Biggest risk:** `Execute()` returns `false` both for "wait for completion" and for failure. When a manager fails, the session waits for a completion that never comes.
- **Biggest design gap:** `FRpyImage` is a tagged union (sprite, layered sprite, video, level sequence), and every manager switches on its type, each supporting a different subset. Showables should be resolved through an asset-to-showable registry instead.
- **Conventions:** most interfaces follow the `Required` / `Hooks` rules. The exceptions: optional hooks are declared `= 0`, and a few calls bypass `Execute_*`.

### Key findings

| #   | Severity | Where                                                     | Finding                                                                                                                                      |
| --- | -------- | --------------------------------------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------- |
| 1   | High     | `URpySession::StartLabel`                                 | `script->labels[label]` asserts on a missing label; use `labels.Find`.                                                                       |
| 2   | High     | `URpySession::Run` + managers                             | `false` means both "wait" and "failed", so a failure stalls the script.                                                                      |
| 3   | High     | `UKishiRpyShowManagerWidgetBase::Show`                    | Returns `true` only for sprites, so video and level-sequence images stall the script.                                                        |
| 4   | High     | `RpyInstruction.h`                                        | `operator&` performs a bitwise OR; `operator!` returns `bool(value)` instead of its negation.                                                |
| 5   | Medium   | Session vs managers                                       | Show, scene and statement state is stored twice and can drift; `LoadFromState` replays `Show` / `Say` instead of calling `Set*ManagerState`. |
| 6   | Medium   | Manager interfaces                                        | Both `Get*ManagerState` and `GetCurrent*State` exist; the `_Implementation` constness doesn't match the UFUNCTION.                           |
| 7   | Medium   | Interpreter, BaseManager, MainActor, LayeredSpriteHandler | Optional hooks declared `= 0`.                                                                                                               |
| 8   | Medium   | Show manager Base, `ARpyMainActorBase`                    | Direct `Show_Implementation` / `CloseVN_Implementation` calls bypass Blueprint overrides.                                                    |
| 9   | Medium   | `IAnimatable`                                             | No defaults and no categories; `HandleAnimationEnded` is named like a handler but documented as a hook.                                      |
| 10  | Low      | `URpyComponent`                                           | `TSubclassOf<URpyScriptInterpreter>` names an interface class, which can't be instantiated.                                                  |
| 11  | Low      | `URpySession`                                             | `ClearScript` ignores its argument; `sceneState` is assigned twice; `try` / `catch` needs `bEnableExceptions`.                               |
| 12  | Low      | Naming                                                    | Lowercase getters, `const` setters on `mutable` fields, `Rpy` prefix left over after the switch to XML, `KishiLib` missing from the uplugin. |

### Target architecture

Each feature gets four pieces:

| Piece       | Example                          | Holds                                                                                                                                          |
| ----------- | -------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------- |
| Interface   | `IRpyShowManager`                | Required `Handle...` operations (the fallback logs an error and returns a safe default) and optional `On...` hooks (empty bodies, never `= 0`) |
| Library     | `URpyShowManagerLibrary`         | Static `*_Default` functions on `TScriptInterface<I...>` that hold the shared logic; dispatch only through `Execute_*`                         |
| Widget Base | `UKishiRpyShowManagerWidgetBase` | `_Implementation` overrides that call the Library, plus Blueprint hooks in its own category                                                    |
| Actor Base  | `AKishiRpyShowManagerActorBase`  | Same as the widget Base, for presentation in world space                                                                                       |

Other parts of the target:

- **Instruction results:** an `ERpyInstructionResult` enum (`Continue`, `Wait`, `Failed`) replaces the bool from `Execute()` and `bAutoExecuteNext`.
- **Showable assets:** `UKishiShowableAsset` with sprite, layered sprite, video, model and sequence subclasses. A `UKishiVisualNovelSettings` registry maps each asset class to a showable class. Managers resolve the class, create the showable, then call `IShowable::Execute_SetShowableAsset` and `Execute_Show`.
- **State:** `URpySession` becomes the only owner of state. Managers rebuild the view from it in `Set*ManagerState`.

## Roadmap

Port the VN2D managers right after the session fixes. That way steps 3 to 7 break only the VN2D widgets built on the C++ Bases, not the five Legacy widgets.

Rename any UFUNCTION, class or struct with a `[CoreRedirects]` entry in `Config/DefaultEngine.ini` so existing Blueprint nodes keep resolving.

| #   | Step                    | C++ change                                                                                                                                                                       | Blueprint rework                                                                                            | Other actions                                                                    |
| --- | ----------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------- | -------------------------------------------------------------------------------- |
| 1   | Stabilize the session   | `labels.Find`; fix `operator&` / `operator!`; `ClearScript` removes only its argument; drop `try` / `catch`; `InterpreterClass` becomes `TSubclassOf<UObject>` + `MustImplement` | None                                                                                                        | Test that a missing label logs an error instead of asserting                     |
| 2   | Port the VN2D managers  | Only hooks a Base is missing                                                                                                                                                     | Create VN2D Choice, Scene, Show and Audio managers from the C++ Bases; add them to `VN2D_RPYPlayer`         | Check that no level uses `WGT_VNPlayer` / `RpyActor`; quarantine `Styles/Legacy` |
| 3   | Instruction result enum | `ERpyInstructionResult` on `Execute()` and on every manager operation                                                                                                            | VN2D: none. Legacy (if kept): redo the return value in the five `WGT_RPY*Manager` widgets                   | Decide what `Failed` does                                                        |
| 4   | Tidy the interfaces     | Empty hook bodies; remove `GetCurrent*State`; fix `const`; add categories; use `Execute_*` at self-calls; PascalCase menu functions                                              | Delete `GetCurrent*State` overrides; menu nodes in `WGT_VNPlayer` / `WGT_RPYStatementManager`               | `FunctionRedirects`                                                              |
| 5   | Libraries               | Rename the interpreter library; add five manager libraries; move shared logic into `*_Default`                                                                                   | `WGT_VNPlayer` `*_Default` nodes                                                                            | `ClassRedirects`                                                                 |
| 6   | Showable assets         | Asset hierarchy, registry, `SetShowableAsset`, slimmer `FRpyImage` with a `PostLoad` conversion                                                                                  | Reparent `WGT_Sprite` / `WGT_LayeredSprite`; Legacy scene and show managers can't be kept                   | Create assets; fill the registry; resave scripts and layered sprites             |
| 7   | Unify show and scene    | Shared show Library; delete the `ShownWidgets` path; merge the option structs                                                                                                    | Re-wire struct nodes in `KishiWidget` / `WGT_LayeredSprite`; add a background canvas to `VN2D_SceneManager` | `StructRedirects`                                                                |
| 8   | Actor Bases             | Actor show and scene managers                                                                                                                                                    | Optional: reparent world showables                                                                          | Decide which managers need world space                                           |
| 9   | Docs                    | None                                                                                                                                                                             | None                                                                                                        | Point `CLAUDE.md` at `@AGENTS.md`; add the architecture rules                    |

## Checklist

Finish each step with the following:

- [ ] Build `JailBrainEditor`.
- [ ] Compile all Blueprints.
- [ ] Play one VN2D script from start to finish.
- [ ] Re-run the Node2Code export to check Blueprint usage.

### Open decisions

- [ ] Should `KishiLib` go back into the uplugin, or be deleted?
- [ ] Should the `Rpy` prefix stay now that scripts are XML?
- [ ] What should `Failed` do: advance and log, or stop the script? (Needed before step 3.)
- [ ] Should the asset-to-showable map live in Project Settings or on each asset class?
- [ ] Which managers need an actor Base (world space)? (Needed before step 8.)

### 1. Stabilize the session

- [x] `StartLabel`: replace `script->labels[label]` with `labels.Find(label)` and log an error when the label is missing (also fixed in `UJumpInstruction::GetNext`).
- [x] `RpyInstruction.h`: make `operator&` use `&`, and make `operator!` return the negation.
- [x] `ClearScript(script)`: remove only the script passed in.
- [x] `LoadFromState`: remove the duplicate `sceneState` assignment.
- [x] Remove `try` / `catch` from `Run`.
- [ ] Drop `bEnableExceptions` from Build.cs once the `try` / `catch` in `RpyScript.cpp`, `AudioPlayer.cpp` and the importer factory are gone (they still need it).
- [x] `URpyComponent`: change `InterpreterClass` to `TSubclassOf<UObject>` with `meta=(MustImplement="RpyScriptInterpreter")`.
- [x] Test: a missing label logs an error instead of asserting.
- [x] Check that `RpyActor`, `RpyFirstPersonCharacter` and `VNCamera` still start labels.

### 2. Port the VN2D managers (Blueprint)

- [ ] Create `VN2D_ChoiceManager` from `UKishiRpyChoiceManagerWidgetBase`.
- [ ] Create `VN2D_SceneManager` from `UKishiRpySceneManagerWidgetBase`.
- [ ] Create `VN2D_ShowManager` from `UKishiRpyShowManagerWidgetBase`.
- [ ] Create `VN2D_AudioManager` from `UKishiRpyAudioManagerWidgetBase`.
- [ ] Copy only layout and styling from the Legacy widgets, not graph logic.
- [ ] Add all four to `VN2D_RPYPlayer`, with one per interface.
- [ ] Add any hook a Base is missing in C++ (rebuild).
- [ ] Confirm that no level references `WGT_VNPlayer` or `RpyActor`.
- [ ] Move `Styles/Legacy` to a quarantine folder and fix up redirectors.

### 3. Instruction result enum

- [ ] Add `ERpyInstructionResult { Continue, Wait, Failed }`.
- [ ] Make `URpyInstruction::Execute()` return it, and store it on the instruction in place of `bAutoExecuteNext`.
- [ ] Make `Scene`, `Show`, `Hide`, `Say`, `Menu`, `PlayAudio`, `QueueAudio`, `StopAudio` and `PauseAudio` return it on the interfaces and Bases.
- [ ] Update `URpySession::Run` to advance on `Continue`, wait on `Wait`, and log and apply the chosen policy on `Failed`.
- [ ] Show manager Base: return `Failed` for unsupported types instead of stalling (finding 3).
- [ ] Blueprint: refresh the VN2D managers and fix any return pins.
- [ ] Blueprint (only if Legacy is kept): redo the return values in the five `WGT_RPY*Manager` widgets.

### 4. Tidy the interfaces

- [ ] Give the hooks on `IRpyScriptInterpreter`, `IRpyScriptInterpreterBaseManager`, `IRpyMainActor` and `ILayeredSpriteHandler` empty bodies instead of `= 0`.
- [ ] Remove `GetCurrent*State`, keeping one `Get*ManagerState` / `Set*ManagerState` pair per manager.
- [ ] Make `_Implementation` constness match each UFUNCTION.
- [ ] `IAnimatable`: add `Required` / `Hooks` categories and defaults, and rename `HandleAnimationEnded` to `OnAnimationEnded` if it is a hook.
- [ ] `ILayeredSpriteHandler`: add categories.
- [ ] Use `IRpyShowManager::Execute_Show` in `SetShowManagerState`, and `Execute_CloseVN` in `StartRpyScript`.
- [ ] Rename `getSaveMenuVisible` and similar functions to PascalCase, and add `FunctionRedirects`.
- [ ] Blueprint: delete the `GetCurrent*State` overrides (Legacy, if kept).
- [ ] Blueprint: check the menu nodes in `WGT_VNPlayer` and `WGT_RPYStatementManager`.

### 5. Libraries

- [ ] Rename `URpyScriptInterpreterImplementation` to `URpyScriptInterpreterLibrary` and add a `ClassRedirects` entry.
- [ ] Add the five `U*ManagerLibrary` classes (replace the commented-out stubs).
- [ ] Move shared Base logic into the `*_Default` functions; Bases call the Library.
- [ ] Blueprint: check that the `IHandleInstructionComplete_Default` and `IOnDialogueViseme_Default` nodes in `WGT_VNPlayer` still resolve.

### 6. Showable assets

- [ ] Add `UKishiShowableAsset : UPrimaryDataAsset`.
- [ ] Move `ULayeredSprite` and `UVideoMediaSprite` under it.
- [ ] Add `UKishiSpriteAsset`, `UKishiModelAsset` and `UKishiSequenceAsset`.
- [ ] Add a `UKishiVisualNovelSettings` registry that maps asset class to widget and actor showable classes and resolves up the class hierarchy.
- [ ] Add `IShowable::SetShowableAsset` (required), plus `OnShown` / `OnHidden` hooks.
- [ ] Reduce `FRpyImage` to `Tag` + `TSoftObjectPtr<UKishiShowableAsset>`, and convert from `ERpyImageType` in `PostLoad`.
- [ ] Show manager `Show`: resolve, create, then `Execute_SetShowableAsset` and `Execute_Show`.
- [ ] Blueprint: reparent `WGT_Sprite` and `WGT_LayeredSprite`, or implement `SetShowableAsset`.
- [ ] Blueprint: retire the Legacy scene and show managers.
- [ ] Create the sprite, model and sequence assets.
- [ ] Fill the registry in Project Settings.
- [ ] Resave every `URpyScript` and `ULayeredSprite` asset.

### 7. Unify show and scene

- [ ] Have the scene manager call the show Library on a background layer.
- [ ] Delete the `ShownWidgets` path and keep `TrackedShowables`.
- [ ] Merge `FRpySceneOptions` into `FKishiShowableOptions`, with `StructRedirects` if needed.
- [ ] Stop the scene manager from calling `AddToViewport` on its own sprite.
- [ ] Blueprint: re-wire the break/make struct nodes in `KishiWidget` and `WGT_LayeredSprite`.
- [ ] Blueprint: add a background canvas to `VN2D_SceneManager`.

### 8. Actor Bases

- [ ] Add actor show and scene manager Bases that use the same Libraries.
- [ ] Blueprint (optional): reparent `LayeredSpriteInteractableActor` and other world showables.

### 9. Docs

- [ ] Reduce `CLAUDE.md` to `@AGENTS.md` plus anything specific to Claude.
- [ ] Add a module map, the script pipeline, the Interface + Library + Base pattern, how to add an XML command, the instruction result contract, the showable registry rule, and the VN2D vs Legacy status to `AGENTS.md`.
- [ ] Add the plugin-specific rules (Interface + Library + Base, result contract, showable registry) to `Plugins/KishiVisualNovel/CLAUDE.md` too, since it already holds the plugin's source layout and runtime rules; keep only project-wide rules at the root.
- [ ] Update `Scripts/xml/XML_FORMAT.md` and the fixtures under `Scripts/xml/tests` if step 6 changes how scripts reference images.
- [ ] Trim the Node2Code panel walkthrough in `AGENTS.md`.
- [ ] Commit `AGENTS.md` and `CLAUDE.md`.
