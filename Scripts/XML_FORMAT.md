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

Required command data is explicit in attributes or element text. Supported commands include `label`, `scene`, `show`, `hide`, `say`, `menu`, `choice`, `set`, `jump`, `call`, `return`, `if`, `else`, `pause`, `play`, `queue`, `voice`, `stop`, `character`, and `asset`. For example, `scene` and `show` require `asset`; `hide` requires `tag`; `say` takes dialogue as text and optional `character`; `set` requires `name` and exactly one of `value` or `expression`.

Any additional command attributes are optional props and are stored in each instruction's `FRpyInstructionProps`. The props API provides typed getters and setters plus cloning. The XML importer infers booleans and numeric values; other attributes are strings. Registered C++ commands can extend the instruction vocabulary.

The runtime imports `.xml` scripts. `Tools/rpy_to_xml.py` is a migration utility for the supported subset of legacy `.rpy`; constructs it cannot translate are retained as `<raw source="..." />` for review.

Dialogue can contain nested XML style elements. Use matching XML tag names for the closing tags:

```xml
<say character="aug">I like this <red>dress</red>, and these <blue>jeans with <red>red stitching</red></blue>.</say>
```

The importer preserves the order of text and nested style elements. At runtime, the statement adapter converts the XML tags to Unreal Rich Text Block markup, which uses shorthand closing markers. Do not use the shorthand `</>` in XML; it is not well-formed XML.
