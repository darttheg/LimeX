#!/usr/bin/env python3
import argparse
import re
from pathlib import Path

MODULE_RE    = re.compile(r'^\s*//\s*Module\s+(.+?)\s*$')
ENDMODULE_RE = re.compile(r'^\s*//\s*End\s+Module\s*$')
OBJECT_RE    = re.compile(r'^\s*//\s*Object\s+(.+?)\s*$')
ENDOBJECT_RE = re.compile(r'^\s*//\s*End\s+Object\s*$')

FIELD_RE     = re.compile(r'^\s*//\s*Field\s+(\S+)\s+([A-Za-z_]\w*)\s*$')  # // Field <type> <name>

COMMENT_RE   = re.compile(r'^\s*//\s*(.*?)\s*$')
RET_LINE_RE  = re.compile(r'^\s*Returns\s+(.+?)\s*$')

BIND_PATTERNS = [
    re.compile(r'\bmodule\.set_function\s*\(\s*"([^"]+)"'),
    re.compile(r'\bmodule\[\s*"([^"]+)"\s*\]\s*='),
    re.compile(r'\bbindType\[\s*"([^"]+)"\s*\]\s*='),
    re.compile(r'\bbindType\.set_function\s*\(\s*"([^"]+)"'),
]

def parse_params(raw: str):
    raw = raw.strip()
    if not raw:
        return []
    out = []
    for item in raw.split(","):
        part = item.strip()
        if not part:
            continue
        bits = part.split()
        if len(bits) < 2:
            continue
        typ = bits[0]
        name = bits[1]
        out.append((name, typ))  # (name, type)
    return out

def normalize_varargs(params):
    for idx, (name, _typ) in enumerate(params):
        if name == "...":
            return params[:idx] + [params[idx]]
    return params

def ensure_tables(path: str, emitted: set, out_lines: list[str]):
    path = path.strip()
    if not path:
        return
    parts = path.split(".")
    if len(parts) <= 1:
        return
    for k in range(2, len(parts) + 1):
        sub = ".".join(parts[:k])
        if sub not in emitted:
            out_lines.append(f"{sub} = {sub} or {{}}")
            emitted.add(sub)

def ensure_object_class(obj: str, emitted_classes: set, out_lines: list[str]):
    obj = obj.strip()
    if not obj or obj in emitted_classes:
        return
    out_lines.append(f"---@class {obj}")
    out_lines.append(f"{obj} = {obj} or {{}}")
    emitted_classes.add(obj)

def extract_binding_name(line: str):
    for pat in BIND_PATTERNS:
        m = pat.search(line)
        if m:
            return m.group(1)
    return None

def collect_doc_block(lines: list[str], idx: int):
    docs_rev = []
    j = idx - 1
    while j >= 0:
        cm = COMMENT_RE.match(lines[j])
        if not cm:
            break
        text = cm.group(1).strip()
        if (
            text.startswith("Module ")
            or text.startswith("End Module")
            or text.startswith("Object ")
            or text.startswith("End Object")
            or text.startswith("Field ")
        ):
            break
        if text != "":
            docs_rev.append(text)
        j -= 1
    docs_rev.reverse()
    return docs_rev

def parse_doc_triple(docs: list[str]):
    desc = ""
    params = []
    ret = "void"
    if not docs:
        return desc, params, ret

    desc = docs[0]

    for d in reversed(docs[1:]):
        m = RET_LINE_RE.match(d)
        if m:
            ret = m.group(1).strip()
            break

    for d in docs[1:]:
        if RET_LINE_RE.match(d):
            continue
        params = parse_params(d)
        break

    params = normalize_varargs(params)
    return desc, params, ret

def add_field(owner: str, ftype: str, fname: str, fields_map: dict):
    owner_fields = fields_map.setdefault(owner, {})
    owner_fields[fname] = ftype

def parse_cpp(text: str, kind: str, emitted_tables: set, emitted_obj_classes: set, out_lines: list[str], fields_map: dict):
    lines = text.splitlines()
    owner = None
    had = False
    collected_funcs = []

    for i, line in enumerate(lines):
        if kind == "module":
            m = MODULE_RE.match(line)
            if m:
                owner = m.group(1).strip()
                had = True
                ensure_tables(owner, emitted_tables, out_lines)
                out_lines.append("")
                continue
            if ENDMODULE_RE.match(line):
                owner = None
                continue
        else:
            m = OBJECT_RE.match(line)
            if m:
                owner = m.group(1).strip()
                had = True
                ensure_object_class(owner, emitted_obj_classes, out_lines)
                ensure_tables(owner, emitted_tables, out_lines)
                out_lines.append("")
                continue
            if ENDOBJECT_RE.match(line):
                owner = None
                continue

        if not owner:
            continue

        fm = FIELD_RE.match(line)
        if fm:
            had = True
            ftype = fm.group(1)
            fname = fm.group(2)
            add_field(owner, ftype, fname, fields_map)
            continue

        name = extract_binding_name(line)
        if not name:
            continue

        docs = collect_doc_block(lines, i)
        if not docs:
            continue

        desc, params, ret = parse_doc_triple(docs)
        collected_funcs.append(f"{owner}.{name}")

        if desc:
            out_lines.append(f"--- {desc}")
        for pname, ptype in params:
            out_lines.append(f"---@param {pname} {ptype}")
        out_lines.append(f"---@return {ret}")

        sep = ":" if kind == "object" else "."
        arglist = ", ".join(pname for pname, _ in params)
        out_lines.append(f"function {owner}{sep}{name}({arglist}) end")
        out_lines.append("")
        had = True

    return had, collected_funcs

def scan(root: Path, kind: str, base: Path, emitted_tables: set, emitted_obj_classes: set, out_lines: list[str], fields_map: dict):
    if not root.exists():
        return
    for cpp in sorted(root.rglob("*.cpp")):
        rel = cpp.relative_to(base)
        text = cpp.read_text(encoding="utf-8", errors="ignore")
        had, funcs = parse_cpp(text, kind, emitted_tables, emitted_obj_classes, out_lines, fields_map)
        if had and funcs:
            print(f"Writing file {rel} completions...")
            for f in funcs:
                print(f"  {f}")

def build_lime_header(fields_map: dict):
    lines = ["---@class Lime"]
    for fname, ftype in sorted(fields_map.get("Lime", {}).items()):
        lines.append(f"---@field {fname} {ftype}")
    lines.append("Lime = Lime or {}")
    return lines

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--src", required=True)
    ap.add_argument("--out", required=True)
    args = ap.parse_args()

    src = Path(args.src).resolve()
    out_dir = Path(args.out).resolve()
    out_dir.mkdir(parents=True, exist_ok=True)

    body_lines: list[str] = []
    emitted_tables = set()
    emitted_obj_classes = set()
    fields_map = {}  # owner -> {fieldName: fieldType}

    scan(src / "Modules", "module", src, emitted_tables, emitted_obj_classes, body_lines, fields_map)
    scan(src / "Objects", "object", src, emitted_tables, emitted_obj_classes, body_lines, fields_map)

    header_lines = build_lime_header(fields_map)

    # Also inject object/module fields where the owner isn't Lime by emitting @field in their class/table area.
    # For objects, we already emit their class before methods; we will append @field blocks near the end for simplicity.
    extra_field_blocks = []
    for owner, fields in sorted(fields_map.items()):
        if owner == "Lime":
            continue
        extra_field_blocks.append(f"---@class {owner}")
        for fname, ftype in sorted(fields.items()):
            extra_field_blocks.append(f"---@field {fname} {ftype}")
        extra_field_blocks.append("")

    out_path = out_dir / "Lime.lua"
    out_path.write_text("\n".join(header_lines + extra_field_blocks + body_lines).rstrip(), encoding="utf-8")
    print(f"Wrote {out_path}")

if __name__ == "__main__":
    main()