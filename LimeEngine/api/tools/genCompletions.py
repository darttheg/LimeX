# genCompletions_updated_v5.py
# Key fix: preserve owner declaration order and emit blocks strictly in that order.

import re
from pathlib import Path

OWNER_RE = re.compile(r"//\s*(Module|Object)\s+([\w\.]+)")
CTOR_RE = re.compile(r"//\s*Constructor\s*(.*)")
FIELD_RE = re.compile(r"//\s*Field\s+(\w+)\s+(\w+)(?:,\s*(.*))?")
FUNC_RE = re.compile(r'bindType\["([^"]+)"\]')

def parse(path: Path):
    owners_in_order = []
    owners_seen = set()

    ctors = {}
    fields = {}
    funcs = {}

    current_owner = None

    for line in path.read_text(encoding="utf-8").splitlines():
        m = OWNER_RE.match(line)
        if m:
            kind, name = m.groups()
            current_owner = name
            if name not in owners_seen:
                owners_seen.add(name)
                owners_in_order.append(name)
            ctors.setdefault(name, [])
            fields.setdefault(name, [])
            funcs.setdefault(name, [])
            continue

        if current_owner is None:
            continue

        m = CTOR_RE.match(line)
        if m:
            ctors[current_owner].append(m.group(1).strip())
            continue

        m = FIELD_RE.match(line)
        if m:
            ftype, fname, comment = m.groups()
            fields[current_owner].append((ftype, fname))
            continue

        m = FUNC_RE.search(line)
        if m:
            funcs[current_owner].append(m.group(1))

    return owners_in_order, ctors, fields, funcs

def emit(out: Path, owners, ctors, fields, funcs):
    lines = []

    for owner in owners:
        # class
        lines.append(f"---@class {owner}")

        # ctors
        for sig in ctors.get(owner, []):
            if not sig:
                lines.append(f"---@overload fun(): {owner}")
            else:
                args = []
                for p in sig.split(","):
                    t, n = p.strip().split()
                    args.append(f"{n}:{t}")
                lines.append(f"---@overload fun({', '.join(args)}): {owner}")

        # fields
        for t, n in fields.get(owner, []):
            lines.append(f"---@field {n} {t}")

        # table stub (ALWAYS)
        lines.append(f"{owner} = {owner} or {{}}")
        lines.append("")

    out.write_text("\n".join(lines).rstrip() + "\n", encoding="utf-8")

if __name__ == "__main__":
    import sys
    src = Path(sys.argv[1])
    dst = Path(sys.argv[2])

    owners, ctors, fields, funcs = parse(src)
    emit(dst, owners, ctors, fields, funcs)
