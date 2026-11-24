# KishiVisualNovel: Summary and Roadmap

Source: the audit doc [KishiVisualNovel Plugin Audit & Proposed Architecture](https://claude.ai/artifact/Dpi71tF9rdyjpQGQTNEppg) (2026-11-01). That doc also has the full findings table and the architecture diagram.

## Summary

The XML script pipeline and the interface layer are in good shape. Since the Legacy content was deleted (2026-11-22), every feature follows Interface → C++ Base → Blueprint or C++ child. The asset layer (`FRpyImage`) is the main thing blocking a clean showable abstraction.

- **Done in C++:** XML parser, instruction registry, `URpySession`, the five manager interfaces and their widget Bases, `UKishiVNPlayerWidgetBase`, the save menu and save slot (`IRpySaveMenu` / `IRpySaveSlot` and their widget Bases), `ISpeaker` with `UKishiCharacterAsset`, the VN and interaction components, and the `IShowable` / `IAnimatable` widgets.
- **Legacy removed (2026-11-22):** `Styles/Legacy` and the old Blueprints, widgets and maps (`demo1`, `demo_danganronpa`, `test_widget`, `VNCamera`, `RpyActor`, `RpyFirstPersonCharacter` and their redirectors) are deleted. VN2D, VN3D and FPS run on the C++ Bases and components. A scan of the binary assets finds no asset that uses the Actor Bases in `Public/Deprecated/Actors` any more.
- **Execution results (fixed in step 3, 2026-11-22):** `Execute()` and the manager operations return `Continue`, `Wait` or `Failed`, and a failure follows the Failed Instruction Policy (default: log and advance) instead of stalling the script.
- **Biggest risk now:** show, scene and statement state is stored twice, in the session and in the managers, and can drift (finding 5, step 7 and the State target below).
- **Showable assets (step 6, 2026-11-22):** `FRpyImage` holds a reference to any asset, and the show manager resolves its showable through the registry in Project Settings, then calls `IShowable::SetShowableAsset`. The scene manager still chooses by asset class itself until step 7.
- **Libraries (step 5, 2026-11-22):** the interpreter and each manager have a `U*Library` with the logic that does not depend on a Base's storage or widgets; the widget Bases call it, and the coming components can too.
- **Conventions (step 4, 2026-11-22):** every interface follows the `Required` / `Hooks` rules. No interface event is declared `= 0`, required operations log an error and return a safe default, and self-calls go through `Execute_*`.

### Key findings

| #   | Severity | Where                                                     | Finding                                                                                                                                               |
| --- | -------- | --------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------- |
| 1   | High     | `URpySession::StartLabel`                                 | `script->labels[label]` asserts on a missing label; use `labels.Find`. Fixed in step 1.                                                               |
| 2   | High     | `URpySession::Run` + managers                             | `false` means both "wait" and "failed", so a failure stalls the script. Fixed in step 3.                                                              |
| 3   | High     | `UKishiRpyShowManagerWidgetBase::Show`                    | Returns `true` only for sprites, so video and level-sequence images stall the script. Fixed in step 3.                                                |
| 4   | High     | `RpyInstruction.h`                                        | `operator&` performs a bitwise OR; `operator!` returns `bool(value)` instead of its negation. Fixed in step 1.                                        |
| 5   | Medium   | Session vs managers                                       | Show, scene and statement state is stored twice and can drift; `LoadFromState` replays `Show` / `Say` instead of calling `Set*ManagerState`.          |
| 6   | Medium   | Manager interfaces                                        | Both `Get*ManagerState` and `GetCurrent*State` exist; the `_Implementation` constness doesn't match the UFUNCTION. Fixed in step 4.                   |
| 7   | Medium   | Interpreter, BaseManager, MainActor, LayeredSpriteHandler | Interface events declared `= 0` (also in `IRpyCameraAnchor`): optional hooks, and required ops that should log and return a default. Fixed in step 4. |
| 8   | Medium   | Show manager Base, `ARpyMainActorBase`                    | Direct `Show_Implementation` / `CloseVN_Implementation` calls bypass Blueprint overrides. Fixed in step 4.                                            |
| 9   | Medium   | `IAnimatable`                                             | No defaults and no categories; `HandleAnimationEnded` is named like a handler but documented as a hook. Fixed in step 4.                              |
| 10  | Low      | `URpyComponent`                                           | `TSubclassOf<URpyScriptInterpreter>` names an interface class, which can't be instantiated.                                                           |
| 11  | Low      | `URpySession`                                             | `ClearScript` ignores its argument; `sceneState` is assigned twice; `try` / `catch` needs `bEnableExceptions`.                                        |
| 12  | Low      | Naming                                                    | Lowercase getters, `const` setters on `mutable` fields, `Rpy` prefix left over after the switch to XML, `KishiLib` missing from the uplugin.          |

### Target architecture

Each feature gets four pieces:

| Piece       | Example                          | Holds                                                                                                                                          |
| ----------- | -------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------- |
| Interface   | `IRpyShowManager`                | Required `Handle...` operations (the fallback logs an error and returns a safe default) and optional `On...` hooks (empty bodies, never `= 0`) |
| Library     | `URpyShowManagerLibrary`         | Static `*_Default` functions on `TScriptInterface<I...>` that hold the shared logic; dispatch only through `Execute_*`                         |
| Widget Base | `UKishiRpyShowManagerWidgetBase` | `_Implementation` overrides that call the Library, plus Blueprint hooks in its own category                                                    |
| Component   | `UKishiRpyShowManagerComponent`  | Same as the widget Base, for presentation in world space; attaches to any actor (step 10)                                                      |

Other parts of the target:

- **Instruction results:** an `ERpyInstructionResult` enum (`Continue`, `Wait`, `Failed`) replaces the bool from `Execute()` and `bAutoExecuteNext`.
- **Showable assets:** an image is any asset. A `UKishiVisualNovelSettings` registry maps each asset class to a widget and a world showable class; the plugin's own asset types derive from `UKishiShowableAsset`. Managers resolve the class, create the showable, then call `IShowable::Execute_SetShowableAsset` and `Execute_Show`.
- **Speakers:** `ISpeaker`, implemented by an asset or an actor, replaces the inline `FRpyCharacter`. `UKishiCharacterAsset` owns the display name, the showable tag, one `DT_RichTextStyle` row for the name and another for the statement, and the voice lines. `<say>` resolves the speaker, plays its voice line on the `voice` channel, and hands the style to the statement manager (step 12).
- **State:** `URpySession` becomes the only owner of state. Managers rebuild the view from it in `Set*ManagerState`.

## Roadmap

Steps 1 to 6, 10 and the C++ of 11 are done (steps 3 to 6 still need a Blueprint compile in the editor). Legacy was deleted on 2026-11-22, so the remaining steps only touch C++ and the VN2D, VN3D and FPS Blueprints built on the Bases, with no Legacy Blueprint rework. Step 7 comes next. Step 13's content moves can run alongside it, since they touch only assets and settings.

Rename any UFUNCTION, class or struct with a `[CoreRedirects]` entry so existing Blueprint nodes keep resolving. Put plugin redirects in `Plugins/KishiVisualNovel/Config/DefaultKishiVisualNovel.ini` (created in step 5), so they ship with the plugin; engine plugins such as Paper2D do the same in their own ini. A rename that only changes case needs no redirect, since FName lookups ignore case.

| #   | Step                    | C++ change                                                                                                                                                                       | Blueprint rework                                                                      | Other actions                                                     |
| --- | ----------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------- | ----------------------------------------------------------------- |
| 1   | Stabilize the session   | `labels.Find`; fix `operator&` / `operator!`; `ClearScript` removes only its argument; drop `try` / `catch`; `InterpreterClass` becomes `TSubclassOf<UObject>` + `MustImplement` | None                                                                                  | Test that a missing label logs an error instead of asserting      |
| 2   | Port the VN2D managers  | Only hooks a Base is missing                                                                                                                                                     | Done: VN2D Choice, Scene, Show, Audio, Save menu and Save slot from the C++ Bases     | Done: Legacy deleted (2026-11-22)                                 |
| 3   | Instruction result enum | Done: `ERpyInstructionResult` on `Execute()` and on every manager operation; `FailedInstructionPolicy` setting                                                                   | None: no Blueprint overrides or calls a manager operation (export, 2026-11-22)        | Done: `Failed` logs and advances by default                       |
| 4   | Tidy the interfaces     | Done: no `= 0` on interface events; `GetCurrent*State` removed; `const` fixed; categories; `Execute_*` at self-calls; PascalCase menu functions                                  | None: no Blueprint uses the removed or renamed functions (export, 2026-11-22)         | None: the menu renames change only case, so no redirects          |
| 5   | Libraries               | Done: `URpyScriptInterpreterLibrary`; five manager libraries; Bases call the shared logic; `ReportInstructionComplete`                                                           | None: no Blueprint calls the renamed or moved functions (export, 2026-11-22)          | Done: redirects in `Config/DefaultKishiVisualNovel.ini`           |
| 6   | Showable assets         | Done: any asset is an image; `UKishiShowableAsset`; registry in Project Settings; `SetShowableAsset`; `FRpyImage` = asset + tag, converted in `PostSerialize`                    | None: `WGT_Sprite` is not a show manager showable                                     | Optional: resave scripts and layered sprites                      |
| 7   | Unify show and scene    | Shared show Library; delete the `ShownWidgets` path; merge the option structs                                                                                                    | Re-wire struct nodes in `KishiWidget`; add a background canvas to `VN2D_SceneManager` | `StructRedirects`                                                 |
| 8   | World-space managers    | Show and scene manager **components** (see step 10), not Actor Bases                                                                                                             | Optional: reparent world showables                                                    | Decide which managers need world space                            |
| 9   | Docs                    | None                                                                                                                                                                             | None                                                                                  | Point `CLAUDE.md` at `@AGENTS.md`; add the architecture rules     |
| 10  | Components over actors  | Component counterpart of every class in `Public/Deprecated/Actors`; `IRpyMainComponent`; actor-or-component lookup libraries                                                     | Done: the VN styles and interaction actors use the components                         | Deprecate `Public/Deprecated/Actors`, which no asset uses         |
| 11  | Interaction             | Interaction interfaces and handler components; `URpyFirstPersonComponent`                                                                                                        | Finish the script chain in `Blueprints/InteractableActors`                            | Re-enter placed values in `DetectiveOffice_Layout` and `FPS_demo` |
| 12  | Speakers                | `ISpeaker` and `UKishiCharacterAsset` (done); `FRpySpeaker`, `SayAs`, voice playback                                                                                             | None until the statement manager applies the styles                                   | Fixtures, `XML_FORMAT.md`, `DA_John`                              |
| 13  | Layout and styles       | Configurable search folders and feature folders (done); `UKishiVNStyleAsset`; `URpyComponent` takes a style                                                                      | Move Content into `Core`, `Styles`, `Demo`, `Tests`, `ThirdParty`; `_Template` style  | Fix up redirectors; re-import fixtures; rerun the reference scan  |

## Checklist

Finish each step with the following:

- [ ] Build `JailBrainEditor`.
- [ ] Compile all Blueprints.
- [ ] Play one VN2D script from start to finish.
- [ ] Re-run the Node2Code export to check Blueprint usage. It runs headless (the VS Code task, or `UE4Editor-Cmd.exe` with the same arguments) while the editor is closed or open; it crashes in `OculusEditor` on exit (code 3) after writing every file, which is harmless.

Found by the export (2026-11-22):

- [ ] `VN2D_RPYMainActor`'s default script is `/KishiVisualNovel/Scripts/Tests/00_hello_world`, which was deleted when the scripts moved to `Scripts/Fixtures` and `Scripts/Samples`. Point it at a sample script, or the VN2D demo starts with no script.
- `DetectiveStart/FirstPerson/Blueprints/FirstPersonCharacter` doesn't compile (`CallActionOnFocusedActor` no longer exists); already listed in step 11.
- The `image_types` fixture names a `stairs` level sequence that is not in the content; see step 6.

### Open decisions

- [ ] Should `KishiLib` go back into the uplugin, or be deleted?
- [ ] Should the `Rpy` prefix stay now that scripts are XML?
- [x] What should `Failed` do? A project setting, `FailedInstructionPolicy`, defaulting to log and advance (2026-11-22, step 3).
- [x] Should the asset-to-showable map live in Project Settings or on each asset class? Project Settings, with per-manager overrides, since raw engine asset classes such as Paper2D sprites cannot carry it (2026-11-22, step 6).
- [ ] Deprecate the classes in `Public/Deprecated/Actors` (`UCLASS(Deprecated)`) or keep them? No asset uses them since Legacy was deleted (binary scan, 2026-11-22; confirm in the Reference Viewer first). (Step 10.)
- [ ] Which managers need a world-space component? (Needed before step 8.)

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

Decisions (2026-11-01): VN2D uses the native showable widgets, and the show fixes live in C++. Legacy was first kept in place; it was deleted on 2026-11-22.

- [x] Add what the Bases are missing in C++ (built 2026-11-01):
  - Show manager `Show` / `Hide` go through `IShowable` when the widget implements it, mapping `FRpySceneOptions` to `FKishiShowableOptions` (attributes, transition type, duration). Widgets without `IShowable` keep the `PlayTransition` path.
  - Showing the same tag with the same layered sprite reuses the widget, so expressions switch in place. A position given with `MOVE` / `EASE` animates from the old position.
  - New optional hook `ILayeredSpriteHandler::OnSpeakerViseme`; `UKishiLayeredSpriteWidgetBase` forwards it to `UpdateVisemeLayer`. The show manager calls it on the sprite whose tag matches the speaker.
  - The `SetSprite` / `SetFileMediaSource` reflection helpers also accept `UKishiSpriteWidget`'s parameter names (`InSprite`, `InMediaSource`, `ScalingMethod`). Before, native sprite widgets got no image in the show and scene managers.
- [x] Base review fixes (built 2026-11-01):
  - Show manager: one path. `Show` calls `ResolveShowableClass` → `CreateShowableFromClass` → `ApplyImageToShowable` → `IShowable::Show`, and every image is tracked in `TrackedShowables` (`ShownWidgets` and the `PlayTransition` fallback are gone, which pulls the step 7 item forward). `ResolveShowableClass` and `ApplyImageToShowable` are overridable hooks with defaults. The C++-implemented helpers moved from `|Required` to `|Showables`. `SpriteWidgetClass` / `LayeredSpriteWidgetClass` must now implement `IShowable`. A widget position shifts the widget instead of shrinking it.
  - Missing setup now logs an error instead of failing silently: no `ShowCanvas`, no widget class, an asset that can't be applied, no `ChoicesPanel` / `ChoiceEntryClass`, no `SceneSprite`, an unknown audio channel.
  - No more stalls on failure (stopgap until step 3): `show` / `scene` with an undefined image, a scene or audio manager that can't play, and `hide` of an unknown tag all log and continue. `scene _` clears the scene.
  - Statement manager `Say` returns `false`, so the script waits for the player. Before, it returned `true` for any non-empty line, and the session ran straight past dialogue.
  - Choice manager: clicks now reach `Choose`. They were bound to `Choose_Implementation`, which isn't a `UFUNCTION`, so the dynamic delegate couldn't bind.
  - `Set*ManagerState` in the choice, scene, show and audio Bases dispatch through `Execute_*`.
  - The audio blip plays directly instead of going through `PlayAudio`, so it's no longer saved in the audio state.
  - `UKishiSpriteWidget` draws its image at brush size from the top-left instead of stretching it. `UKishiShowableWidget::Show` uses `SelfHitTestInvisible`, so shown images don't block clicks.
  - The Bases warn once per class (editor only) when a Blueprint child overrides an interface event they implement in C++.
- [x] Fix `VN2D_RPYShowManager` (a reparented copy of the Legacy show manager):
  - Rename `SpritePanel` to `ShowCanvas`.
  - Delete the `OnDialogueViseme` event and the Legacy graphs (`FindSpriteByTag`, `SetUpSprite`, `Show Sprite`, `ShowLayeredSprite`, `RemoveWidgetFromParent`).
  - Set `SpriteWidgetClass` and `LayeredSpriteWidgetClass`.

  Recreating it from the Base is simpler.

- [x] Fix `VN2D_RPYPlayer`: delete the empty `StartUp` and `OnDialogueViseme` events. They replace the C++ implementations, so managers aren't re-discovered on start and visemes never reach the managers. Use `OnInterpreterStarted` / `OnViseme` instead.
- [x] Create `VN2D_ChoiceEntry` from `UKishiChoiceEntryWidgetBase` (replaces `IndexedButton`).
- [x] Create `VN2D_ChoiceManager` from `UKishiRpyChoiceManagerWidgetBase`: add a `ChoicesPanel` panel and set `ChoiceEntryClass` to `VN2D_ChoiceEntry`.
- [x] Create `VN2D_SceneManager` from `UKishiRpySceneManagerWidgetBase`: add a `SceneSprite` widget (a `UKishiSpriteWidget` subclass), or set `SceneSpriteClass`.
- [x] Create `VN2D_ShowManager` from `UKishiRpyShowManagerWidgetBase`: add a `ShowCanvas` canvas; set `SpriteWidgetClass` to `KishiSpriteWidget` and `LayeredSpriteWidgetClass` to `KishiLayeredSpriteWidget` (or VN2D subclasses of them). Set `NeutralVisemeLayer` on the layered sprite class if lip sync is wanted.
- [x] Create `VN2D_AudioManager` from `UKishiRpyAudioManagerWidgetBase`: copy `AudioChannels` and `Blip` from `WGT_RPYAudioManager`.
- [x] Copy only layout and styling from the Legacy widgets, not graph logic.
- [x] In `VN2D_RPYPlayer`, replace the four `WGT_RPY*Manager` children with the VN2D ones, one per interface.
- [x] Test a script that uses `show` with attributes, a `with` transition and dialogue lip sync.
- [x] Add `IRpySaveMenu` / `IRpySaveSlot` with `UKishiSaveMenuWidgetBase` / `UKishiSaveSlotWidgetBase`, and create `VN2D_SaveMenu` / `VN2D_SaveSlot` from them (they replace `WGT_SaveMenu` / `WGT_SaveSlot`).
- [x] Take the game off Legacy: `RpyFirstPersonGameMode` (the project default, used by `DetectiveOffice_Layout`) and `FirstPersonGameMode` now spawn `Styles/FPS/FPS_RpyMainActor`, which has the VN Main, Interacting and Interactable components.
- [x] Delete Legacy (2026-11-22) instead of quarantining it: `Styles/Legacy`, the redirectors and old Blueprints in `Blueprints/`, `Widgets/WGT_*` except `WGT_Sprite`, and the levels `demo1`, `demo_danganronpa` and `test_widget`. No remaining asset references a deleted package (binary scan).

The 2026-11-01 findings (which assets used Legacy, and the gaps between the Bases and the Legacy managers) are closed by the deletion; see the git history of this file for them. The scene manager's world-space path (2026-11-07) remains: for `<scene transition="..." layer="...">`, the Base attaches the active camera (the view target) to the layer's anchor with Keep Relative and plays the transition sequence, binding the sequence's `Camera`-tagged binding to that camera. It also works with `scene _`. Anchors are found by `IRpyCameraAnchor` (actor or component) first, then by actor tag (`AnchorClass`).

### 3. Instruction result enum

Decision (2026-11-22): `Failed` follows `FailedInstructionPolicy` in `UKishiVisualNovelSettings` (Project Settings > Plugins > Kishi Visual Novel, category Execution). The default is **Log and Advance**; **Log and Stop** ends the script and the interpreter gets `OnScriptComplete`, as at a script's end.

C++ (built 2026-11-22):

- [x] Add `ERpyInstructionResult { Continue, Wait, Failed }` in `Script/RpyScriptTypes.h`.
- [x] `URpyInstruction::Execute()` returns it, and the session stores it in `LastResult`, which replaces `bAutoExecuteNext`. The interpreter's completion handler advances only when `LastResult` is `Wait`.
- [x] `Scene`, `Show`, `Hide`, `Say`, `Menu`, `PlayAudio`, `QueueAudio`, `StopAudio` and `PauseAudio` return it on the interfaces and Bases. A missing required implementation now returns `Failed`; before, it returned `false`, which stalled the script.
- [x] `URpySession::Run` advances on `Continue`, stops on `Wait`, and on `Failed` logs and applies the policy.
- [x] Show manager Base: `Failed` when there is no showable class for the image type (finding 3), the showable can't be created, or the image can't be applied. The scene manager Base returns `Failed` when it can't display the image; the audio Base when there is no player, channel or sound.
- [x] The step 2 stopgaps are now `Failed`: `show` / `scene` with an undefined image, a missing interpreter or manager. `hide` of an unknown tag stays `Continue` with a warning.
- [x] Fixes found on the way: every instruction checks for its manager before calling it (`RpyFindManager`), so a missing manager logs instead of hitting `Execute_*` on null. `play` / `queue` of an undefined audio name logs instead of asserting in `TMap::operator[]`. An unsupported command returns `Failed` instead of stalling. `if` on an unknown bool variable logs an error instead of `Fatal` (which crashed the game) and takes the else branch. A failed menu advances to the instruction after the menu instead of ending the script.
- [x] The choice manager Base returns `Wait`, or `Failed` for a menu with no choices; the statement manager Base returns `Wait`.

Blueprint (in the editor):

- [x] `VN2D_RPYSceneManager` and `VN2D_RPYShowManager` don't override `Scene` / `Show`: each has only an EventGraph (Node2Code export, 2026-11-22). The names the binary scan found were widget names, not overrides.
- [x] No Blueprint calls a manager operation, so no Boolean return pin was wired (same export).
- [ ] Compile all Blueprints in the editor.

Tests:

- [ ] A script with an undefined image in `show` and in `scene`, an undefined sound in `play`, and a menu: with Log and Advance it logs and plays to the end; with Log and Stop it ends at the first failure.
- [ ] Remove the show manager from `VN2D_RPYPlayer` in a test copy: `show` logs and the script continues.

### 4. Tidy the interfaces

Decision (2026-11-22): `HandleAnimationEnded` keeps its name. It is the implementer's required handler: the implementer calls it on itself when an animation finishes or is skipped, and the Bases broadcast their `OnAnimationEnded` delegate from it, which is the listener hook. Renaming the function to `OnAnimationEnded` would also clash with that delegate. Its docs and category now say so.

C++ (built 2026-11-22):

- [x] No interface event is declared `= 0`. Hooks have empty bodies under `<Interface>|Hooks`: `OnDialogueViseme` / `OnScriptComplete` on `IRpyScriptInterpreter`, the same plus `OnInstructionComplete` on `IRpyScriptInterpreterBaseManager`, `IRpyCloseVNEventListener::OnCloseVNEvent`, and `IPlayerAnchor::OnEnterAnchor` / `OnExitAnchor`.
- [x] Required operations log an error and return a safe default, under `<Interface>|Required` with a tooltip: `IRpyMainActor`, `ILayeredSpriteHandler`, `IRpyCameraAnchor`, and also `IRpySaveGame`, `IRpyStateful`, `IAnimatable` and `IPlayerAnchor`, which the audit missed. The last two declared no `_Implementation`, so UHT's generated defaults failed silently. `IPlayerAnchor::GetViewLimits` falls back to no limits, so a missing implementation never locks the view.
- [x] Removed `GetCurrent*State` from the five manager interfaces and their Bases; one `Get*ManagerState` / `Set*ManagerState` pair per manager remains. They were the only UFUNCTIONs whose `_Implementation` constness differed from the declaration.
- [x] `IAnimatable` and `ILayeredSpriteHandler` have `Required` / `Hooks` categories. `HandleAnimationEnded` is now `BlueprintCallable`, so a Blueprint implementer can call it.
- [x] Self-calls go through `Execute_*`: `IsAnimating` / `Skip` in `UKishiAnimationComponent` and `UKishiShowableWidget`, `GetLayerDisplay` in `UKishiLayeredSpriteWidgetBase`, `CloseVN` in `ARpyMainActorBase::StartRpyScript`. `SetShowManagerState` has used `Execute_Show` since step 2. An override calling its parent's `_Implementation` (the layered sprite Base calling `UKishiShowableWidget::Show_Implementation`) is a Super call and stays.
- [x] `getSaveMenuVisible` / `setSaveMenuVisible` and the load, config and backlog pairs are now `GetSaveMenuVisible` / `SetSaveMenuVisible` and so on. The setters are no longer `const`, so the VN player Base drops `mutable` and `const_cast` (part of finding 12). No `FunctionRedirects`: the rename only changes case, and FName lookups ignore case.

Blueprint:

- [x] No Blueprint uses `GetCurrent*State` or the menu functions, and none overrides a function whose signature changed (Node2Code export, 2026-11-22). `FPS_RpyMainActor` implements `IRpyMainActor` and `IRpyCloseVNEventListener` in Blueprint; their signatures are unchanged.
- [ ] Compile all Blueprints in the editor.

Tests:

- [x] In a VN2D script, open and close the save and load menus from the statement manager's keys and from the save menu's Cancel; save and load a slot.
- [x] Change a layered sprite's expression with a whole-image swap; the swap completes and the script continues.
- [x] Anchor to a `PlayerAnchorComponent`: the view limits still apply.

### 5. Libraries

Scope (2026-11-22): the Libraries hold the logic that does not depend on how a manager stores its state or draws itself, so a component manager (step 8) or a Blueprint manager can reuse it. Each Base keeps its own state struct, widgets and Blueprint hooks, and calls the Library for the rest. `*_Default` functions are the default bodies of interface operations, take the manager as `Target` and dispatch through `Execute_*`; the other functions are helpers.

C++ (built 2026-11-22):

- [x] Renamed `URpyScriptInterpreterImplementation` to `URpyScriptInterpreterLibrary`, now exported with `KISHIVISUALNOVEL_API`, and its functions to `HandleInstructionComplete_Default` / `OnDialogueViseme_Default`. Both now loop over the managers and check `ImplementsInterface` before dispatching. A `ClassRedirects` entry and `FunctionRedirects` entries are in `Config/DefaultKishiVisualNovel.ini`.
- [x] Added the five `U*ManagerLibrary` classes in place of the commented-out stubs, in the interface headers, with their code in `Private/Managers/<Feature>/Rpy<Feature>Manager.cpp`:
  - Audio: `ResolveAudio`, `PlayAudio_Default`, `QueueAudio_Default`, `StopAudio_Default`, `PauseAudio_Default`, `SetAudioManagerState_Default`, `PlayDialogueBlip`, and C++ helpers that record played and stopped channels in an `FRpyAudioManagerState`. The manager passes its `UAudioPlayer`.
  - Choice: `SetChoiceManagerState_Default`, `Choose_Default` (sends the index to the session).
  - Scene: `SetSceneManagerState_Default`, `FindSceneCamera`, `FindSceneAnchor`, `AttachCameraToAnchor`, `PlayCameraSequence`, `StopCameraSequence`, `GetTransitionSequence`, `FindSequenceCameraBinding`. None touch widgets.
  - Show: `SetShowManagerState_Default`, `MakeShowableOptions` (moved from the Base, with a `FunctionRedirects` entry), `ApplyImageToShowable` (replaced by `ResolveShowableClass` and `IShowable::SetShowableAsset` in step 6), `CanReuseShowable`, `ForwardSpeakerViseme`.
  - Statement: `GetSpeakerDisplayName`, `ToggleSaveMenu`, `ToggleLoadMenu`, `QuickSave`, `QuickLoad`, and a C++ `EmitDialogueViseme`.
- [x] `URpyScriptInterpreterBaseManagerLibrary::ReportInstructionComplete` calls `HandleInstructionComplete` on a manager's interpreter, so any manager that returned `Wait` can report completion. The statement Base uses it.
- [x] The Bases call the Libraries. They keep what is tied to their storage or widgets: state structs, panels and canvases, showable tracking and placement, the scene sprite, and which camera is anchored and where it came from. Without an interpreter, `Choose` and line completion now log an error where they did nothing before.
- [ ] Left for later: showable tracking, placement and transitions move to the show Library in step 7 (with the scene manager), and remembering the anchored camera moves when step 8 adds the scene component. The session setup that `ARpyMainActorBase` duplicates waits on the deprecation decision in step 10.

Blueprint:

- [x] No Blueprint calls the renamed or moved functions (Node2Code export, 2026-11-22), so the redirects are a safety net.
- [x] Compile all Blueprints in the editor.

Tests:

- [x] Play a VN2D script with a menu, music and a sound, a `show` with a transition, a scene change, and a line with a speaker: choices advance, audio plays and stops, sprites animate, the speaker's name shows and the blip plays.
- [x] Save and load mid-script: audio, show, scene and statement state come back.
- [x] Use the statement manager's save, load, quick save and quick load buttons.

### 6. Showable assets

Decision (2026-11-22): any asset can be an image. `FRpyImage` holds a soft reference to any `UObject`, and the registry in Project Settings maps asset classes to showable classes, so a Paper2D sprite or a level sequence needs no wrapper asset. `UKishiShowableAsset` is the base for the plugin's own showable assets only. This replaces the planned `UKishiSpriteAsset`, `UKishiModelAsset` and `UKishiSequenceAsset`; a model asset can come later as a `UKishiShowableAsset` subclass, or as a raw mesh with a registry row.

C++ (built 2026-11-22):

- [x] Added `UKishiShowableAsset` (abstract). It derives from `UKishiDataAssetBase` rather than `UPrimaryDataAsset`, so its subclasses keep their import data.
- [x] `ULayeredSprite` and `UVideoMediaSprite` derive from it. Existing assets load unchanged (6 layered sprites, 12 video media sprites).
- [x] Added `ShowableClasses` to `UKishiVisualNovelSettings` (Project Settings > Plugins > Kishi Visual Novel > Showables): rows of asset class, widget showable class and world showable class. Resolution walks up the asset's class hierarchy, so the most specific row wins. The C++ defaults reproduce what VN2D used: Paper2D sprite and video media sprite → `KishiSpriteWidget`, layered sprite → `KishiLayeredSpriteWidget`.
- [x] Added `IShowable::SetShowableAsset(Asset, TargetSize)` (required, returns whether the showable accepted the asset). `UKishiSpriteWidget` takes Paper2D sprites and video media sprites; `UKishiLayeredSpriteWidgetBase` takes layered sprites. `UKishiAnimationComponent` keeps the logging fallback until world showables exist (step 8).
- [x] Dropped the planned `OnShown` / `OnHidden` hooks: a showable already gets `Show` / `Hide`, and completion is reported through `IAnimatable::HandleAnimationEnded` and the Bases' `OnAnimationEnded` delegate.
- [x] `FRpyImage` is now `asset` (`TSoftObjectPtr<UObject>`), `name` and `tag`. The old fields keep their names, hidden, and `PostSerialize` moves them into `asset` when old scripts or saves load, then clears them. No asset means "nothing", as in `scene _`. `URpyScript` keeps the loaded assets resident (`LoadedImageAssets`), as the old hard references did.
- [x] Show manager: `Show` resolves the showable class from the asset's class (`ShowableClassOverrides` on the Base, then the registry, through `URpyShowManagerLibrary::ResolveShowableClass`), creates it, then calls `Execute_SetShowableAsset` and `Execute_Show`. `SpriteWidgetClass` and `LayeredSpriteWidgetClass` are gone (VN2D set them to the registry's defaults); the `ApplyImageToShowable` hook is replaced by `SetShowableAsset`.
- [x] Scene manager: chooses by asset class (level sequence, Paper2D sprite, video media sprite) instead of `ERpyImageType`. It still drives `SceneSprite` by reflection until step 7 moves it onto the show Library.

Checked headless (2026-11-22): all 28 `URpyScript` assets load; every image has an asset, and every image that is not a level sequence resolves to a widget showable.

Blueprint and assets:

- [x] `WGT_Sprite` needs no change: the show manager no longer uses it (VN2D creates `KishiSpriteWidget`), and the scene manager still sets it by reflection.
- [x] No sprite, model or sequence wrapper assets to create, and the registry's defaults are in C++.
- [ ] Compile all Blueprints in the editor.
- [ ] Optional: resave the `URpyScript` and `ULayeredSprite` assets so the legacy fields are dropped from disk. They load correctly without it.
- [ ] `Scripts/xml/tests/fixtures/image_types.xml` names a `stairs` level sequence that is not in the content ("Could Not Found Default Image : stairs"). This was already the case before step 6.

Tests:

- [ ] Play `show_layered_attributes`, `show_transitions`, `scene_transitions` and `scene_level_sequence`: sprites, layered sprites and level sequences display as before, and a layered sprite's attribute change still swaps in place.
- [ ] Load a save made before step 6: the shown images and the scene come back.
- [ ] Add a row to `ShowableClassOverrides` on a copy of `VN2D_RPYShowManager` that maps Paper Sprite to another widget showable: that widget is created.

### 7. Unify show and scene

- [ ] Have the scene manager call the show Library on a background layer.
- [x] Delete the `ShownWidgets` path and keep `TrackedShowables` (done in step 2).
- [ ] Merge `FRpySceneOptions` into `FKishiShowableOptions`, with `StructRedirects` if needed.
- [ ] Stop the scene manager from calling `AddToViewport` on its own sprite.
- [ ] Blueprint: re-wire the break/make struct nodes in `KishiWidget`.
- [ ] Blueprint: add a background canvas to `VN2D_SceneManager`.

### 8. World-space managers

- [ ] Add show and scene manager components (not Actor Bases, see step 10) that use the same Libraries.
- [ ] Blueprint (optional): reparent `LayeredSpriteInteractableActor` and other world showables.

### 9. Docs

- [ ] Reduce `CLAUDE.md` to `@AGENTS.md` plus anything specific to Claude.
- [ ] Add a module map, the script pipeline, the Interface + Library + Base pattern, how to add an XML command, the instruction result contract, the showable registry rule, and the VN2D, VN3D and FPS styles to `AGENTS.md`.
- [ ] Add the plugin-specific rules (Interface + Library + Base, result contract, showable registry) to `Plugins/KishiVisualNovel/CLAUDE.md` too, since it already holds the plugin's source layout and runtime rules; keep only project-wide rules at the root.
- [x] Point `CLAUDE.md` and `AGENTS.md` at `Scripts/XML_FORMAT.md` (they named `Scripts/xml/XML_FORMAT.md`, which does not exist).
- [ ] Update `Scripts/XML_FORMAT.md` and the fixtures under `Scripts/xml/tests` if step 6 changes how scripts reference images.
- [ ] Trim the Node2Code panel walkthrough in `AGENTS.md`.
- [ ] Commit `AGENTS.md` and `CLAUDE.md`.

### 10. Components over actors

**Why:** `Maps/demo_danganronpa` ran its VN from `Blueprints/VNCamera`, which derives from `Pawn` (both deleted with Legacy on 2026-11-22). The native VN logic lives in Actor Bases (`ARpyMainActorBase` and the others), so using it would force `VNCamera` to change its parent class. Components can be added to any actor, whether it is an `Actor`, a `Pawn` or a `Character`.

**Rule:** new VN logic goes in the feature folders under `Source/KishiVisualNovel/Public` (`World`, `Interaction`, `Showables`). The classes in `Public/Deprecated/Actors` stay, unchanged, and get no new features. No asset uses them since Legacy was deleted; see the open decision on deprecating them.

| Actor class (kept)             | Component                                                 | Interface                  | Find it on an actor                                                                  |
| ------------------------------ | --------------------------------------------------------- | -------------------------- | ------------------------------------------------------------------------------------ |
| `ARpyMainActorBase`            | `URpyComponent` ("VN Main Component")                     | `IRpyMainComponent` (new)  | `URpyMainComponentLibrary::FindVNHost`, `StartRpyScriptOnActor`, `CloseVNOnActor`    |
| `ARpyCloseVNEventListenerBase` | `URpyCloseVNEventListenerComponent` ("VN Close Listener") | `IRpyCloseVNEventListener` | `RegisterWithVNHost`, or `AutoRegisterVNHost` on BeginPlay                           |
| `APlayerAnchorActorBase`       | `UPlayerAnchorComponent` (scene component)                | `IPlayerAnchor`            | `UPlayerAnchorLibrary::FindPlayerAnchor`                                             |
| `ARpyCameraAnchorActorBase`    | `URpyCameraAnchorComponent` (scene component)             | `IRpyCameraAnchor`         | `URpyCameraAnchorLibrary::FindAnchorByLayerName` (components match on `AnchorLayer`) |

Notes:

- `IRpyMainComponent` has the same operations as `IRpyMainActor`, except that `IsActive` is named `IsVNActive`. A component can't implement `IRpyMainActor`, because `UActorComponent::IsActive` is already a UFUNCTION: `Execute_IsActive` would find it and return the component's activation state. `IRpyMainComponent` derives from `IRpyStatefulComponent`, so the save game stores the session with the owner's components.
- The lookup libraries check the actor first, then its components. Callers should go through them (or `Execute_*` on what they return), not cast to a component class.

C++ (built 2026-11-07):

- [x] Add `IRpyMainComponent` and `URpyMainComponentLibrary` (`World/RpyMainComponent.h`).
- [x] Rewrite `URpyComponent` as the counterpart of `ARpyMainActorBase`: it has `VNPlayerClass`, `SessionClass`, `Interpreter`, close listeners plus an `OnVNClosed` delegate, `OnScriptComplete`, and save and load of the session. It uses the pawn's controller when its owner is a possessed pawn. Two new options: `bCloseOnScriptComplete`, and `bSetUIInputMode`, which restores Game input on close. The rewrite fixes these bugs in the old component:
  - It created the interpreter with `NewObject`, which can't create widgets.
  - A second `StartScript` reused the old session and ignored the new script.
  - `AddDynamic` ran on every start.
  - The interpreter never got `SetSession` / `StartUp`.
  - `CloseScript` kept the session.
  - The state was named `RpyMainActorState`.

  `InterpreterClass`, `StartScript` and `CloseScript` are removed; no Blueprint used them.

- [x] Add `UPlayerAnchorComponent`. The anchor transform is relative to the component. It uses `OnAnchorEntered` / `OnAnchorExited` delegates instead of BlueprintImplementableEvents, and it tracks who is anchored (`bSingleOccupant`).
- [x] Add `URpyCameraAnchorComponent`. Its camera is the first camera below it, then the owner's first camera; `SetCameraComponent` overrides that.
- [x] Add `URpyCloseVNEventListenerComponent`, which broadcasts `OnVNClosed`.
- [x] `FindAnchorByLayerName` also matches components. Add `UPlayerAnchorLibrary::FindPlayerAnchor`. Make `UpdateAnchoredCameraAndCursor` static: it was an instance function on a function library, so Blueprint couldn't call it.
- [x] Scene manager Base: `FindSceneAnchor` returns a `USceneComponent`. It checks `IRpyCameraAnchor` first, then actors with an `AnchorClass` tag, and the camera attaches to that component.

Blueprint and content:

- [x] `VNCamera`, `RpyActor` and `RpyFirstPersonCharacter`: deleted with Legacy. `VN2D_RPYMainActor`, `VN3D_RPYMainActor` and `FPS_RpyMainActor` use the VN Main Component.
- [ ] `RpyScriptInteractableActor`: replace the `StartRpyScript` call on the `RpyMainActor` interface with `Start Rpy Script On Actor`, so it works with actors and components. `URpyScriptInteractionComponent` does this; see step 11.
- [x] `PlayerAnchorActor`: uses a Player Anchor Component (step 11).
- [ ] `VN3D_Demo` (optional): replace the `Anchor` actors with VN Camera Anchor Components whose `AnchorLayer` is the old tag. Tagged actors still work through `AnchorClass`.
- [ ] Step 8: build the world-space show and scene managers as components.

Tests:

- [ ] `VN3D_Demo` (replaces `demo_danganronpa`): start a script through `VN3D_RPYMainActor`'s component. Play `<scene asset="_" transition="intro" layer="student3" />`, then save, load and close mid-script, and check that a VN Close Listener Component fires.
- [x] Point `Scripts/xml/tests/fixtures/scene_level_sequence.xml` at `Styles/VN3D/VN3D_Demo` as its host level (it named `Maps/demo_danganronpa`).
- [ ] Put a VN Main Component on a plain `Actor`, a `Pawn` and a `Character`, and run one script on each.

Follow-up:

- [ ] `ARpyMainActorBase` and `URpyComponent` duplicate their session setup. Deprecating the Actor classes (no asset uses them) removes the duplicate; otherwise move it into a main Library. Step 5 left it, pending that decision.

### 11. Interaction and first-person migration

**Goal:** move the interaction system to Interface + native component + Blueprint, and take the first-person game off `Legacy/RpyFirstPersonCharacter` (now deleted) without reimplementing the FPS basics. Following step 10, all of it is components, so designers attach it to existing actors and never have to inherit from a plugin class.

An actor has one `UInteractableComponent` and any number of handler components, for example "talk" + "shop" or "sit" + "move". At BeginPlay each handler finds or adds the Interactable and registers its action there. The Interactable broadcasts its delegates and runs only the handler registered for the requested action; that handler also gets `HandleInteractionEnd`, while focus and nearby hooks go to all handlers. It also shows the prompt widget at its own location while focused, when `InteractionWidgetClass` is set. Actors that only bind the delegates, or implement `IInteractable` without registering, keep working, so the Blueprints can be migrated one at a time. `EndInteraction` resets `State` to `Interactable` itself.

| Blueprint (`Blueprints/InteractableActors`) | Add these components                                              | Native behavior                                                           |
| ------------------------------------------- | ----------------------------------------------------------------- | ------------------------------------------------------------------------- |
| `InteractableActor`                         | None: set `InteractionWidgetClass` on its Interactable            | Prompt widget that follows the Interactable while focused                 |
| `InteractableLightActor`                    | `ULightInteractionComponent`                                      | Its action toggles the owner's first light and ends the interaction       |
| `RpyScriptInteractableActor`                | `URpyScriptInteractionComponent`                                  | Starts the script on the interacting actor's VN host; ends when it closes |
| `LayeredSpriteInteractableActor`            | `ULayeredSpriteWidgetComponent`, `URpyScriptInteractionComponent` | World-space sprite that faces the owner's camera                          |
| `PlayerAnchorActor`                         | `UPlayerAnchorInteractionComponent`, `UPlayerAnchorComponent`     | Camera view until the `Return` action, with occupancy through the anchor  |

- A handler adds an Interactable component when the actor has none (`FindOrAddInteractable`). Add one yourself to set `InteractionWidgetClass`, `InteractableName` or the prompt's position.
- `UInteractionHandlerComponentBase` is the native starting point for new handlers, which implement `HandleInteractionStart`. `HandledActionName` (default `action`) is the input action that runs the handler, and `HandledActionLabel` is the prompt text. Each handler on an actor needs its own action name; a duplicate logs an error and is ignored.
- `Actions` on the Interactable is the list on offer: authored entries plus one per handler. A handler with the same name as an authored entry takes that entry, and an empty `HandledActionLabel` keeps its label. An action not on the list is ignored. An empty list accepts any action.
- A second action needs its own input: add a mapping in the input settings and add the name to `InteractActionNames` on `URpyFirstPersonComponent`.
- `UInteractionCameraHandlerComponentBase` adds the view change and `BlendTime`. The view moves only when the owner has a Camera component, and a Camera Anchor component on the owner adds edge turning.
- The script and anchor handlers do not need the owner to be a `StaticMeshActor`, or any particular class.

`URpyFirstPersonComponent` ("VN First Person Component") derives from `URpyComponent` and replaces the VN side of the deleted `RpyFirstPersonCharacter`:

| `RpyFirstPersonCharacter`                                     | `URpyFirstPersonComponent`                                  |
| ------------------------------------------------------------- | ----------------------------------------------------------- |
| `Interacting` component                                       | Finds the owner's, or creates one                           |
| `InputAction action`                                          | Binds `InteractActionNames` on the pawn's input component   |
| `startScript`, started at `start` on BeginPlay                | `StartScript` and `StartLabel`, once the player has control |
| `CursorWidgetClass`, `SetMouseCursorWidget`, `CenterOnScreen` | `CursorWidgetClass`; recentered on close                    |
| `Session`, `VNPlayer`, `SaveToState`, `LoadFromState`         | Inherited from `URpyComponent`                              |
| `SwitchInputModeToUI`, `SwitchInputModeToDefault`             | `VNInputMode` while a VN runs; Gameplay afterwards          |
| `CloseVN` ends the current interaction                        | Same                                                        |
| Movement, look, jump, gun, VR                                 | Not included; the owner keeps its own                       |

C++ (built 2026-11-08):

- [x] Add `IInteractable`, `IInteractionPromptWidget` and `UInteractableLibrary` (`Interaction/Interactable.h`).
- [x] Add the components above and `URpyFirstPersonComponent`.
- [x] Fix `UInteractingComponent`: an interaction that ended during `Interact` (the light) never released its flag, so scanning stopped; a refused `Interact` also kept the flag; `HandleInteractionEnded` used the focused interactable, which may have changed or be null. `CallEndInteractionOnInteractable` now ends the running interactions, not whatever is focused.
- [x] Merge `UInteractionPromptComponent` into `UInteractableComponent`. Move `HandledActionName` to `UInteractionHandlerComponentBase` and add `HandledActionLabel`. Handlers register their action with `RegisterAction`, so an actor can have several.

Blueprint and content. Back up or commit first. Work from child to parent, because a child's graph can call functions in its parent, and do each Blueprint's graph steps together, since the old graph would otherwise run alongside the native code.

Done:

- [x] `WGT_interaction`: add the `InteractionPromptWidget` interface. Implement `Handle Interaction Prompt Setup`: set `Tag` from `DisplayName`, then call `SetInteractableActions`.
- [x] `InteractableLightActor`: reparented to `StaticMeshActor`, with its own Interactable and Light Interaction Components.
- [x] `PlayerAnchorActor`: reparented to `StaticMeshActor`, with its own Interactable, Player Anchor Interaction and Player Anchor Components.

After the prompt merge:

- [ ] `InteractableLightActor` and `PlayerAnchorActor`: the Interaction Prompt Component class no longer exists. Commit or back up the assets before opening them. Set `InteractionWidgetClass = WGT_interaction` on the Interactable, remove any leftover prompt entry, compile and save.
- [ ] Open and save `DetectiveOffice_Layout` and `FPS_demo`, whose placed actors still reference the removed class.

The script chain, in this order. Do not reparent it: `RpyScriptInteractableActor` and `LayeredSpriteInteractableActor` keep their parent, so placed actors keep their inherited `Interactable`, `Camera` and `CameraAnchor` values, and they get the prompt once, from `InteractableActor`:

- [ ] Record the placed values first. In `DetectiveOffice_Layout` and `FPS_demo`, select the script and sprite actors, press Ctrl+C and paste into a text file. Non-default values such as `InteractionScript`, `InteractionScriptLabel`, `Layered Sprite` and `Name` appear as text. Deleting the Blueprint variables loses them.
- [ ] `LayeredSpriteInteractableActor`: note the size and transform of `LayeredSpriteWidget`, then replace it with a Layered Sprite Widget Component, and re-enter them. Its `Widget Class` defaults to the native `KishiLayeredSpriteWidget` (`WGT_LayeredSprite` was deleted). Set its `Layered Sprite` to the old variable's default (`Augustina`). Delete the construction script graph, the `WidgetLookAtCamera` macro, the `Layered Sprite` variable and the empty `ActorBeginOverlap` event.
- [ ] `RpyScriptInteractableActor`: add a VN Script Interaction Component with `InteractionScript = 00_hello_world` and `InteractionScriptLabel = start`, the old defaults; `BlendTime` already defaults to 0.5. Delete the whole event graph, including the `SetUpWidget` call and the nearby debug print. Delete `handleInteractionStart`, `handleInteractionEnd`, the `InteractionEnd` dispatcher and the three moved variables.
- [ ] `InteractableActor`: on the Interactable, set `InteractionWidgetClass = WGT_interaction` and set `InteractableName` from `Name`. Delete the event graph, `SetUpWidget`, `UpdateWidget`, `TickInteractableActor` and the `InteractionWidget` and `Name` variables. Compile all three Blueprints.
- [ ] Re-enter the recorded values on placed actors, on the components in each actor's Details panel. Press Play and run the script tests below with the `FPS_RpyMainActor` player.

The first-person player. This was done differently from the original plan, which put a VN First Person Component on `FirstPersonCharacter`:

- [x] `RpyFirstPersonGameMode` (the project default, used by `DetectiveOffice_Layout`) and `FirstPersonGameMode` spawn `Styles/FPS/FPS_RpyMainActor`. It has a VN Main Component (`URpyComponent`), an Interacting component and an Interactable component.
- [x] `Legacy/RpyFirstPersonCharacter`, its redirectors under `Blueprints` and the placed `VNFirstPersonCharacter_4` are gone; no remaining asset references them.
- [ ] `FPS_RpyMainActor` uses `URpyComponent`, not `URpyFirstPersonComponent`. Check that it binds the interact input, starts its script and switches input mode, or swap in the VN First Person Component, which does all three.
- [ ] `Content/DetectiveStart/FirstPerson/Blueprints/FirstPersonCharacter` is no longer spawned. Delete it, or give it a VN First Person Component if it should host scripts again.

Tests:

- [ ] Add a handler component to a plain `Actor` and a `StaticMeshActor` that are not interaction Blueprints, and interact with each. The actor needs collision that blocks the Visibility channel.
- [ ] Check placed instances in `DetectiveOffice_Layout` and `Styles/FPS/FPS_demo` after re-entering their values.
- [ ] Two handlers on one actor, with two input mappings on the first-person component: the prompt lists both labels with their keys, each key runs its own handler, and only that handler gets the end call. Two handlers with the same action name log an error.
- [ ] Light: toggle it twice, then focus something else. The second toggle and the refocus failed before the `UInteractingComponent` fix.
- [ ] Script interactable, with and without a Camera on the owner: start, finish and close. The view returns, the player can move, and the actor can be used again. Repeat three times, because `SetIgnoreMoveInput` is a counter.
- [ ] Player anchor, with and without a Camera Anchor: enter, leave with `Return`, re-enter. With a Player Anchor Component, a second occupant is refused.
- [ ] Layered sprite: the sprite shows in a level loaded in PIE and faces the camera.
- [ ] Save and load mid-script on the first-person player (`FPS_RpyMainActor`).
- [ ] Possess a pawn after BeginPlay and check that `action` still works.

### 12. Speakers

**Goal:** a statement names an asset instead of an inline `<character>` definition. `DA_John` — a `UKishiCharacterAsset`, or any actor or component that implements `ISpeaker` — owns the display name, the showable tag, the rich-text styles and the voice lines, so the script carries only the reference. Unlike showables (step 6), a speaker has nothing to instantiate, so it needs no asset-to-class registry: the asset implements the interface itself.

Rich text reuses the existing style-set format. A speaker points at rows of an `FRichTextStyleRow` table such as `Content/Fonts/DT_RichTextStyle`, the same table the statement manager already resolves `<red>` and `<blue>` against. The name and the statement have **separate** row handles, since the two cases that matter are a per-character name color and a styled monologue for the main character. The manager reads the row and applies it with `URichTextBlock::SetDefaultTextStyle`, so a `<red>` span inside a line still wins for its own range and the typewriter output stays escaped. Nothing wraps the statement in an extra tag.

Interface and asset:

- [x] Add `ISpeaker` in `Public/Speakers/Speaker.h`, with `FKishiSpeakerStyle` (display name, tag, name style row, statement style row, default attributes), `FKishiSpeechOptions` (voice id, label, line index, attributes) and `FKishiVoiceLine` (sound, path, volume, advance-on-end).
- [x] Required `GetSpeakerStyle` and `ResolveVoiceLine`; optional `OnSpeechStarted` / `OnSpeechFinished` hooks. `ResolveVoiceLine`'s fallback returns `false` without logging, because a text-only script has no voice lines and that is not an error.
- [x] Add `USpeakerLibrary`: `LoadSpeakerByName` (convention search), `GetDisplayText`, `ResolveVoiceLineForSpeech`, `LoadVoiceSound`, `ApplyRichTextStyleRow` and `ApplySpeakerStyle`.
- [x] Add `UKishiCharacterAsset : UKishiDataAssetBase`, `Blueprintable`, implementing `ISpeaker` with a voice line map, a voice folder and auto voice ids.
- [ ] Tie the character asset to its showable with a `TSoftObjectPtr<UKishiShowableAsset>` once step 6 lands, and drop `FRpyCharacter::image`.

Script and execution:

- [ ] `FRpySpeaker { FName tag; TSoftObjectPtr<UObject> asset; FKishiSpeakerStyle inlineStyle; }` replaces `FRpyCharacter`, converted in `URpyScript::PostLoad`. Keep `characters` and `xmlGeneratedCharacterKeys` working through the transition, with `StructRedirects` if the name changes.
- [ ] `URpyScript::FindOrAddDefaultSpeaker(FName)` next to `FindOrAddDefaultImage`, so `<say character="john">` resolves `DA_John` with no definition at all.
- [ ] `<asset kind="speaker" name="john" path="Characters/John"/>`: one new `Kind` in the `defineAsset` branch.
- [ ] `<character>` keeps parsing. `displayName` fills the inline style; `color` has no equivalent in the new model, so it either logs a warning or maps to a style row (see the open decisions).
- [ ] `USayInstruction::Execute`: resolve the speaker, read its style into `session->statementState`, play its voice line through `IRpyAudioManager::Execute_PlayAudio` on the `voice` channel, call `Execute_OnSpeechStarted`, then `Execute_SayAs`.
- [ ] `voice` and `voiceAdvance` as optional `<say>` props, resolved against the speaker; `voice="auto"` uses the label and line index.
- [ ] `IRpyStatementManager::SayAs(FName, const FString&, const FKishiSpeakerStyle&)` as a new op whose `_Implementation` forwards to `Execute_Say`, so every existing Blueprint implementer keeps working unchanged. It has a working default, so it goes in the interface's main category, not `|Required` (a required fallback logs an error). Do not change `Say`'s signature.
- [ ] `FRpyStatementManagerState` gains the speaker style and a weak speaker reference, for save and load.
- [ ] `UKishiRpyStatementManagerWidgetBase::SayAs` applies both style rows and calls `Execute_OnSpeechFinished` when the line is fully revealed.
- [ ] Route visemes by `FKishiSpeakerStyle::Tag` instead of the display name, in `OnDialogueViseme` and in the show manager's speaker match.

Checks:

- [ ] Fixtures, named after the existing `say_` and `voice_` areas: `say_speaker_asset.xml`, `voice_speaker_line.xml`, and an invalid one whose `kind="speaker"` path is an asset that does not implement `ISpeaker`.
- [ ] Document the speaker reference, the two style rows and the `voice` props in `Scripts/XML_FORMAT.md`.
- [ ] Create `DA_John` and give the main character a monologue style row; play a script with both a voiced and an unvoiced line.

Open decisions:

- [ ] Does `<character color="...">` keep working by generating a style row, or is it dropped with a warning once an asset exists? (The asset should win where both are set.)
- [ ] Voice lookup: folder convention (`<VoiceFolder>/<VoiceId>`) or the explicit map on the asset? Both are in now; decide which one the fixtures and docs teach.

### 13. Content and source layout, and styles

**Goal:** every new style (a Disco Elysium-like investigation, a Danganronpa-like class trial, an Ace Attorney-like courtroom) brings its own UI design without forking the plugin. C++ is grouped by mechanic and Content by style. A style is a Content-only bundle: Blueprint children of the C++ Bases plus its theme. A mechanic a style needs (a dialogue log, skill checks, evidence, cross-examination, a debate minigame) goes into C++ as a generic feature under `Managers/<Mechanic>/`, with the usual Interface → Library → Base and an XML command, so later styles can reuse it.

**Rules:**

- A style depends only on `Core/`, plus at most one parent style it explicitly extends (for example, Courtroom reusing VN2D's choice manager). Styles never reference each other otherwise.
- Every style has the same subfolders and a name prefix: `UI/` (player, managers, save menu and slot, choice entry, prompts), `Theme/` (fonts, textbox art, rich-text table, UI sounds), `Gameplay/` (main actor, game mode), `Maps/` (its demo level).
- Name style folders and mechanics after what they do (`Investigation`, `Trial`, `Courtroom`, `Managers/Evidence`), not after the games that inspired them.

Target Content layout:

```text
Content/
  Core/           Widgets/ (KishiWidget, WGT_Sprite, Interaction/), Blueprints/ (Anchor, RpyMediaPlayer*, RpyFirstPersonGameMode),
                  Data/ (default DT_RichTextStyle, KeyPromptMapping), Materials/, Audio/SoundClass/
  Styles/         _Template/, VN2D/, VN3D/, FPS/, then new styles; each with DA_<Style>_Style, UI/, Theme/, Gameplay/, Maps/
  Demo/           Characters/<Name>/ (sprites, layered sprite, DA_<Name>, Voice/), Backgrounds/, Movies/, Music/, Models/, Scripts/, Maps/
  Tests/          Scripts/Fixtures/, Scripts/Samples/, Maps/ (fixture host levels)
  ThirdParty/     <Pack>/ for kenney, BridalUI, uipack_wave, otakuchama and the fonts, with their licenses in credits.txt
  Editor/         EBP_TextureAutomata, GroupToLayeredSprite
```

C++ (built 2026-11-22):

- [x] Add `UKishiVisualNovelSettings` (Project Settings > Plugins > Kishi Visual Novel) with ordered search folders for images, audio, transitions, speakers and voice lines. `URpyScript::AddDefaultImage` / `AddDefaultAudio` / `AddDefaultTransition` and `USpeakerLibrary::LoadSpeakerByName` / `LoadVoiceSound` use it. The defaults reproduce the old hardcoded order, so existing scripts resolve the same assets. A script path that starts with `/` is now used as is.
- [x] Group `Source/KishiVisualNovel` by feature: `Script`, `Session`, `Player`, `Managers/<Manager>`, `Save`, `Speakers`, `Showables`, `World`, `Interaction`, `Animation`, `Assets`, `Settings`, `Deprecated/Actors`. Files were moved with `git mv` and includes rewritten; no class was renamed, so Blueprints need no redirect. `KishiChoiceEntryWidget.cpp` is renamed to match its header.
- [ ] Add `UKishiVNStyleAsset : UPrimaryDataAsset` in `Styles/`: VN player class, save menu class, rich-text style table, default fonts and sounds, optional main actor and game mode.
- [ ] `URpyComponent`: add a `Style` property; when set, it supplies `VNPlayerClass`. Keep `VNPlayerClass` working for existing actors.
- [ ] When the demo content moves, add its folders (`/KishiVisualNovel/Demo/Characters`, `/KishiVisualNovel/Demo/Backgrounds`, and so on) to the search-folder defaults, or to `Config/DefaultGame.ini` for this project only.
- [ ] Rename `UCameraAnchorComponent` (interaction edge turning), which is easy to confuse with `URpyCameraAnchorComponent` (scene anchor). Needs a `ClassRedirects` entry.

Content (in the editor only, never on disk; commit first, and fix up redirectors after each batch):

- [ ] `Demo/`: move the sample characters, backgrounds, movies, music, models, `Scripts/db_cooper*` and `Maps/demo*`. Then update the search folders and re-import one fixture and one sample to check that resolution finds the new folders.
- [ ] `ThirdParty/`: move `UI/kenney`, `UI/BridalUI_All_Engines_`, `UI/uipack_wave`, `UI/otakuchama` and the fonts; list each pack and its license in `credits.txt`.
- [ ] `Tests/`: move `Scripts/Fixtures` and `Scripts/Samples`, and add a fixture host level under `Tests/Maps` if `VN3D_Demo` should stay a style demo.
- [ ] `Core/`: move `Widgets/`, `Blueprints/`, `Materials/`, `Fonts/DT_RichTextStyle`, `UI/KeyPromptMapping` and `Audio/SoundClass`. `RpyFirstPersonGameMode` is the project's default game mode: update `GlobalDefaultGameMode` in `Config/DefaultEngine.ini` if the redirector is removed.
- [ ] `Styles/<Style>/`: sort VN2D, VN3D and FPS into `UI/`, `Theme/`, `Gameplay/` and `Maps/`. Rename `Styles/FPS/VN3D_GameMode` to `FPS_GameMode`.
- [ ] Delete leftovers: `Images/Sylvie` (empty), `Widgets/RpyManagers` (empty), `Debug/Libraries/NewBlueprint`.
- [ ] Create `Styles/_Template` from VN2D: the player, the five managers, save menu and slot, choice entry and a style asset, with placeholder theme.
- [ ] Rerun the binary reference scan and the Node2Code export; compile all Blueprints.

Tests:

- [ ] Every fixture under `Scripts/xml/tests/fixtures` still imports and resolves its assets after the content moves.
- [ ] `VN2D_demo`, `VN3D_Demo`, `FPS_demo` and `DetectiveOffice_Layout` play as before.
- [ ] Switch a level from one style to another by changing only the style asset on its VN Main Component.

Open decisions:

- [ ] Should `Interaction` become its own module (`KishiInteraction`)? It has nothing VN-specific, and the FPS style uses it without scripts.
- [ ] Where do the class-trial models (`Models/Danganronpa_Trial`) go? If they were extracted from the game, keep them out of the plugin's shipped content (a local-only folder or `Demo/` excluded from packaging).

