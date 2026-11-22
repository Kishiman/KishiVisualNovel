# XML script tests

Two kinds of script live here, plus a folder of scripts that must not import at all.

## fixtures/

One feature per file. A fixture opens with a comment that names the feature and states the
expected result, declares the assets it needs with `<asset>`, keeps its dialogue about the
behaviour under test, and runs in a few seconds. Add a fixture whenever a parser or manager
behaviour is worth pinning down; name it `<area>_<feature>.xml`, with no number.

| Fixture | Covers |
| --- | --- |
| `smoke_minimal.xml` | smallest valid document; default script for interaction actors |
| `say_speaker.xml` | where the name box text comes from, including empty narration |
| `character_definitions.xml` | `<character>` with `displayName`, `color`, `image`, `voice` |
| `text_rich_markup.xml` | style elements versus escaped literal angle brackets |
| `text_whitespace.xml` | wrapped lines, `&#10;`, CDATA, in-line tabs |
| `scalar_values.xml` | scalar type inference and `scope="default"` |
| `asset_resolution.xml` | declared paths, implicit names, the `_Sprite` fallback |
| `image_types.xml` | sprite, layered sprite, level sequence and video showables |
| `show_transitions.xml` | every built-in transition name and direction |
| `show_positions.xml` | named and vector positions, `zOrder`, moving a shown sprite |
| `show_layered_attributes.xml` | attribute groups, widget reuse on re-show |
| `scene_transitions.xml` | backgrounds, transitions, `duration`, `layer` |
| `scene_clear.xml` | `scene asset="_"` with and without a layer |
| `scene_level_sequence.xml` | sequence transitions on named camera anchors |
| `voice_lipsync.xml` | the voice channel and viseme-driven mouth layers |
| `audio_channels.xml` | `play` / `queue` / `stop`, fades, volume, loop |
| `flow_menu_jump.xml` | menus, choices, jumps, reconverging branches |
| `flow_if_else.xml` | flags, `if` / `else` |
| `flow_call_return.xml` | the call stack |
| `flow_pause_return.xml` | timed pauses and script end |
| `session_save_restore.xml` | save and load mid-script |

## samples/

Full demos, played start to finish, written the way a real script is written. They are not
feature probes: when a sample breaks, look for the fixture that covers the broken feature.

| Sample | Covers |
| --- | --- |
| `a_simple_game.xml` | dialogue only, no assets |
| `the_question.xml` | the full demo: assets, transitions, audio, branching, three endings |

The project's own scripts live outside this folder, in `Scripts/xml/` and
`Content/VisualNovel/Scripts`.

## fixtures/invalid/

Scripts that must fail to import, with the expected error for each. See its README.

## Assets the fixtures reference

All of these exist in the plugin content. An image or sound name resolves against
`/Game/`, `/Game/<kind>/`, `/KishiVisualNovel/` and `/KishiVisualNovel/<kind>/`, trying the
name itself first and then `<name>_Sprite`.

| Asset | Type | Used by |
| --- | --- | --- |
| `bg_meadow`, `bg_lecturehall`, `bg_uni`, `bg_club`, `black` | Texture + `_Sprite` PaperSprite | scene and show fixtures, `the_question` |
| `sylvie` | LayeredSprite: `green`\|`blue` x `normal`\|`smile`\|`giggle`\|`surprised` | show fixtures, `the_question` |
| `Augustina` | LayeredSprite: `outfit` (`dress`/`jeans`/`swimsuit`), `mouth`, `eyes`, `eyebrows`, `emotion`, `glasses` | `show_layered_attributes`, `voice_lipsync`, `scene_level_sequence` |
| `eileen` | LayeredSprite | `show_positions` |
| `DBCooper/airplane_attendent_LS` | LayeredSprite | `asset_resolution` |
| `stairs` | LevelSequence shown as an image | `image_types` |
| `DBCooperKeiku/plane_takeoff` | VideoMediaSprite | `image_types` |
| `Transitions/trial_intro` | LevelSequence used as a transition | `scene_level_sequence` |
| `illurock`, `pixabay/footsteps-in-a-hallway`, `effect` | SoundWave | `audio_channels`, `the_question` |
| `cicifyre/BrightFemale/01-*` | SoundWave | `voice_lipsync`, `audio_channels`, `the_question` |

Audio channel names come from the `AudioChannels` map on the VN audio manager, which
defines `music`, `sound` and `voice`. A channel that is not in the map logs an error.

A `queue` starts when the audio ahead of it on the channel reaches its end, so nothing can
be queued behind a looping audio. The lip-sync blip of the audio manager plays on the
`sound` channel as a one-shot, beside the channel's own audio rather than in its place, so
a fixture may play a sound effect and dialogue at the same time.

## Behaviour that logs but still imports

These are deliberately not in `fixtures/invalid/`, because the importer accepts them:

- an unknown image or sound name logs `Could Not Found Default Image` / `Audio` and the
  instruction is dropped;
- an unknown transition name logs `Could Not Found Default Transition` and the show or
  scene runs without a transition;
- a duplicate `<label name="...">` silently replaces the earlier label;
- a `<jump>` or `<call>` to a label that does not exist is only detected at run time.

At run time, an instruction that cannot run (an undefined image or sound, a missing manager,
a menu with no choices, an unsupported command) logs an error and returns `Failed`. The
session then follows the Failed Instruction Policy in Project Settings > Plugins > Kishi
Visual Novel: by default it logs and moves on to the next instruction; `Log and Stop` ends
the script instead.
