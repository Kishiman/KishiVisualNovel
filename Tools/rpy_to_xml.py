#!/usr/bin/env python3
"""One-time migration utility. It reads legacy .rpy syntax; runtime parsing is XML-only."""
from __future__ import annotations

import argparse
import re
import xml.etree.ElementTree as ET
from pathlib import Path


QUOTED = re.compile(r'''(["'`])((?:\\.|(?!\1).)*?)\1''')


def decode_string(value: str) -> str:
    if value.startswith(('"""', "'''")) and value.endswith(value[:3]):
        value = value[3:-3]
    elif len(value) >= 2 and value[0] in "\"'`" and value[-1] == value[0]:
        value = value[1:-1]
    return value.replace(r"\n", "\n").replace(r'\"', '"').replace(r"\'", "'")


def normalize_richtext_closers(value: str) -> str:
    """Convert Ren'Py's shorthand close tag into paired, well-formed XML tags."""
    result: list[str] = []
    stack: list[str] = []
    index = 0
    while index < len(value):
        if value[index] != "<":
            result.append(value[index]); index += 1; continue
        end = value.find(">", index + 1)
        if end < 0:
            result.append(value[index]); index += 1; continue
        token = value[index + 1:end]
        if token == "/" and stack:
            result.append(f"</{stack.pop()}>")
            index = end + 1
            continue
        if token.startswith("/") and token[1:] in stack:
            name = token[1:]
            while stack:
                top = stack.pop()
                result.append(f"</{top}>")
                if top == name: break
            index = end + 1
            continue
        if token and all(ch.isalnum() or ch in "_.-" for ch in token):
            result.append(f"<{token}>")
            stack.append(token)
            index = end + 1
            continue
        result.append(value[index:end + 1]); index = end + 1
    while stack:
        result.append(f"</{stack.pop()}>")
    return "".join(result)


def append_mixed_text(parent: ET.Element, text: str) -> None:
    if len(parent):
        parent[-1].tail = (parent[-1].tail or "") + text
    else:
        parent.text = (parent.text or "") + text


def append_richtext_xml(parent: ET.Element, value: str) -> None:
    """Emit a rich-text string as ordered XML text and style elements."""
    stack = [parent]
    index = 0
    while index < len(value):
        if value[index] != "<":
            next_tag = value.find("<", index)
            if next_tag < 0: next_tag = len(value)
            append_mixed_text(stack[-1], value[index:next_tag])
            index = next_tag
            continue
        end = value.find(">", index + 1)
        if end < 0:
            append_mixed_text(stack[-1], value[index:]); break
        token = value[index + 1:end]
        if token.startswith("/") and len(stack) > 1 and stack[-1].tag == token[1:]:
            stack.pop()
        elif token and all(ch.isalnum() or ch in "_.-" for ch in token):
            stack.append(ET.SubElement(stack[-1], token))
        else:
            append_mixed_text(stack[-1], value[index:end + 1])
        index = end + 1


def infer(line: str) -> tuple[str, dict[str, tuple[str, str]], dict[str, tuple[str, str]]]:
    """Return command, required values, optional props. Unrecognized source stays lossless."""
    text = line.strip()
    required: dict[str, tuple[str, str]] = {}
    props: dict[str, tuple[str, str]] = {}

    def req(name: str, value: str, typ: str = "string") -> None:
        required[name] = (typ, value)

    if text.startswith("label ") and text.endswith(":"):
        req("name", text[6:-1].strip()); return "label", required, props
    if text == "menu:": return "menu", required, props
    if text.startswith("if ") and text.endswith(":"):
        req("condition", text[3:-1].strip()); return "if", required, props
    if text in ("else:", "else :"):
        return "else", required, props
    if text.startswith("jump "):
        req("label", text[5:].strip()); return "jump", required, props
    if text.startswith("call "):
        req("label", text[5:].strip()); return "call", required, props
    if text == "return": return "return", required, props
    if text == "init:": return "init", required, props
    if text.startswith("pause "):
        req("duration", text[6:].strip(), "float"); return "pause", required, props

    match = re.match(r"^(scene|show|hide)\s+(.+)$", text)
    if match:
        command, tail = match.groups()
        option = re.search(r"\s+(with|at|onlayer|zorder)\s+", tail)
        subject = tail[:option.start()].strip() if option else tail.strip()
        subject_parts = subject.split()
        primary = subject_parts[0] if subject_parts else subject
        req("image" if command != "hide" else "tag", primary)
        if command != "hide" and len(subject_parts) > 1:
            props["attributes"] = ("string", " ".join(subject_parts[1:]))
        if option:
            optional = tail[option.start():].strip()
            for key, pattern, kind in (
                ("transition", r"\bwith\s+([\w.-]+)", "name"),
                ("position", r"\bat\s+(\([^)]*\)|[\w.-]+)", "string"),
                ("layer", r"\bonlayer\s+([\w.-]+)", "name"),
                ("zOrder", r"\bzorder\s+([+-]?\d+)", "integer"),
            ):
                found = re.search(pattern, optional)
                if found: props[key] = (kind, found.group(1))
            duration = re.search(r"\bwith\s+[\w.-]+\s+([+-]?(?:\d+\.\d*|\d*\.\d+|\d+))", optional)
            if duration: props["duration"] = ("float", duration.group(1))
        return command, required, props

    match = re.match(r"^(play|queue|stop)\s+(music|sound|voice|audio)\b(.*)$", text)
    if match:
        command, channel, tail = match.groups()
        req("channel", channel)
        asset = tail.strip().split()[0] if tail.strip() else ""
        if command in ("play", "queue"):
            req("asset", asset)
        elif asset:
            props["legacyChannelTarget"] = ("string", asset)
        rest = tail.strip()[len(asset):].strip() if asset else ""
        for key, pattern, kind in (
            ("volume", r"\bvolume\s+([\d.]+)", "float"),
            ("fadeOut", r"\bfadeout\s+([\d.]+)", "float"),
            ("fadeIn", r"\bfadein\s+([\d.]+)", "float"),
        ):
            found = re.search(pattern, rest)
            if found: props[key] = (kind, found.group(1))
        if re.search(r"\bloop\b", rest): props["loop"] = ("bool", "true")
        elif re.search(r"\bnoloop\b", rest): props["loop"] = ("bool", "false")
        if re.search(r"\bif_changed\b", rest): props["if_changed"] = ("bool", "true")
        return command, required, props

    match = re.match(r"^(voice)\s+(.+)$", text)
    if match:
        req("asset", decode_string(match.group(2))); return "voice", required, props

    match = re.match(r"^(define|default)\s+(\w+)\s*=\s*(.*)$", text)
    if match:
        scope, name, value = match.groups()
        req("name", name)
        if value.startswith("Character("):
            strings = [decode_string(m.group(0)) for m in QUOTED.finditer(value)]
            req("displayName", strings[0] if strings else "")
            props["declaration"] = ("string", "character")
            if len(strings) > 1: props["color"] = ("string", strings[1])
            return "defineCharacter", required, props
        if value in ("True", "False"):
            req("value", value.lower(), "bool")
        elif value.startswith(("'", '"', '`')):
            req("value", decode_string(value))
        else:
            req("value", value)
        props["scope"] = ("string", scope)
        return "set", required, props

    match = re.match(r"^(image|sound|music|audio|transition)\s+(.+?)\s*=\s*(.+)$", text)
    if match:
        kind, name, asset = match.groups()
        req("assetKind", kind); req("name", name.strip()); req("asset", decode_string(asset.strip()))
        return "defineAsset", required, props

    match = re.match(r"^\$\s*(\w+)\s*=\s*(.+)$", text)
    if match:
        name, value = match.groups(); req("name", name)
        if value in ("True", "False"): req("value", value.lower(), "bool")
        elif re.fullmatch(r"[+-]?\d+", value): req("value", value, "integer")
        elif re.fullmatch(r"[+-]?(?:\d+\.\d*|\d*\.\d+)", value): req("value", value, "float")
        elif value.startswith(("'", '"', '`')): req("value", decode_string(value))
        else: req("expression", value)
        return "set", required, props

    # Multiline dialogue/narration is coalesced before the line-oriented grammar.
    for delimiter in ('"""', "'''"):
        if delimiter in text:
            opening = text.find(delimiter)
            closing = text.rfind(delimiter)
            if closing > opening:
                prefix = text[:opening].strip()
                statement = text[opening:closing + 3]
                req("text", decode_string(statement))
                if prefix: props["speaker"] = ("string", prefix.split()[0])
                return "say", required, props

    # Dialogue, narration, and menu choices. Text is kept as explicit required data.
    quoted = list(QUOTED.finditer(text))
    if quoted:
        last = quoted[-1]
        before, statement = text[:quoted[0].start()].strip(), decode_string(last.group(0))
        trailing = text[last.end():].strip()
        effect = re.search(r"\bwith\s+([\w.-]+)", trailing)
        if effect: props["with"] = ("name", effect.group(1))
        is_choice = text.endswith(":") and len(quoted) == 1
        if is_choice:
            req("text", decode_string(quoted[0].group(0)))
            return "choice", required, props
        if len(quoted) > 1:
            props["speaker"] = ("string", decode_string(quoted[0].group(0)))
            req("text", statement)
        elif before:
            speaker = before.split()[0]
            props["speaker"] = ("string", speaker); req("text", statement)
        else:
            req("text", statement)
        return "say", required, props

    # Preserve unfamiliar commands verbatim so conversion never silently discards source.
    req("source", text)
    return "raw", required, props


def convert(path: Path, base: Path, output_root: Path) -> None:
    relative = path.relative_to(base)
    out_path = (output_root / relative).with_suffix(".xml")
    out_path.parent.mkdir(parents=True, exist_ok=True)
    root = ET.Element("visualNovel", {"version": "1"})
    stack: list[tuple[int, ET.Element]] = [(-1, root)]
    serial = 0
    source_lines = path.read_text(encoding="utf-8-sig").splitlines()
    logical_lines: list[tuple[int, str]] = []
    pending = ""
    pending_line = 0
    quote: str | None = None
    for line_number, raw in enumerate(source_lines, 1):
        if not pending and (not raw.strip() or raw.lstrip().startswith("#")):
            continue
        if not pending:
            pending, pending_line = raw, line_number
        else:
            pending += "\n" + raw
        for delimiter in ('"""', "'''"):
            if pending.count(delimiter) % 2:
                quote = delimiter
                break
        if quote:
            if pending.count(quote) % 2 == 0:
                logical_lines.append((pending_line, pending))
                pending, quote = "", None
            continue
        logical_lines.append((pending_line, pending))
        pending = ""
    if pending:
        logical_lines.append((pending_line, pending))

    for line_number, raw in logical_lines:
        if not raw.strip() or raw.lstrip().startswith("#"):
            continue
        indent = len(raw) - len(raw.lstrip(" \t"))
        source = raw.strip()
        while len(stack) > 1 and indent <= stack[-1][0]:
            stack.pop()
        command, required, props = infer(source)
        attrs: dict[str, str] = {}
        text_value = None
        aliases = {"image": "asset", "assetKind": "kind"}
        for name, (_, value) in required.items():
            if command == "say" and name == "text": text_value = normalize_richtext_closers(value)
            elif command == "defineAsset" and name == "asset": attrs["path"] = value
            else: attrs[aliases.get(name, name)] = value
        for name, (_, value) in props.items():
            if name not in ("scope", "declaration"):
                if name == "speaker": name = "character"
                attrs[name] = value
        tag = {"defineCharacter": "character", "defineAsset": "asset"}.get(command, command)
        if command == "set" and "expression" in required:
            attrs["expression"] = required["expression"][1]
        node = ET.SubElement(stack[-1][1], tag, attrs)
        if text_value is not None: append_richtext_xml(node, text_value)
        if source.endswith(":") or command in ("menu", "label", "if", "else", "choice"):
            stack.append((indent, node))

    ET.indent(root, space="  ")
    out_path.write_text(ET.tostring(root, encoding="unicode", xml_declaration=True) + "\n", encoding="utf-8")
    print(f"{path} -> {out_path}")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--project", type=Path, required=True)
    parser.add_argument("--plugin", type=Path, required=True)
    args = parser.parse_args()
    project = args.project.resolve()
    plugin = args.plugin.resolve()
    output = plugin / "Scripts" / "xml"
    for base in (plugin / "Scripts", project / "Scripts"):
        for source in sorted(base.rglob("*.rpy")):
            convert(source, base, output / ("plugin" if base == plugin / "Scripts" else "project"))


if __name__ == "__main__":
    main()
