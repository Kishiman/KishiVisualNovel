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
| Component   | `UKishiRpyShowManagerComponent`  | Same as the widget Base, for presentation in world space; attaches to any actor (step 10)                                                      |

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
| 8   | World-space managers    | Show and scene manager **components** (see step 10), not Actor Bases                                                                                                             | Optional: reparent world showables                                                                          | Decide which managers need world space                                           |
| 9   | Docs                    | None                                                                                                                                                                             | None                                                                                                        | Point `CLAUDE.md` at `@AGENTS.md`; add the architecture rules                    |
| 10  | Components over actors  | Component counterpart of every class in `Public/Actors`; `IRpyMainComponent`; actor-or-component lookup libraries                                                                | `VNCamera`, `RpyScriptInteractableActor`, then the other Legacy VN actors use the components                | Keep `Public/Actors` unchanged for existing assets; decide when to deprecate it  |

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
- [ ] When no asset uses the classes in `Public/Actors` any more, deprecate them (`UCLASS(Deprecated)`) or keep them? (Step 10.)
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
- [x] Copy only layout and styling from the Legacy widgets, not graph logic.
- [x] In `VN2D_RPYPlayer`, replace the four `WGT_RPY*Manager` children with the VN2D ones, one per interface.
- [x] Test a script that uses `show` with attributes, a `with` transition and dialogue lip sync.
- [ ] Confirm that no level references `WGT_VNPlayer` or `RpyActor`. **Failed (2026-11-01), see below.**
- [ ] Move `Styles/Legacy` to a quarantine folder and fix up redirectors. **Deferred: the game's levels still use Legacy.**
- [ ] Plan how the first-person game (`DetectiveOffice_Layout`, `RpyFirstPersonCharacter`) moves off Legacy. It needs the world-space scene path (`VNCamera` + `Anchor`), now in the scene manager Base, and the VN components from step 10.

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
- **Scene:** handles sprite, video and level sequence. ~~The Legacy `ENONE` / level path is missing.~~ Done (2026-11-07): for `<scene transition="..." layer="...">`, the Base attaches the active camera (the view target) to the layer's anchor with Keep Relative and plays the transition sequence, binding the sequence's `Camera`-tagged binding to that camera. It also works with `scene _`. Anchors are found by `IRpyCameraAnchor` (actor or component) first, then by actor tag (`AnchorClass`).
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

### 8. World-space managers

- [ ] Add show and scene manager components (not Actor Bases, see step 10) that use the same Libraries.
- [ ] Blueprint (optional): reparent `LayeredSpriteInteractableActor` and other world showables.

### 9. Docs

- [ ] Reduce `CLAUDE.md` to `@AGENTS.md` plus anything specific to Claude.
- [ ] Add a module map, the script pipeline, the Interface + Library + Base pattern, how to add an XML command, the instruction result contract, the showable registry rule, and the VN2D vs Legacy status to `AGENTS.md`.
- [ ] Add the plugin-specific rules (Interface + Library + Base, result contract, showable registry) to `Plugins/KishiVisualNovel/CLAUDE.md` too, since it already holds the plugin's source layout and runtime rules; keep only project-wide rules at the root.
- [ ] Update `Scripts/xml/XML_FORMAT.md` and the fixtures under `Scripts/xml/tests` if step 6 changes how scripts reference images.
- [ ] Trim the Node2Code panel walkthrough in `AGENTS.md`.
- [ ] Commit `AGENTS.md` and `CLAUDE.md`.

### 10. Components over actors

**Why:** `Maps/demo_danganronpa` runs its VN from `Blueprints/VNCamera`, which derives from `Pawn`. The native VN logic lives in Actor Bases (`ARpyMainActorBase` and the others), so using it would force `VNCamera` to change its parent class. Components can be added to any actor, whether it is an `Actor`, a `Pawn` or a `Character`.

**Rule:** new VN logic goes in `Source/KishiVisualNovel/Public/Components`. The classes in `Public/Actors` stay, unchanged, so existing assets keep working. They get no new features.

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

- [x] Add `IRpyMainComponent` and `URpyMainComponentLibrary` (`Interfaces/RpyMainComponent.h`).
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

- [ ] `VNCamera`: add a VN Main Component with `VNPlayerClass = VN2D_RPYPlayer`. Replace `Set Up Session And Player`, `Event Start Rpy Script`, `SaveToState` and `LoadFromState` with calls to the component. Then remove `IRpyMainActor` from its interfaces. Until then the pawn and the component would both save a session.
- [ ] `RpyScriptInteractableActor`: replace the `StartRpyScript` call on the `RpyMainActor` interface with `Start Rpy Script On Actor`, so it works with actors and components. `URpyScriptInteractionComponent` does this; see step 11.
- [ ] `PlayerAnchorActor`: use a Player Anchor Component. `UPlayerAnchorInteractionComponent` works with one; see step 11.
- [ ] `demo_danganronpa` (optional): replace the `Anchor` actors with VN Camera Anchor Components whose `AnchorLayer` is the old tag. Tagged actors still work through `AnchorClass`.
- [ ] `RpyActor` and `RpyFirstPersonCharacter`: move to the VN Main Component when they leave Legacy (step 2).
- [ ] Step 8: build the world-space show and scene managers as components.

Tests:

- [ ] `demo_danganronpa`: start a script on `VNCamera` through its component. Play `<scene asset="_" transition="intro" layer="student3" />`, then save, load and close mid-script, and check that a VN Close Listener Component fires.
- [ ] Put a VN Main Component on a plain `Actor`, a `Pawn` and a `Character`, and run one script on each.

Follow-up:

- [ ] `ARpyMainActorBase` and `URpyComponent` duplicate their session setup. Move it into the main Library in step 5, or leave it until the Actor classes are deprecated.

### 11. Interaction and first-person migration

**Goal:** move the interaction system to Interface + native component + Blueprint, and take the first-person game off `Legacy/RpyFirstPersonCharacter` without reimplementing the FPS basics. Following step 10, all of it is components, so designers attach it to existing actors and never have to inherit from a plugin class.

An actor has one `UInteractableComponent` and any number of handler components, for example "talk" + "shop" or "sit" + "move". At BeginPlay each handler finds or adds the Interactable and registers its action there. The Interactable broadcasts its delegates and runs only the handler registered for the requested action; that handler also gets `HandleInteractionEnd`, while focus and nearby hooks go to all handlers. It also shows the prompt widget at its own location while focused, when `InteractionWidgetClass` is set. Actors that only bind the delegates, or implement `IInteractable` without registering, keep working, so the Blueprints can be migrated one at a time. `EndInteraction` resets `State` to `Interactable` itself.

| Blueprint (`Blueprints/InteractableActors`) | Add these components                                               | Native behavior                                                                |
| ------------------------------------------- | ------------------------------------------------------------------ | ------------------------------------------------------------------------------ |
| `InteractableActor`                         | None: set `InteractionWidgetClass` on its Interactable             | Prompt widget that follows the Interactable while focused                      |
| `InteractableLightActor`                    | `ULightInteractionComponent`                                       | Its action toggles the owner's first light and ends the interaction            |
| `RpyScriptInteractableActor`                | `URpyScriptInteractionComponent`                                   | Starts the script on the interacting actor's VN host; ends when it closes      |
| `LayeredSpriteInteractableActor`            | `ULayeredSpriteWidgetComponent`, `URpyScriptInteractionComponent`  | World-space sprite that faces the owner's camera                               |
| `PlayerAnchorActor`                         | `UPlayerAnchorInteractionComponent`, `UPlayerAnchorComponent`      | Camera view until the `Return` action, with occupancy through the anchor       |

- A handler adds an Interactable component when the actor has none (`FindOrAddInteractable`). Add one yourself to set `InteractionWidgetClass`, `InteractableName` or the prompt's position.
- `UInteractionHandlerComponentBase` is the native starting point for new handlers, which implement `HandleInteractionStart`. `HandledActionName` (default `action`) is the input action that runs the handler, and `HandledActionLabel` is the prompt text. Each handler on an actor needs its own action name; a duplicate logs an error and is ignored.
- `Actions` on the Interactable is the list on offer: authored entries plus one per handler. A handler with the same name as an authored entry takes that entry, and an empty `HandledActionLabel` keeps its label. An action not on the list is ignored. An empty list accepts any action.
- A second action needs its own input: add a mapping in the input settings and add the name to `InteractActionNames` on `URpyFirstPersonComponent`. `RpyFirstPersonCharacter` only sends `action`.
- `UInteractionCameraHandlerComponentBase` adds the view change and `BlendTime`. The view moves only when the owner has a Camera component, and a Camera Anchor component on the owner adds edge turning.
- The script and anchor handlers do not need the owner to be a `StaticMeshActor`, or any particular class.

`URpyFirstPersonComponent` ("VN First Person Component") derives from `URpyComponent` and replaces the VN side of `RpyFirstPersonCharacter`:

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

- [x] Add `IInteractable`, `IInteractionPromptWidget` and `UInteractableLibrary` (`Interfaces/Interactable.h`).
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
- [ ] `LayeredSpriteInteractableActor`: note the size and transform of `LayeredSpriteWidget`, then replace it with a Layered Sprite Widget Component with `Widget Class = WGT_LayeredSprite`, and re-enter them. Set its `Layered Sprite` to the old variable's default (`Augustina`). Delete the construction script graph, the `WidgetLookAtCamera` macro, the `Layered Sprite` variable and the empty `ActorBeginOverlap` event.
- [ ] `RpyScriptInteractableActor`: add a VN Script Interaction Component with `InteractionScript = 00_hello_world` and `InteractionScriptLabel = start`, the old defaults; `BlendTime` already defaults to 0.5. Delete the whole event graph, including the `SetUpWidget` call and the nearby debug print. Delete `handleInteractionStart`, `handleInteractionEnd`, the `InteractionEnd` dispatcher and the three moved variables.
- [ ] `InteractableActor`: on the Interactable, set `InteractionWidgetClass = WGT_interaction` and set `InteractableName` from `Name`. Delete the event graph, `SetUpWidget`, `UpdateWidget`, `TickInteractableActor` and the `InteractionWidget` and `Name` variables. Compile all three Blueprints.
- [ ] Re-enter the recorded values on placed actors, on the components in each actor's Details panel. Press Play and run the script tests below while the player is still `RpyFirstPersonCharacter`. Both character types can host the new handler.

The first-person character, last:

- [ ] `Content/DetectiveStart/FirstPerson/Blueprints/FirstPersonCharacter`: delete the broken `InteractableScanner` variable and the `InputAction action` / `CallActionOnFocusedActor` nodes. Add a VN First Person Component with `VNPlayerClass` and `CursorWidgetClass = WGT_Cursor`, plus `StartScript` if the level needs one. Add an Interacting component too if `FocusDistance` or `NearbyRadius` need tuning.
- [ ] Set `Default Pawn Class` to `FirstPersonCharacter` on `Blueprints/RpyFirstPersonGameMode`, which is the project default and the mode `DetectiveOffice_Layout` references, and on `FirstPersonGameMode`.
- [ ] `DetectiveOffice_Layout`: delete the placed `VNFirstPersonCharacter_4`. The name comes from a redirector to `Legacy/RpyFirstPersonCharacter`, and it scans alongside the spawned player.
- [ ] Open the Reference Viewer on `Legacy/RpyFirstPersonCharacter` and clear what still points to it, including `Styles/FPS/FPS_RpyMainActor`. Then fix up the redirectors under `Blueprints`.

Tests:

- [ ] Add a handler component to a plain `Actor` and a `StaticMeshActor` that are not interaction Blueprints, and interact with each. The actor needs collision that blocks the Visibility channel.
- [ ] Check placed instances in `DetectiveOffice_Layout` and `Styles/FPS/FPS_demo` after re-entering their values.
- [ ] Two handlers on one actor, with two input mappings on the first-person component: the prompt lists both labels with their keys, each key runs its own handler, and only that handler gets the end call. Two handlers with the same action name log an error.
- [ ] Light: toggle it twice, then focus something else. The second toggle and the refocus failed before the `UInteractingComponent` fix.
- [ ] Script interactable, with and without a Camera on the owner: start, finish and close. The view returns, the player can move, and the actor can be used again. Repeat three times, because `SetIgnoreMoveInput` is a counter.
- [ ] Player anchor, with and without a Camera Anchor: enter, leave with `Return`, re-enter. With a Player Anchor Component, a second occupant is refused.
- [ ] Layered sprite: the sprite shows in a level loaded in PIE and faces the camera.
- [ ] Save and load mid-script on the first-person character.
- [ ] Possess a pawn after BeginPlay and check that `action` still works.
