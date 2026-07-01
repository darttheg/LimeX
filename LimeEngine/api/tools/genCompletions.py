import re
import sys
import argparse
from pathlib import Path
from dataclasses import dataclass
from typing import Optional

DOC_TAGS = {
    "+": "**This function cannot be run until window creation.**",
    "-": "**This function can only be run before window creation.**",
    "x": "**DEPRECATED**",
    "s": "**This function can only be run by a server host.**",
    "p": "**This function can only be run by a peer of a server.**",
}
TYPE_MAP = {
    "void": "nil", "boolean": "boolean", "bool": "boolean",
    "number": "number", "int": "integer", "float": "number",
    "double": "number", "string": "string", "table": "table",
    "nil": "nil", "function": "fun(...)",
}

def lua_type(t: str) -> str:
    optional = t.endswith("?")
    base = t.rstrip("?")
    return TYPE_MAP.get(base, base) + ("?" if optional else "")

@dataclass
class ParamSet:
    params: list

@dataclass
class FunctionEntry:
    name: str
    description: str
    doc_tag: Optional[str]
    param_sets: list
    returns: list
    is_constructor: bool = False

@dataclass
class FieldEntry:
    name: str
    ftype: str
    comment: str
    params: list = None

@dataclass
class OperationEntry:
    ret: str
    operand: str
    op: str

@dataclass
class ObjectEntry:
    name: str
    comment: str
    inherits: list
    fields: list
    constructors: list
    methods: list
    operations: list

@dataclass
class ModuleEntry:
    name: str
    append: bool
    functions: list
    fields: list

def strip_comment(line: str) -> Optional[str]:
    s = line.strip()
    if s.startswith("//"):
        return s[2:].strip()
    return None

def parse_params(raw: str) -> ParamSet:
    params = []
    for part in raw.split(","):
        part = part.strip()
        if not part:
            continue
        tokens = part.split()
        if len(tokens) >= 2:
            params.append((tokens[0], tokens[1]))
        elif len(tokens) == 1:
            params.append((tokens[0], "arg"))
    return ParamSet(params)

def parse_returns(raw: str) -> list:
    return [t.strip() for t in raw.split(",") if t.strip()]

TAG_RE = re.compile(r"^\[([+\-xsp])\]\s*")
SET_FN_RE = re.compile(r'set_function\s*\(\s*"([^"]+)"')

def strip_tag(text: str):
    m = TAG_RE.match(text)
    if m:
        return m.group(1), text[m.end():].strip()
    return None, text

def parse_file(path: str):
    with open(path, encoding="utf-8") as f:
        lines = f.readlines()
    modules = []
    objects = []
    cur_module = None
    cur_object = None
    desc_lines = []
    doc_tag = None
    param_sets = []
    returns = []
    pending_field = []

    def flush():
        nonlocal desc_lines, doc_tag, param_sets, returns
        desc_lines = []; doc_tag = None; param_sets = []; returns = []

    def commit_pending_field():
        nonlocal pending_field
        for fe in pending_field:
            if cur_object is not None:
                cur_object.fields.append(fe)
            elif cur_module is not None:
                cur_module.fields.append(fe)
        pending_field = []

    def commit_function(name: str):
        commit_pending_field()
        fn = FunctionEntry(
            name=name,
            description=" ".join(desc_lines).strip(),
            doc_tag=doc_tag,
            param_sets=list(param_sets),
            returns=list(returns),
        )
        if cur_object is not None:
            cur_object.methods.append(fn)
        elif cur_module is not None:
            cur_module.functions.append(fn)
        flush()

    for raw_line in lines:
        cmt = strip_comment(raw_line)
        if cmt is None:
            m = SET_FN_RE.search(raw_line)
            if m and (cur_module is not None or cur_object is not None):
                commit_function(m.group(1))
            elif re.search(r'\["[^"]+"]\s*=', raw_line):
                commit_pending_field()
            continue
        if cmt.startswith("Module "):
            flush()
            rest = cmt[7:].strip()
            append = False
            if rest.endswith(", append"):
                rest = rest[:-8].strip()
                append = True
            cur_module = ModuleEntry(name=rest, append=append, functions=[], fields=[])
            cur_object = None
            modules.append(cur_module)
            continue
        if cmt.startswith("End Module"):
            commit_pending_field(); flush(); cur_module = None
            continue
        if cmt.startswith("Interface "):
            flush()
            name = cmt[10:].strip()
            cur_object = ObjectEntry(name=name, comment="", inherits=[], fields=[], constructors=[], methods=[], operations=[])
            cur_object._is_interface = True
            objects.append(cur_object)
            continue
        if cmt.startswith("Object "):
            flush()
            rest = cmt[7:].strip()
            obj_comment = ""
            if "," in rest:
                idx = rest.index(",")
                obj_comment = rest[idx+1:].strip()
                rest = rest[:idx].strip()
            cur_object = ObjectEntry(name=rest, comment=obj_comment, inherits=[], fields=[], constructors=[], methods=[], operations=[])
            objects.append(cur_object)
            continue
        if cmt.startswith("End Object") or cmt.startswith("End Interface"):
            flush(); cur_object = None
            continue
        if cmt.startswith("Inherits ") and cur_object is not None:
            cur_object.inherits.extend(i.strip() for i in cmt[9:].split(",") if i.strip())
            continue
        if cmt.startswith("Field ") and (cur_object is not None or cur_module is not None):
            commit_pending_field()
            rest = cmt[6:].strip()
            first_comma = rest.find(",")
            if first_comma != -1:
                before = rest[:first_comma].strip()
                fcomment = rest[first_comma+1:].strip()
            else:
                before = rest.strip()
                fcomment = ""
            tokens = before.split()
            ftype = tokens[0] if tokens else "any"
            fname = tokens[1] if len(tokens) > 1 else "field"
            pending_field.append(FieldEntry(name=fname, ftype=ftype, comment=fcomment))
            flush()
            continue
        if cmt.startswith("Operation ") and cur_object is not None:
            parts = cmt[10:].strip().split()
            if len(parts) == 3:
                cur_object.operations.append(OperationEntry(ret=parts[0], operand=parts[1], op=parts[2]))
            flush()
            continue
        if cmt.startswith("Constructor"):
            rest = cmt[11:].strip()
            if cur_object is not None:
                cur_object.constructors.append(parse_params(rest) if rest else ParamSet([]))
            flush()
            continue
        if cmt.startswith("Params "):
            if pending_field and not desc_lines and not param_sets and not returns:
                if pending_field[-1].params is None:
                    pending_field[-1].params = []
                pending_field[-1].params.append(parse_params(cmt[7:].strip()))
            else:
                commit_pending_field()
                param_sets.append(parse_params(cmt[7:].strip()))
            continue
        if cmt.startswith("Returns "):
            rest = cmt[8:].strip()
            tokens = [t.strip() for t in rest.split(",") if t.strip()]
            if tokens and all(" " not in t for t in tokens):
                returns.append(parse_returns(rest))
                continue
        tag, text = strip_tag(cmt)
        if tag:
            doc_tag = tag
        if text:
            desc_lines.append(text)
    return modules, objects

def emit_type(t: str) -> str:
    return lua_type(t)

def emit_param_list(ps: ParamSet) -> str:
    return ", ".join(p[1] for p in ps.params)

def emit_function(fn: FunctionEntry, parent: str, indent: str = "", is_method: bool = False) -> list:
    out = []
    desc = fn.description
    if fn.doc_tag and fn.doc_tag in DOC_TAGS:
        note = DOC_TAGS[fn.doc_tag]
        desc = f"{note} {desc}".strip() if desc else note
    if desc:
        out.append(f"{indent}--- {desc}")

    returns_list = fn.returns
    if returns_list and not isinstance(returns_list[0], list):
        returns_list = [returns_list]

    def ret_str_for(rset):
        parts = [emit_type(r) for r in rset if r.lower() not in ("void", "nil")]
        return ", ".join(parts) if parts else "nil"

    if len(returns_list) > 1:
        types_seen = []
        for rset in returns_list:
            for r in rset:
                t = emit_type(r)
                if t.lower() not in ("nil", "void") and t not in types_seen:
                    types_seen.append(t)
        ret_parts = types_seen
    else:
        rset = returns_list[0] if returns_list else []
        ret_parts = [emit_type(r) for r in rset if r.lower() not in ("void", "nil")]

    ret_str = "|".join(ret_parts) if len(returns_list) > 1 else (", ".join(ret_parts) if ret_parts else "nil")

    def fmt_param(pt, pn):
        return f"... {emit_type(pt)}" if pn == "..." else f"{pn}: {emit_type(pt)}"

    def emit_param_annotation(pt, pn):
        return (indent + "--- @param ... " + emit_type(pt)) if pn == "..." else (indent + "--- @param " + pn + " " + emit_type(pt))

    if len(fn.param_sets) > 1:
        primary = max(fn.param_sets, key=lambda ps: len(ps.params))
        primary_idx = fn.param_sets.index(primary)
        max_len = len(primary.params)
        all_same_len = all(len(ps.params) == max_len for ps in fn.param_sets)

        def get_ret(idx):
            if idx < len(returns_list):
                return ret_str_for(returns_list[idx])
            return ret_str

        if all_same_len:
            ret_sets = [ret_str_for(returns_list[i]) if i < len(returns_list) else ret_str for i in range(len(fn.param_sets))]
            returns_differ = len(set(ret_sets)) > 1
            if returns_differ:
                self_prefix = "self, " if is_method else ""
                for i, ps in enumerate(fn.param_sets):
                    if ps is primary:
                        continue
                    param_str = ", ".join(fmt_param(pt, pn) for pt, pn in ps.params)
                    out.append(indent + "--- @overload fun(" + self_prefix + param_str + "): " + ret_sets[i])
                for ptype, pname in primary.params:
                    out.append(emit_param_annotation(ptype, pname))
            else:
                union_params = []
                for i in range(max_len):
                    pname = primary.params[i][1]
                    types_seen = []
                    for ps in fn.param_sets:
                        t = emit_type(ps.params[i][0])
                        if t not in types_seen:
                            types_seen.append(t)
                    union_params.append(("|".join(types_seen), pname))
                for ptype, pname in union_params:
                    out.append(emit_param_annotation(ptype, pname))
        else:
            def is_prefix(short_ps, long_ps):
                for i, (pt, pn) in enumerate(short_ps.params):
                    if emit_type(pt) != emit_type(long_ps.params[i][0]):
                        return False
                return True
            others = [(i, ps) for i, ps in enumerate(fn.param_sets) if ps is not primary]
            if all(is_prefix(ps, primary) for _, ps in others):
                shortest = min(len(ps.params) for _, ps in others)
                for i, (ptype, pname) in enumerate(primary.params):
                    t = emit_type(ptype)
                    if i >= shortest:
                        t = t.rstrip("?") + "?"
                    out.append(emit_param_annotation(t, pname))
            else:
                self_prefix = "self, " if is_method else ""
                for i, ps in enumerate(fn.param_sets):
                    if ps is primary:
                        continue
                    param_str = ", ".join(fmt_param(pt, pn) for pt, pn in ps.params)
                    out.append(indent + "--- @overload fun(" + self_prefix + param_str + "): " + get_ret(i))
                for ptype, pname in primary.params:
                    out.append(emit_param_annotation(ptype, pname))
    else:
        primary = fn.param_sets[0] if fn.param_sets else ParamSet([])
        for ptype, pname in primary.params:
            out.append(emit_param_annotation(ptype, pname))

    if ret_parts:
        if len(returns_list) > 1:
            out.append(indent + "--- @return " + "|".join(ret_parts))
        else:
            out.append(indent + "--- @return " + ", ".join(ret_parts))

    param_names = ", ".join("..." if pn == "..." else pn for _, pn in primary.params)
    sep = ":" if is_method else "."
    out.append(f"{indent}function {parent}{sep}{fn.name}({param_names}) end")
    out.append("")
    return out

def make_event_stub(alias: str, param_str: str) -> str:
    s  = f"--- @class {alias}\n"
    s += f"{alias} = {{}}\n"
    s += f"--- @param Function fun({param_str})\n"
    s += f"--- @return Hook\n"
    s += f"function {alias}:hook(Function) end\n"
    s += f"function {alias}:clear() end\n"
    s += f"--- @param ... any\n"
    s += f"function {alias}:run(...) end\n"
    s += f"--- @return number\n"
    s += f"function {alias}:length() end\n"
    return s

def emit_module(mod: ModuleEntry, declared: set = None) -> list:
    if declared is None:
        declared = set()
    out = []
    parts = mod.name.split(".")
    for i, part in enumerate(parts):
        path = ".".join(parts[:i+1])
        is_leaf = (i == len(parts) - 1)
        already = path in declared
        if not already:
            declared.add(path)
        if is_leaf:
            out.append(f"--- @class {path}")
            for f in mod.fields:
                ftype = emit_type(f.ftype)
                if ftype == "Event":
                    alias = f"{mod.name.replace('.', '_')}_{f.name}"
                    param_str = ", ".join(f"{pn}: {emit_type(pt)}" for pt, pn in f.params[0].params) if f.params else ""
                    ftype = alias
                    out.insert(0, make_event_stub(alias, param_str))
                line = f"--- @field {f.name} {ftype}"
                if f.comment:
                    line += f" {f.comment}"
                out.append(line)
            if i == 0:
                out.append(f"{part} = {{}}")
                if path == "Lime":
                    out.append("Lime.Enum = {}")
            else:
                out.append(f"{'.'.join(parts[:i])}.{part} = {{}}")
        elif not already:
            if i == 0:
                out.append(f"{part} = {{}}")
            else:
                out.append(f"{'.'.join(parts[:i])}.{part} = {{}}")
    out.append("")
    for fn in mod.functions:
        out.extend(emit_function(fn, mod.name))
    return out

def emit_object(obj: ObjectEntry) -> list:
    out = []
    if obj.comment:
        out.append(f"--- {obj.comment}")
    class_line = f"--- @class {obj.name}"
    if obj.inherits:
        class_line += " : " + ", ".join(obj.inherits)
    out.append(class_line)
    op_insert_idx = len(out) - 1
    for f in obj.fields:
        ftype = emit_type(f.ftype)
        if ftype == "Event":
            alias = f"{obj.name}_{f.name}"
            param_str = ", ".join(f"{pn}: {emit_type(pt)}" for pt, pn in f.params[0].params) if f.params else ""
            ftype = alias
            out.insert(0, make_event_stub(alias, param_str))
        line = f"--- @field {f.name} {ftype}"
        if f.comment:
            line += f" {f.comment}"
        out.append(line)
    out.append(f"{obj.name} = {{}}")
    out.append("")
    if obj.constructors:
        primary = obj.constructors[0]
        for ps in obj.constructors[1:]:
            param_str = ", ".join(f"{pn}: {emit_type(pt)}" for pt, pn in ps.params)
            out.append(f"--- @overload fun({param_str}): {obj.name}")
        for pt, pn in primary.params:
            out.append(f"--- @param {pn} {emit_type(pt)}")
        out.append(f"--- @return {obj.name}")
        out.append(f"function {obj.name}.new({emit_param_list(primary)}) end")
        out.append("")
    OP_KEYWORD = {"==": "eq", "+": "add", "-": "sub", "*": "mul", "/": "div", "<": "lt", "<=": "le"}
    for n, op in enumerate(obj.operations):
        out.insert(op_insert_idx + 1 + n, f"--- @operator {OP_KEYWORD.get(op.op, op.op)}({emit_type(op.operand)}): {emit_type(op.ret)}")
    for fn in obj.methods:
        out.extend(emit_function(fn, obj.name, is_method=True))
    return out

def merge_modules(modules) -> list:
    merged: dict = {}
    order = []
    for mod in modules:
        if mod.name in merged:
            merged[mod.name].fields.extend(mod.fields)
            merged[mod.name].functions.extend(mod.functions)
        else:
            merged[mod.name] = ModuleEntry(name=mod.name, append=mod.append, functions=list(mod.functions), fields=list(mod.fields))
            order.append(mod.name)
    return [merged[n] for n in order]

def generate(modules, objects) -> str:
    lines = ["---@meta", ""]
    declared_tables: set = set()
    for mod in merge_modules(modules):
        lines.extend(emit_module(mod, declared_tables))

    interfaces = {obj.name: obj for obj in objects if getattr(obj, "_is_interface", False)}

    for obj in objects:
        if getattr(obj, "_is_interface", False):
            continue
        for iface_name in obj.inherits:
            if iface_name in interfaces:
                iface = interfaces[iface_name]
                existing_names = {m.name for m in obj.methods}
                for m in iface.methods:
                    if m.name not in existing_names:
                        obj.methods.append(m)
                existing_fields = {f.name for f in obj.fields}
                for f in iface.fields:
                    if f.name not in existing_fields:
                        obj.fields.append(f)
        obj.inherits = [i for i in obj.inherits if i not in interfaces]
        lines.extend(emit_object(obj))
    return "\n".join(lines)

FOLDER_ORDER = ["Interfaces", "Objects", "Modules"]

def collect_files(src: Path) -> list:
    ordered = []
    seen = set()
    for folder in FOLDER_ORDER:
        d = src / folder
        if d.is_dir():
            for f in sorted(d.rglob("*.cpp")):
                if f not in seen:
                    ordered.append(f)
                    seen.add(f)
    for f in sorted(src.rglob("*.cpp")):
        if f not in seen:
            ordered.append(f)
            seen.add(f)
    return ordered

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--src", required=True, metavar="DIR")
    ap.add_argument("--out", required=True, metavar="DIR")
    args = ap.parse_args()
    src = Path(args.src)
    out = Path(args.out)
    if not src.is_dir():
        print(f"error: --src '{src}' is not a directory")
        sys.exit(1)
    cpp_files = collect_files(src)
    all_modules = []
    all_objects = []
    for path in cpp_files:
        mods, objs = parse_file(str(path))
        all_modules.extend(mods)
        all_objects.extend(objs)
        if mods or objs:
            print(f" + {path.relative_to(src)}: {len(objs)} objects, {len(mods)} modules")
    out.mkdir(parents=True, exist_ok=True)
    dest = out / "Lime.lua"
    dest.write_text(generate(all_modules, all_objects), encoding="utf-8")
    print(f"Compiled Lime.lua with {len(all_objects)} objects and {len(all_modules)} modules")

if __name__ == "__main__":
    main()