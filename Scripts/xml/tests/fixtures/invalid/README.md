# Invalid import fixtures

Every file in this directory must **fail** to import, and the importer must leave the
previously imported script data unchanged after each failure. Import each one and confirm
the log names the problem.

## Malformed XML

| File | Expected error |
| --- | --- |
| `shorthand_close_tag.xml` | invalid first character in an XML name (`</>` is Rich Text shorthand, not XML) |
| `mismatched_tag.xml` | closing tag does not match its opening tag |
| `unknown_entity.xml` | unknown XML entity `&nbsp;` |

## Rejected by instruction validation

| File | Expected error |
| --- | --- |
| `unknown_instruction.xml` | unknown XML instruction type `teleport` |
| `missing_show_asset.xml` | instruction `show` is missing required field `asset` |
| `set_value_and_expression.xml` | `set` takes exactly one of `value` or `expression` |
| `together_control_flow.xml` | `<together>` cannot contain `<jump>` |

## Custom instruction schema

These four need the example `camera` schema below to be registered. **No module registers
it today**, so all four currently fail as unknown instruction types, including the
otherwise valid `custom_instruction_schema.xml`. Register the schema to exercise the
field-level validation they were written for; the handler may be a no-op.

```cpp
FRpyInstructionSchema Schema;
Schema.RequiredFields.Add(FName(TEXT("target")), TEXT("name"));
Schema.OptionalProps.Add(FName(TEXT("duration")), TEXT("float"));
Schema.OptionalProps.Add(FName(TEXT("enabled")), TEXT("bool"));
Schema.OptionalProps.Add(FName(TEXT("offset")), TEXT("vector"));
FRpyInstructionRegistry::Register(FName(TEXT("camera")), Schema, CreateCameraInstruction);
```

| File | Expected error once the schema is registered |
| --- | --- |
| `custom_instruction_schema.xml` | none: it imports, with typed required fields and props |
| `missing_required_field.xml` | omits required `target` |
| `unknown_attribute.xml` | includes undeclared `speed` |
| `invalid_optional_type.xml` | non-numeric text for float `duration` |
