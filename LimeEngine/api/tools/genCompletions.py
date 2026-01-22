#!/usr/bin/env python3
import argparse
import re
from pathlib import Path

MODULE_RE    = re.compile(r'^\s*//\s*Module\s+(.+?)\s*$')
ENDMODULE_RE = re.compile(r'^\s*//\s*End\s+Module\s*$')
OBJECT_RE    = re.compile(r'^\s*//\s*Object\s+(.+?)\s*$')
ENDOBJECT_RE = re.compile(r'^\s*//\s*End\s+Object\s*$')
FIELD_RE     = re.compile(r'^\s*//\s*Field\s+(\S+)\s+([A-Za-z_]\w*)\s*(?:,\s*(.*?))?\s*$')
CTOR_RE      = re.compile(r'^\s*//\s*Constructor\s*(.*?)\s*$')
COMMENT_RE   = re.compile(r'^\s*//\s*(.*?)\s*$')
RET_LINE_RE  = re.compile(r'^\s*Returns\s+(.+?)\s*$')

BIND_PATTERNS = [
    re.compile(r'\bmodule\.set_function\s*\(\s*\"([^\"]+)\"'),
    re.compile(r'\bmodule\[\s*\"([^\"]+)\"\s*\]\s*='),
    re.compile(r'\bbindType\[\s*\"([^\"]+)\"\s*\]\s*='),
    re.compile(r'\bbindType\.set_function\s*\(\s*\"([^\"]+)\"'),
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
        out.append((name, typ))
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
            or text.startswith("Constructor")
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

def add_field(owner: str, ftype: str, fname: str, fcomment: str | None, fields_map: dict):
    owner_fields = fields_map.setdefault(owner, {})
    owner_fields[fname] = (ftype, (fcomment or "").strip())

def add_ctor(owner: str, params: list[tuple[str, str]], ctors_map: dict):
    if not owner:
        return
    owner_ctors = ctors_map.setdefault(owner, [])
    owner_ctors.append(params)

def bump_stats(stats_map: dict, kind: str, owner: str, key: str, inc: int = 1):
    if not owner:
        return
    st = stats_map.setdefault((kind, owner), {"constructors": 0, "fields": 0, "functions": 0})
    st[key] += inc

def parse_cpp(
    text: str,
    kind: str,
    emitted_tables: set,
    emitted_obj_classes: set,
    out_lines: list[str],
    fields_map: dict,
    ctors_map: dict,
    stats_map: dict,
):
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
            fcomment = fm.group(3)
            add_field(owner, ftype, fname, fcomment, fields_map)
            bump_stats(stats_map, kind, owner, "fields", 1)
            continue

        cm = CTOR_RE.match(line)
        if cm:
            had = True
            raw = (cm.group(1) or "").strip()
            params = normalize_varargs(parse_params(raw))
            add_ctor(owner, params, ctors_map)
            bump_stats(stats_map, kind, owner, "constructors", 1)
            continue

        name = extract_binding_name(line)
        if not name:
            continue

        docs = collect_doc_block(lines, i)
        if not docs:
            continue

        desc, params, ret = parse_doc_triple(docs)
        collected_funcs.append(f"{owner}.{name}")
        bump_stats(stats_map, kind, owner, "functions", 1)

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

def scan(
    root: Path,
    kind: str,
    base: Path,
    emitted_tables: set,
    emitted_obj_classes: set,
    out_lines: list[str],
    fields_map: dict,
    ctors_map: dict,
    stats_map: dict,
):
    if not root.exists():
        return
    for cpp in sorted(root.rglob("*.cpp")):
        text = cpp.read_text(encoding="utf-8", errors="ignore")
        parse_cpp(text, kind, emitted_tables, emitted_obj_classes, out_lines, fields_map, ctors_map, stats_map)

def emit_owner_block(owner: str, fields: dict, ctors: list[list[tuple[str, str]]] | None):
    lines = [f"---@class {owner}"]
    if ctors:
        for params in ctors:
            arglist = ", ".join(f"{n}:{t}" for n, t in params)
            # This enables `Owner(...)` autocomplete (instead of `Owner.new(...)`)
            lines.append(f"---@overload fun({arglist}): {owner}")
    for fname, (ftype, fcomment) in sorted(fields.items()):
        if fcomment:
            lines.append(f"--- {fcomment}")
        lines.append(f"---@field {fname} {ftype}")
    lines.append("")
    return lines

def emit_debug(stats_map: dict):
    def sort_key(item):
        (kind, owner), _st = item
        return (0 if kind == "module" else 1, owner.lower())

    for (kind, owner), st in sorted(stats_map.items(), key=sort_key):
        label = "Module" if kind == "module" else "Object"
        print(f"{label} {owner} - {st['constructors']} constructors, {st['fields']} fields, {st['functions']} functions")

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
    fields_map: dict = {}
    ctors_map: dict = {}
    stats_map: dict = {}

    scan(src / "Modules", "module", src, emitted_tables, emitted_obj_classes, body_lines, fields_map, ctors_map, stats_map)
    scan(src / "Objects", "object", src, emitted_tables, emitted_obj_classes, body_lines, fields_map, ctors_map, stats_map)

    # Debug summary
    emit_debug(stats_map)

    lime_fields = fields_map.get("Lime", {})
    lime_ctors = ctors_map.get("Lime", [])

    header_lines = ["---@class Lime"]
    if lime_ctors:
        for params in lime_ctors:
            arglist = ", ".join(f"{n}:{t}" for n, t in params)
            header_lines.append(f"---@overload fun({arglist}): Lime")
    for fname, (ftype, fcomment) in sorted(lime_fields.items()):
        if fcomment:
            header_lines.append(f"--- {fcomment}")
        header_lines.append(f"---@field {fname} {ftype}")
    header_lines.append("Lime = Lime or {}")

    extra_blocks = []
    for owner, fields in sorted(fields_map.items()):
        if owner == "Lime":
            continue
        extra_blocks.extend(emit_owner_block(owner, fields, ctors_map.get(owner)))

    out_path = out_dir / "Lime.lua"
    out_path.write_text("\n".join(header_lines + extra_blocks + body_lines).rstrip(), encoding="utf-8")
    print(f"Wrote {out_path}")

if __name__ == "__main__":
    main()