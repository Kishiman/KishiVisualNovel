# Visual novel XML scripts

Scripts use readable command elements. Commands nest directly inside labels, menus, choices, and conditional blocks:

```xml
<visualNovel version="1">
  <label name="prologue">
    <scene asset="bg_airport" transition="fade" duration="0.5" />
    <say character="Augustina">We should go.</say>
    <menu>
      <choice text="Wait here">
        <set name="waited" value="true" />
        <jump label="waiting" />
      </choice>
      <choice text="Leave now">
        <jump label="departure" />
      </choice>
    </menu>
  </label>
</visualNovel>
```

Required command data is explicit in attributes or element text. Supported commands include `label`, `scene`, `show`, `hide`, `say`, `menu`, `choice`, `set`, `jump`, `call`, `return`, `if`, `else`, `pause`, `play`, `queue`, `voice`, `stop`, `together`, `character`, and `asset`. For example, `scene` and `show` require `asset`; `hide` requires `tag`; `say` takes dialogue as text and optional `character`; `set` requires `name` and exactly one of `value` or `expression`.

## Timing: one command after another, or `<together>`

Commands run in order, and a command that takes time finishes before the next one starts: a `show` or `hide` with a `transition` and a `duration`, a line of dialogue (until the player clicks through it), and a `pause`. So a `hide` that fades out over half a second is over before the `show` after it begins, and the dialogue after a `show` appears once the sprite has arrived. Commands without a transition, `play`, `queue`, `stop` and `set` take no time. A `scene` does not wait for its transition yet.

A click outside dialogue skips what the script is waiting for: running transitions jump to their end, and a `pause` ends.

To run commands at the same time, put them in `<together>`. Every command in it starts at once, and the script continues after the block when all of them have finished:

```xml
<together>
  <hide tag="sylvie" transition="fade" duration="0.5" />
  <show asset="eileen" transition="easeinright" duration="0.5" />
  <say character="e">This line appears while they swap.</say>
</together>
```

A `<together>` holds only commands that run and finish on their own: `say`, `show`, `hide`, `scene`, `pause`, `play`, `queue`, `voice`, `stop`, `set` and registered custom commands. Labels, menus, conditions, jumps, calls, returns and nested `<together>` blocks are rejected when the script is imported. If two commands in a block use the same tag, the later one wins: a `show` of a sprite that is still fading out from a `hide` ends the hide and brings the sprite back in.

Ease-in and move-in transitions (`easeinleft`, `moveintop` and so on) slide in an image that is entering. Showing an image that is already on screen, at the same position, does not move it; hide it first to replay its entrance.

Any additional command attributes are optional props and are stored in each instruction's `FRpyInstructionProps`. The props API provides typed getters and setters plus cloning. The XML importer infers booleans and numeric values; other attributes are strings. Registered C++ commands can extend the instruction vocabulary.

The runtime imports `.xml` scripts. `Tools/rpy_to_xml.py` is a migration utility for the supported subset of legacy `.rpy`; constructs it cannot translate are retained as `<raw source="..." />` for review.

Dialogue can contain nested XML style elements. Use matching XML tag names for the closing tags:

```xml
<say character="aug">I like this <red>dress</red>, and these <blue>jeans with <red>red stitching</red></blue>.</say>
```

The importer preserves the order of text and nested style elements. At runtime, the statement adapter converts the XML tags to Unreal Rich Text Block markup, which uses shorthand closing markers. Do not use the shorthand `</>` in XML; it is not well-formed XML.

To display angle-bracket text literally, escape it using XML entities. The importer keeps that distinction from actual child style elements, and Unreal Rich Text decodes the escaped characters for display:

```xml
<say character="narrator">Type &lt;red&gt; to show the literal text &lt;red&gt;.</say>
```

Line breaks in the file, and the indentation that follows them, are formatting rather than script content, so long dialogue can be wrapped freely. Every run of literal whitespace that spans a line break is read as a single space, and a statement is trimmed at both ends. Write an intentional line break as `&#10;`, which is decoded after that normalization, or put the text in a `<![CDATA[ ... ]]>` section, which is kept verbatim. Runs of spaces and tabs within one line are kept as written.

```xml
<say character="aug">
  This wrapped line reads as one line,
  with a single space before "with".
</say>
<say character="aug">First line.&#10;Second line.</say>
```

Custom C++ instructions register a field schema with their factory. Required attribute values are passed in the required-field map; optional attributes are passed as typed `FRpyInstructionProps`. Supported schema types are `string`, `bool`, `integer`, `float`, `name`, `asset`, and three-component `vector`. Attributes absent from both maps are rejected.

```cpp
FRpyInstructionSchema Schema;
Schema.RequiredFields.Add(FName(TEXT("target")), TEXT("name"));
Schema.OptionalProps.Add(FName(TEXT("duration")), TEXT("float"));
FRpyInstructionRegistry::Register(FName(TEXT("camera")), Schema, CreateCameraInstruction);
```

`URpyComponent` session state is captured by the base save game mode for stateful components and restored after the destination map loads. The component must have a valid `InterpreterClass` configured so its interpreter can be recreated during restoration.
