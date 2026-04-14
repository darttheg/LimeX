import argparse
import re
from dataclasses import dataclass, field
from pathlib import Path
from typing import Dict, List, Optional, Tuple

KNOWN_TYPES = {"string","number","boolean","table","any","nil","void","function","userdata","thread","number[]", "string[]"}
ARTICLES = {"the","a","an"}

OP_MAP = {
    "+":  "add",
    "-":  "sub",
    "*":  "mul",
    "/":  "div",
    "==": "eq",
}

@dataclass
class Param:
    name: str
    typ: str

@dataclass
class FieldDoc:
    name: str
    typ: str
    comment: Optional[str] = None
    event_params: List[Param] = field(default_factory=list)

@dataclass
class FunctionDoc:
    owner: str
    name: str
    is_method: bool
    doc_lines: List[str] = field(default_factory=list)
    overloads: List[List[Param]] = field(default_factory=list)
    returns: Optional[str] = None

@dataclass
class ModuleDoc:
    name: str
    fields: List[FieldDoc] = field(default_factory=list)

@dataclass
class InterfaceDoc:
    name: str
    fields: List[FieldDoc] = field(default_factory=list)
    methods: List[FunctionDoc] = field(default_factory=list)

@dataclass
class ObjectDoc:
    name: str
    ctor_comment: Optional[str] = None
    inherits: List[str] = field(default_factory=list)
    fields: List[FieldDoc] = field(default_factory=list)
    ctors: List[List[Param]] = field(default_factory=list)
    operations: List[Tuple[str, str, str]] = field(default_factory=list)

def parse_params(sig: str) -> List[Param]:
    out: List[Param] = []
    for part in [p.strip() for p in sig.split(",") if p.strip()]:
        tokens = part.split()
        if len(tokens) >= 2:
            out.append(Param(name=tokens[1].strip(), typ=tokens[0].strip()))
    return out

def looks_like_type(t: str) -> bool:
    if not t:
        return False
    if t in KNOWN_TYPES:
        return True
    if "." in t or "?" in t:
        return True
    return t[0].isupper()

def looks_like_sig_type(t: str) -> bool:
    if not t:
        return False
    return t in KNOWN_TYPES or "." in t or "?" in t or t[0].isupper()

def is_param_signature(s: str) -> bool:
    s = s.strip()
    if not s or s.endswith((".", "?", "!", ":")):
        return False
    parts = [p.strip() for p in s.split(",") if p.strip()]
    if not parts:
        return False
    for p in parts:
        tokens = p.split()
        if len(tokens) < 2:
            return False
        typ, name = tokens[0], tokens[1]
        if name.lower() in ARTICLES:
            return False
        if not looks_like_sig_type(typ):
            return False
    return True

def parse_field_decl(s: str) -> Optional[FieldDoc]:
    rest = s.strip()
    comment = None
    if "," in rest:
        before, after = rest.split(",", 1)
        rest = before.strip()
        comment = after.strip() or None
    tokens = rest.split()
    if len(tokens) < 2:
        return None
    return FieldDoc(name=tokens[1].strip(), typ=tokens[0].strip(), comment=comment)

def sig_key(params: List[Param]) -> Tuple[Tuple[str, str], ...]:
    return tuple((p.name, p.typ) for p in params)

def parse_returns_line(s: str) -> Tuple[Optional[str], Optional[str]]:
    rest = s[len("Returns "):].strip()
    if not rest:
        return None, None
    tokens = rest.split()
    if len(tokens) == 1 and looks_like_type(tokens[0]):
        return tokens[0], None
    return None, rest

def parse_object_header(s: str) -> Tuple[str, Optional[str]]:
    rest = s[len("Object "):].strip()
    if not rest:
        return "", None
    if "," in rest:
        name, comment = rest.split(",", 1)
        name = name.strip()
        comment = comment.strip() or None
        return name, comment
    return rest.strip(), None

def parse_inherits_line(s: str) -> List[str]:
    m = re.match(r"^Inherits\b(.*)$", s.strip())
    if not m:
        return []
    rest = (m.group(1) or "").strip()
    if not rest:
        return []
    return [x.strip() for x in rest.split(",") if x.strip()]

def parse_cpp_files(src: Path) -> Tuple[Dict[str, ModuleDoc], Dict[str, InterfaceDoc], List[ObjectDoc], List[FunctionDoc]]:
    modules: Dict[str, ModuleDoc] = {}
    interfaces: Dict[str, InterfaceDoc] = {}
    objects_by_name: Dict[str, ObjectDoc] = {}
    functions: List[FunctionDoc] = []

    current_module: Optional[str] = None
    current_object: Optional[ObjectDoc] = None
    current_interface: Optional[InterfaceDoc] = None

    pending_doc: List[str] = []
    pending_overloads: List[List[Param]] = []
    pending_returns: Optional[str] = None
    pending_event_field: Optional[FieldDoc] = None

    comment_re = re.compile(r'^\s*//\s?(.*)$')
    bind_func_re = re.compile(r'\.set_function\s*\(\s*"([^"]+)"')
    module_field_assign_re = re.compile(r'\bmodule\s*\[\s*"([^"]+)"\s*\]\s*=')

    def reset_pending():
        nonlocal pending_doc, pending_overloads, pending_returns, pending_event_field
        pending_doc = []
        pending_overloads = []
        pending_returns = None
        pending_event_field = None

    def add_field_to_current(decl: FieldDoc):
        nonlocal pending_event_field
        if current_module:
            modules[current_module].fields.append(decl)
        elif current_interface:
            current_interface.fields.append(decl)
        elif current_object:
            current_object.fields.append(decl)
        pending_event_field = decl if decl.typ == "Event" else None

    for cpp in sorted([p for p in src.rglob("*.cpp") if p.is_file()]):
        try:
            lines = cpp.read_text(encoding="utf-8", errors="ignore").splitlines()
        except Exception:
            continue

        for raw in lines:
            m = comment_re.match(raw)
            if m:
                s = m.group(1).strip()

                if s.startswith("Module "):
                    name = s[len("Module "):].strip()
                    if name:
                        current_module = name
                        current_object = None
                        current_interface = None
                        modules.setdefault(name, ModuleDoc(name=name))
                    reset_pending()
                    continue

                if s == "End Module":
                    current_module = None
                    reset_pending()
                    continue

                if s.startswith("Interface "):
                    name = s[len("Interface "):].strip()
                    if name:
                        current_module = None
                        current_object = None
                        current_interface = interfaces.get(name) or InterfaceDoc(name=name)
                        interfaces[name] = current_interface
                    reset_pending()
                    continue

                if s == "End Interface":
                    current_interface = None
                    reset_pending()
                    continue

                if s.startswith("Object "):
                    name, comment = parse_object_header(s)
                    if name:
                        current_module = None
                        current_interface = None
                        current_object = objects_by_name.get(name) or ObjectDoc(name=name)
                        if comment:
                            current_object.ctor_comment = comment
                        objects_by_name[name] = current_object
                    reset_pending()
                    continue

                if s == "End Object":
                    current_object = None
                    reset_pending()
                    continue

                if current_object:
                    inh = parse_inherits_line(s)
                    if inh:
                        current_object.inherits = inh
                        pending_event_field = None
                        continue

                if (current_module or current_interface or current_object) and s.startswith("Field "):
                    decl = parse_field_decl(s[len("Field "):])
                    if decl:
                        add_field_to_current(decl)
                    continue

                if current_object and s.startswith("Constructor"):
                    rest = s[len("Constructor"):].strip()
                    current_object.ctors.append(parse_params(rest) if rest else [])
                    pending_event_field = None
                    continue

                if current_object and s.startswith("Operation "):
                    rest = s[len("Operation "):].strip()
                    tokens = rest.split()
                    if len(tokens) == 2:
                        rtype, sym = tokens[0], tokens[1]
                        if sym in OP_MAP:
                            current_object.operations.append((rtype, rtype, sym))
                    elif len(tokens) == 3:
                        rtype, operand, sym = tokens[0], tokens[1], tokens[2]
                        if sym in OP_MAP:
                            current_object.operations.append((rtype, operand, sym))
                    pending_event_field = None
                    continue

                if pending_event_field and s == "Params":
                    pending_event_field.event_params = []
                    continue

                if pending_event_field and s.startswith("Params "):
                    pending_event_field.event_params = parse_params(s[len("Params "):].strip())
                    continue

                if pending_event_field and is_param_signature(s):
                    pending_event_field.event_params = parse_params(s)
                    continue

                pending_event_field = None

                if s.startswith("Returns "):
                    rtype, rdoc = parse_returns_line(s)
                    if rtype:
                        pending_returns = rtype
                    elif rdoc:
                        pending_doc.append("Returns " + rdoc)
                    continue

                if s == "Params":
                    pending_overloads.append([])
                    continue

                if s.startswith("Params "):
                    pending_overloads.append(parse_params(s[len("Params "):].strip()))
                    continue

                if is_param_signature(s):
                    pending_overloads.append(parse_params(s))
                    continue

                if s:
                    pending_doc.append(s.rstrip())
                continue

            pending_event_field = None

            if current_interface:
                fm = bind_func_re.search(raw)
                if fm:
                    fname = fm.group(1).strip()
                    if fname:
                        current_interface.methods.append(FunctionDoc(
                            owner=current_interface.name,
                            name=fname,
                            is_method=True,
                            doc_lines=list(pending_doc),
                            overloads=list(pending_overloads),
                            returns=pending_returns
                        ))
                    reset_pending()
                continue

            is_method = current_object is not None
            owner = current_object.name if current_object else current_module
            if owner:
                fm = bind_func_re.search(raw)
                if fm:
                    fname = fm.group(1).strip()
                    if fname:
                        functions.append(FunctionDoc(
                            owner=owner,
                            name=fname,
                            is_method=is_method,
                            doc_lines=list(pending_doc),
                            overloads=list(pending_overloads),
                            returns=pending_returns
                        ))
                    reset_pending()
                    continue

            if current_module:
                am = module_field_assign_re.search(raw)
                if am:
                    key = am.group(1).strip()
                    if key:
                        md = modules.setdefault(current_module, ModuleDoc(name=current_module))
                        if not any(f.name == key for f in md.fields):
                            md.fields.append(FieldDoc(name=key, typ="any", comment=None))
                    continue

    objects = [objects_by_name[k] for k in sorted(objects_by_name.keys())]
    functions.sort(key=lambda f: (f.owner, f.name, 0 if f.is_method else 1))
    for idef in interfaces.values():
        idef.methods.sort(key=lambda f: f.name)
    return modules, interfaces, objects, functions

def ctor_sig(params: List[Param]) -> str:
    return ", ".join([f"{p.name}:{p.typ}" for p in params])

def fn_sig(params: List[Param]) -> str:
    parts: List[str] = []
    for p in params:
        parts.append(f"...:{p.typ}" if p.name == "..." else f"{p.name}:{p.typ}")
    return ", ".join(parts)

def callback_sig(params: List[Param]) -> str:
    parts: List[str] = []
    for p in params:
        parts.append(f"{p.name}: {p.typ}" if p.name != "..." else f"...: {p.typ}")
    return ", ".join(parts)

def sanitize_ident(s: str) -> str:
    cleaned = re.sub(r"[^0-9A-Za-z_]", "_", s)
    if cleaned and cleaned[0].isdigit():
        cleaned = "_" + cleaned
    return cleaned or "Generated"

def pascal_case(s: str) -> str:
    parts = re.split(r"[^0-9A-Za-z]+", s)
    out = "".join(part[:1].upper() + part[1:] for part in parts if part)
    if out and out[0].isdigit():
        out = "_" + out
    return out or "Generated"

def event_alias_name(owner: str, field_name: str) -> str:
    return f"{pascal_case(field_name)}Callback"

def event_class_name(owner: str, field_name: str) -> str:
    return f"{pascal_case(field_name)}Event"

def field_line(owner: str, fld: FieldDoc) -> str:
    typ = event_class_name(owner, fld.name) if fld.typ == "Event" else fld.typ
    return f"---@field {fld.name} {typ} @{fld.comment}" if fld.comment else f"---@field {fld.name} {typ}"

DOC_TAGS = {
    "+": "**This function cannot be run until window creation.**",
    "-": "**This function can only be run before window creation.**",
    "x": "**DEPRECATED**",
    "s": "**This function can only be run by a server host.**",
    "p": "**This function can only be run by a peer of a server.**"
}

def append_event_defs(out: List[str], owner: str, fields: List[FieldDoc], emitted: set) -> None:
    for fld in fields:
        if fld.typ != "Event":
            continue
        cls = event_class_name(owner, fld.name)
        if cls in emitted:
            continue
        emitted.add(cls)
        alias = event_alias_name(owner, fld.name)
        sig = callback_sig(fld.event_params)
        out.append(f"---@alias {alias} fun({sig})" if sig else f"---@alias {alias} fun()")
        out.append(f"---@class {cls} : Event")
        if fld.comment:
            out.append(f"--- {fld.comment}")
        out.append(f"{cls} = {cls} or {{}}")
        out.append(f"---@param callback {alias}")
        out.append(f"---@return Hook")
        out.append(f"function {cls}:hook(callback) end")
        out.append("")

def emit_lua(modules: Dict[str, ModuleDoc], interfaces: Dict[str, InterfaceDoc], objects: List[ObjectDoc], functions: List[FunctionDoc]) -> str:
    out: List[str] = []
    emitted_event_defs = set()

    for mname in sorted(modules.keys()):
        append_event_defs(out, mname, modules[mname].fields, emitted_event_defs)

    for iname in sorted(interfaces.keys()):
        append_event_defs(out, iname, interfaces[iname].fields, emitted_event_defs)

    for obj in objects:
        append_event_defs(out, obj.name, obj.fields, emitted_event_defs)
        for iname in obj.inherits:
            idef = interfaces.get(iname)
            if idef:
                append_event_defs(out, obj.name, idef.fields, emitted_event_defs)

    for mname in sorted(modules.keys()):
        md = modules[mname]
        out.append(f"---@class {mname}")
        for fld in md.fields:
            out.append(field_line(mname, fld))
        out.append(f"{mname} = {mname} or {{}}")
        out.append("")

    inherited_methods: List[FunctionDoc] = []
    inherited_set = set()

    for obj in objects:
        out.append(f"---@class {obj.name}")

        field_seen = set()
        for fld in obj.fields:
            field_seen.add(fld.name)
            out.append(field_line(obj.name, fld))

        for iname in obj.inherits:
            idef = interfaces.get(iname)
            if not idef:
                continue
            for fld in idef.fields:
                if fld.name in field_seen:
                    continue
                field_seen.add(fld.name)
                out.append(field_line(obj.name, fld))

        for rtype, operand, sym in obj.operations:
            out.append(f"---@operator {OP_MAP[sym]}({operand}): {rtype}")

        out.append(f"{obj.name} = {obj.name} or {{}}")

        if obj.ctors:
            if obj.ctor_comment:
                out.append(f"--- {obj.ctor_comment}")

            uniq: List[List[Param]] = []
            seenk = set()
            for c in obj.ctors:
                k = sig_key(c)
                if k in seenk:
                    continue
                seenk.add(k)
                uniq.append(c)

            base = min(uniq, key=lambda c: len(c))
            for c in uniq:
                if sig_key(c) == sig_key(base):
                    continue
                sig = ctor_sig(c)
                out.append(f"---@overload fun({sig}): {obj.name}" if sig else f"---@overload fun(): {obj.name}")

            for p in base:
                out.append(f"---@param {p.name} {p.typ}")
            out.append(f"---@return {obj.name}")
            out.append(f"function {obj.name}.new({', '.join([p.name for p in base])}) end")
            out.append("")

        for iname in obj.inherits:
            idef = interfaces.get(iname)
            if not idef:
                continue
            for m in idef.methods:
                k = (obj.name, m.name)
                if k in inherited_set:
                    continue
                inherited_set.add(k)
                inherited_methods.append(FunctionDoc(
                    owner=obj.name,
                    name=m.name,
                    is_method=True,
                    doc_lines=list(m.doc_lines),
                    overloads=list(m.overloads),
                    returns=m.returns
                ))

    direct_methods = set((f.owner, f.name) for f in functions if f.is_method)
    merged: List[FunctionDoc] = list(functions)

    for m in inherited_methods:
        if (m.owner, m.name) in direct_methods:
            continue
        merged.append(m)

    merged.sort(key=lambda f: (f.owner, f.name, 0 if f.is_method else 1))

    for fn in merged:
        emitted_tags = set()

        for line in fn.doc_lines:
            while line.startswith("["):
                end = line.find("]")
                if end == -1:
                    break

                tag = line[1:end].strip()
                line = line[end + 1:].lstrip()

                msg = DOC_TAGS.get(tag)
                if msg and tag not in emitted_tags:
                    out.append(f"--- {msg}  ")
                    emitted_tags.add(tag)

            if line:
                out.append(f"--- {line}")

        overloads = fn.overloads or [[]]
        uniq: List[List[Param]] = []
        seenk = set()
        for ov in overloads:
            k = sig_key(ov)
            if k in seenk:
                continue
            seenk.add(k)
            uniq.append(ov)

        base = min(uniq, key=lambda o: len(o))
        for ov in uniq:
            if sig_key(ov) == sig_key(base):
                continue
            sig = fn_sig(ov)
            if fn.returns:
                out.append(f"---@overload fun({sig}): {fn.returns}" if sig else f"---@overload fun(): {fn.returns}")
            else:
                out.append(f"---@overload fun({sig})" if sig else f"---@overload fun()")

        args: List[str] = []
        for p in base:
            if p.name == "...":
                out.append(f"---@param ... {p.typ}")
                args.append("...")
            else:
                out.append(f"---@param {p.name} {p.typ}")
                args.append(p.name)

        if fn.returns:
            out.append(f"---@return {fn.returns}")

        sep = ":" if fn.is_method else "."
        out.append(f"function {fn.owner}{sep}{fn.name}({', '.join(args)}) end")
        out.append("")

    while out and out[-1] == "":
        out.pop()
    return "\n".join(out) + "\n"

def main() -> int:
    ap = argparse.ArgumentParser(add_help=True)
    ap.add_argument("--src", required=True)
    ap.add_argument("--out", required=True)
    args = ap.parse_args()

    src = Path(args.src).resolve()
    out_dir = Path(args.out).resolve()
    out_dir.mkdir(parents=True, exist_ok=True)

    modules, interfaces, objects, functions = parse_cpp_files(src)
    (out_dir / "Lime.lua").write_text(emit_lua(modules, interfaces, objects, functions), encoding="utf-8")
    return 0

if __name__ == "__main__":
    raise SystemExit(main())