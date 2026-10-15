# XML import validation fixtures

`../18_custom_instruction_schema.xml` is a valid custom-command fixture when the project registers the example `camera` schema below. The command handler may be a no-op for parser validation.

```cpp
FRpyInstructionSchema Schema;
Schema.RequiredFields.Add(FName(TEXT("target")), TEXT("name"));
Schema.OptionalProps.Add(FName(TEXT("duration")), TEXT("float"));
Schema.OptionalProps.Add(FName(TEXT("enabled")), TEXT("bool"));
Schema.OptionalProps.Add(FName(TEXT("offset")), TEXT("vector"));
FRpyInstructionRegistry::Register(FName(TEXT("camera")), Schema, CreateCameraInstruction);
```

The XML files in this directory are intentionally invalid. Import each one with the schema registered and confirm that import fails with a useful validation error:

- `missing_required_field.xml`: omits required `target`.
- `unknown_attribute.xml`: includes undeclared `speed`.
- `invalid_optional_type.xml`: supplies non-numeric text for float `duration`.

The importer should leave the previously imported script data unchanged after each failed import.
