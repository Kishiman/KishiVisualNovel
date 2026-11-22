# KishiVisualNovel: Summary and Roadmap

Source: the audit doc [KishiVisualNovel Plugin Audit & Proposed Architecture](https://claude.ai/artifact/Dpi71tF9rdyjpQGQTNEppg) (2026-11-01). That doc also has the full findings table and the architecture diagram.

## Summary

The XML script pipeline and the interface layer are in good shape. Since the Legacy content was deleted (2026-11-22), every feature follows Interface → C++ Base → Blueprint or C++ child. The asset layer (`FRpyImage`) is the main thing blocking a clean showable abstraction.

- **Done in C++:** XML parser, instruction registry, `URpySession`, the five manager interfaces and their widget Bases, `UKishiVNPlayerWidgetBase`, the save menu and save slot (`IRpySaveMenu` / `IRpySaveSlot` and their widget Bases), `ISpeaker` with `UKishiCharacterAsset`, the VN and interaction components, and the `IShowable` / `IAnimatable` widgets.
- **Legacy removed (2026-11-22):** `Styles/Legacy` and the old Blueprints, widgets and maps (`demo1`, `demo_danganronpa`, `test_widget`, `VNCamera`, `RpyActor`, `RpyFirstPersonCharacter` and their redirectors) are deleted. VN2D, VN3D and FPS run on the C++ Bases and components. A scan of the binary assets finds no asset that uses the Actor Bases in `Public/Deprecated/Actors` any more.
- **Biggest risk:** `Execute()` returns `false` both for "wait for completion" and for failure. When a manager fails, the session waits for a completion that never comes.
- **Biggest design gap:** `FRpyImage` is a tagged union (sprite, layered sprite, video, level sequence), and every manager switches on its type, each supporting a different subset. Showables should be resolved through an asset-to-showable registry instead.
- **Conventions:** most interfaces follow the `Required` / `Hooks` rules. The exceptions: some interface events, both optional hooks and required operations, are declared `= 0`, and a few calls bypass `Execute_*`.

### Key findings

| #   | Severity | Where                                                     | Finding                                                                                                                                      |
| --- | -------- | --------------------------------------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------- |
| 1   | High     | `URpySession::StartLabel`                                 | `script->labels[label]` asserts on a missing label; use `labels.Find`.                                                                       |
| 2   | High     | `URpySession::Run` + managers                             | `false` means both "wait" and "failed", so a failure stalls the script.                                                                      |
| 3   | High     | `UKishiRpyShowManagerWidgetBase::Show`                    | Returns `true` only for sprites, so video and level-sequence images stall the script.                                                        |
| 4   | High     | `RpyInstruction.h`                                        | `operator&` performs a bitwise OR; `operator!` returns `bool(value)` instead of its negation.                                                |
| 5   | Medium   | Session vs managers                                       | Show, scene and statement state is stored twice and can drift; `LoadFromState` replays `Show` / `Say` instead of calling `Set*ManagerState`. |
| 6   | Medium   | Manager interfaces                                        | Both `Get*ManagerState` and `GetCurrent*State` exist; the `_Implementation` constness doesn't match the UFUNCTION.                           |
| 7   | Medium   | Interpreter, BaseManager, MainActor, LayeredSpriteHandler | Interface events declared `= 0` (also in `IRpyCameraAnchor`): optional hooks, and required ops that should log and return a default.         |
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
- **Speakers:** `ISpeaker`, implemented by an asset or an actor, replaces the inline `FRpyCharacter`. `UKishiCharacterAsset` owns the display name, the showable tag, one `DT_RichTextStyle` row for the name and another for the statement, and the voice lines. `<say>` resolves the speaker, plays its voice line on the `voice` channel, and hands the style to the statement manager (step 12).
- **State:** `URpySession` becomes the only owner of state. Managers rebuild the view from it in `Set*ManagerState`.

## Roadmap

Steps 1, 2, 10 and the C++ of 11 are done. Legacy was deleted on 2026-11-22, so the remaining steps only touch C++ and the VN2D, VN3D and FPS Blueprints built on the Bases, with no Legacy Blueprint rework. Steps 3 to 5 come next; step 3 waits on the `Failed` decision. Step 13's content moves can run alongside them, since they touch only assets and settings.

Rename any UFUNCTION, class or struct with a `[CoreRedirects]` entry in `Config/DefaultEngine.ini` so existing Blueprint nodes keep resolving.

| #   | Step                    | C++ change                                                                                                                                                                       | Blueprint rework                                                                      | Other actions                                                        |
| --- | ----------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------- | -------------------------------------------------------------------- |
| 1   | Stabilize the session   | `labels.Find`; fix `operator&` / `operator!`; `ClearScript` removes only its argument; drop `try` / `catch`; `InterpreterClass` becomes `TSubclassOf<UObject>` + `MustImplement` | None                                                                                  | Test that a missing label logs an error instead of asserting         |
| 2   | Port the VN2D managers  | Only hooks a Base is missing                                                                                                                                                     | Done: VN2D Choice, Scene, Show, Audio, Save menu and Save slot from the C++ Bases     | Done: Legacy deleted (2026-11-22)                                    |
| 3   | Instruction result enum | `ERpyInstructionResult` on `Execute()` and on every manager operation                                                                                                            | Refresh the return pins in the VN2D managers                                          | Decide what `Failed` does                                            |
| 4   | Tidy the interfaces     | No `= 0` on interface events; remove `GetCurrent*State`; fix `const`; add categories; use `Execute_*` at self-calls; PascalCase menu functions                                   | Refresh the state and menu nodes in the VN2D widgets                                  | `FunctionRedirects`                                                  |
| 5   | Libraries               | Rename the interpreter library; add five manager libraries; move shared logic into `*_Default`                                                                                   | Check that the VN2D nodes still resolve                                               | `ClassRedirects`                                                     |
| 6   | Showable assets         | Asset hierarchy, registry, `SetShowableAsset`, slimmer `FRpyImage` with a `PostLoad` conversion                                                                                  | Reparent `WGT_Sprite`, or implement `SetShowableAsset`                                | Create assets; fill the registry; resave scripts and layered sprites |
| 7   | Unify show and scene    | Shared show Library; delete the `ShownWidgets` path; merge the option structs                                                                                                    | Re-wire struct nodes in `KishiWidget`; add a background canvas to `VN2D_SceneManager` | `StructRedirects`                                                    |
| 8   | World-space managers    | Show and scene manager **components** (see step 10), not Actor Bases                                                                                                             | Optional: reparent world showables                                                    | Decide which managers need world space                               |
| 9   | Docs                    | None                                                                                                                                                                             | None                                                                                  | Point `CLAUDE.md` at `@AGENTS.md`; add the architecture rules        |
| 10  | Components over actors  | Component counterpart of every class in `Public/Deprecated/Actors`; `IRpyMainComponent`; actor-or-component lookup libraries                                                     | Done: the VN styles and interaction actors use the components                         | Deprecate `Public/Deprecated/Actors`, which no asset uses            |
| 11  | Interaction             | Interaction interfaces and handler components; `URpyFirstPersonComponent`                                                                                                        | Finish the script chain in `Blueprints/InteractableActors`                            | Re-enter placed values in `DetectiveOffice_Layout` and `FPS_demo`    |
| 12  | Speakers                | `ISpeaker` and `UKishiCharacterAsset` (done); `FRpySpeaker`, `SayAs`, voice playback                                                                                             | None until the statement manager applies the styles                                   | Fixtures, `XML_FORMAT.md`, `DA_John`                                 |
| 13  | Layout and styles       | Configurable search folders and feature folders (done); `UKishiVNStyleAsset`; `URpyComponent` takes a style                                                                      | Move Content into `Core`, `Styles`, `Demo`, `Tests`, `ThirdParty`; `_Template` style  | Fix up redirectors; re-import fixtures; rerun the reference scan     |

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

- [ ] Add `ERpyInstructionResult { Continue, Wait, Failed }`.
- [ ] Make `URpyInstruction::Execute()` return it, and store it on the instruction in place of `bAutoExecuteNext`.
- [ ] Make `Scene`, `Show`, `Hide`, `Say`, `Menu`, `PlayAudio`, `QueueAudio`, `StopAudio` and `PauseAudio` return it on the interfaces and Bases.
- [ ] Update `URpySession::Run` to advance on `Continue`, wait on `Wait`, and log and apply the chosen policy on `Failed`.
- [ ] Show manager Base: return `Failed` for unsupported types instead of stalling (finding 3).
- [ ] Blueprint: refresh the VN2D managers and fix any return pins.

### 4. Tidy the interfaces

- [ ] Remove every `= 0` on an interface event, in `IRpyScriptInterpreter`, `IRpyScriptInterpreterBaseManager`, `IRpyMainActor`, `ILayeredSpriteHandler` and `IRpyCameraAnchor`. Hooks get empty bodies. Required operations (`ILayeredSpriteHandler` get/set, `IRpyMainActor::IsActive` / `GetCurrentSession`, the `IRpyCameraAnchor` getters) get a fallback that logs an error and returns a safe default.
- [ ] Remove `GetCurrent*State`, keeping one `Get*ManagerState` / `Set*ManagerState` pair per manager.
- [ ] Make `_Implementation` constness match each UFUNCTION.
- [ ] `IAnimatable`: add `Required` / `Hooks` categories and defaults, and rename `HandleAnimationEnded` to `OnAnimationEnded` if it is a hook.
- [ ] `ILayeredSpriteHandler`: add categories.
- [ ] Use `IRpyShowManager::Execute_Show` in `SetShowManagerState`, and `Execute_CloseVN` in `StartRpyScript`.
- [ ] Rename `getSaveMenuVisible`, `setSaveMenuVisible` and similar functions to PascalCase, and add `FunctionRedirects`. Do this early: the statement manager, save menu and VN player Bases already call them, and each new Base adds call sites.
- [ ] Blueprint: delete any `GetCurrent*State` overrides and refresh the menu nodes in the VN2D widgets (`VN2D_RPYPlayer`, `VN2D_StatementManager`, `VN2D_SaveMenu`).

### 5. Libraries

- [ ] Rename `URpyScriptInterpreterImplementation` to `URpyScriptInterpreterLibrary` and add a `ClassRedirects` entry.
- [ ] Add the five `U*ManagerLibrary` classes (replace the commented-out stubs).
- [ ] Move shared Base logic into the `*_Default` functions; Bases call the Library.
- [ ] Blueprint: check that the VN2D, VN3D and FPS Blueprints still compile after the rename.

### 6. Showable assets

- [ ] Add `UKishiShowableAsset : UPrimaryDataAsset`.
- [ ] Move `ULayeredSprite` and `UVideoMediaSprite` under it.
- [ ] Add `UKishiSpriteAsset`, `UKishiModelAsset` and `UKishiSequenceAsset`.
- [ ] Add a registry to `UKishiVisualNovelSettings` (it exists since step 13 and holds the asset search folders) that maps asset class to widget and actor showable classes and resolves up the class hierarchy.
- [ ] Add `IShowable::SetShowableAsset` (required), plus `OnShown` / `OnHidden` hooks.
- [ ] Reduce `FRpyImage` to `Tag` + `TSoftObjectPtr<UKishiShowableAsset>`, and convert from `ERpyImageType` in `PostLoad`.
- [ ] Show manager `Show`: resolve, create, then `Execute_SetShowableAsset` and `Execute_Show`.
- [ ] Blueprint: reparent `WGT_Sprite`, or implement `SetShowableAsset` on it. (`WGT_LayeredSprite` was deleted; layered sprites use the native `UKishiLayeredSpriteWidget`.)
- [ ] Create the sprite, model and sequence assets.
- [ ] Fill the registry in Project Settings.
- [ ] Resave every `URpyScript` and `ULayeredSprite` asset.

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

- [ ] `ARpyMainActorBase` and `URpyComponent` duplicate their session setup. Deprecating the Actor classes (no asset uses them) removes the duplicate; otherwise move it into the main Library in step 5.

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
