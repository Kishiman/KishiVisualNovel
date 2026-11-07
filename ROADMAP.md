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

Port the VN2D managers right after the session fixes. That way steps 3 to 7 change the VN2D widgets built on the C++ Bases. The five Legacy widgets still need the step 3 and step 4 Blueprint fixes, because the game's levels use them (see the step 2 findings).

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

Decisions (2026-11-01): Legacy stays in place and quarantine is deferred. VN2D uses the native showable widgets, and the show fixes live in C++.

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
- [ ] Copy only layout and styling from the Legacy widgets, not graph logic.
- [ ] In `VN2D_RPYPlayer`, replace the four `WGT_RPY*Manager` children with the VN2D ones, one per interface.
- [ ] Test a script that uses `show` with attributes, a `with` transition and dialogue lip sync.
- [ ] Confirm that no level references `WGT_VNPlayer` or `RpyActor`. **Failed (2026-11-01), see below.**
- [ ] Move `Styles/Legacy` to a quarantine folder and fix up redirectors. **Deferred: the game's levels still use Legacy.**
- [ ] Plan how the first-person game (`DetectiveOffice_Layout`, `RpyFirstPersonCharacter`) moves off Legacy. It needs the world-space scene path (`VNCamera` + `Anchor`) from step 8.

#### Findings (2026-11-01)

Legacy is still in use. Assets that reference it, found by scanning the binary assets (some go through the redirectors in `/KishiVisualNovel/Blueprints/`):

| Legacy asset              | Used by                                                                                                                 |
| ------------------------- | ----------------------------------------------------------------------------------------------------------------------- |
| `RpyActor`                | Levels `demo1` (game `Content/VisualNovel/Maps`), `demo`, `demo_keiku`; `RpyScriptInteractableActor`                    |
| `RpyFirstPersonCharacter` | Level `DetectiveOffice_Layout` (game); `FirstPersonGameMode` (game); `RpyFirstPersonGameMode`; `VNFirstPersonCharacter` |
| `VNCamera`                | Level `demo_danganronpa`; `CameraActor`; transition `trial_intro`                                                       |
| `WGT_VNPlayer`            | `Widgets/WGT_KVNInterpreter`                                                                                            |
| `WGT_LayeredSprite`       | `LayeredSpriteInteractableActor`, `Widgets/WGT_PSDFile`                                                                 |
| `WGT_RPY*Manager`         | `VN2D_RPYPlayer` (all five), `Widgets/RpyManagers/WGT_Showmanager`                                                      |
| `WGT_SaveMenu`            | `VN2D_RPYPlayer`, `WGT_RPYSaveMenu`, level `test_widget`                                                                |

The game's own first-person level runs on Legacy. So the earlier assumption that steps 3 to 7 only break VN2D does not hold.

Gaps between the C++ Bases and the Legacy managers:

- **Choice:** at parity. VN2D needs a choice entry widget derived from `UKishiChoiceEntryWidgetBase` (Legacy uses `IndexedButton`).
- **Audio:** at parity. It plays the blip on visemes and has the same channels; copy the `AudioChannels` map and `Blip` from `WGT_RPYAudioManager`.
- **Scene:** handles sprite, video and level sequence. The Legacy `ENONE` / level path, which attaches `VNCamera` to an `Anchor` tagged with `options.layer`, is missing. That is world-space behavior (step 8); VN2D doesn't need it.
- **Show:** three gaps.
  - `options.attribute` is ignored, so expressions are never applied.
  - Each `Show` recreates the widget instead of reusing the one with the same tag.
  - Visemes aren't forwarded to the speaker's layered sprite, and nothing calls `UKishiLayeredSpriteWidgetBase::UpdateVisemeLayer`.

  The native `UKishiSpriteWidget` / `UKishiLayeredSpriteWidget` already handle attributes through `IShowable::Show`, but the Rpy `Show` path uses the reflection-based `PlayTransition` on `KishiWidget` instead, and no Blueprint uses the native widgets.

### 3. Instruction result enum

- [ ] Add `ERpyInstructionResult { Continue, Wait, Failed }`.
- [ ] Make `URpyInstruction::Execute()` return it, and store it on the instruction in place of `bAutoExecuteNext`.
- [ ] Make `Scene`, `Show`, `Hide`, `Say`, `Menu`, `PlayAudio`, `QueueAudio`, `StopAudio` and `PauseAudio` return it on the interfaces and Bases.
- [ ] Update `URpySession::Run` to advance on `Continue`, wait on `Wait`, and log and apply the chosen policy on `Failed`.
- [ ] Show manager Base: return `Failed` for unsupported types instead of stalling (finding 3).
- [ ] Blueprint: refresh the VN2D managers and fix any return pins.
- [ ] Blueprint (required, since Legacy is kept and the game uses it): redo the return values in the five `WGT_RPY*Manager` widgets.

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
- [x] Delete the `ShownWidgets` path and keep `TrackedShowables` (done in step 2).
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
