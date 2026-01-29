import argparse
import re
from dataclasses import dataclass, field
from pathlib import Path
from typing import Dict, List, Optional, Tuple

KNOWN_TYPES = {"string","number","boolean","table","any","nil","void","function","userdata","thread"}
ARTICLES = {"the","a","an"}

@dataclass
class Param:
    name: str
    typ: str

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
    fields: List[Tuple[str, str, Optional[str]]] = field(default_factory=list)

@dataclass
class ObjectDoc:
    name: str
    fields: List[Tuple[str, str, Optional[str]]] = field(default_factory=list)
    ctors: List[List[Param]] = field(default_factory=list)

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
        if not looks_like_type(typ):
            return False
    return True

def parse_field_decl(s: str) -> Optional[Tuple[str, str, Optional[str]]]:
    rest = s.strip()
    comment = None
    if "," in rest:
        before, after = rest.split(",", 1)
        rest = before.strip()
        comment = after.strip() or None
    tokens = rest.split()
    if len(tokens) < 2:
        return None
    return (tokens[1].strip(), tokens[0].strip(), comment)

def sig_key(params: List[Param]) -> Tuple[Tuple[str, str], ...]:
    return tuple((p.name, p.typ) for p in params)

def parse_cpp_files(src: Path) -> Tuple[Dict[str, ModuleDoc], List[ObjectDoc], List[FunctionDoc]]:
    modules: Dict[str, ModuleDoc] = {}
    objects_by_name: Dict[str, ObjectDoc] = {}
    functions: List[FunctionDoc] = []

    current_module: Optional[str] = None
    current_object: Optional[ObjectDoc] = None

    pending_doc: List[str] = []
    pending_overloads: List[List[Param]] = []
    pending_returns: Optional[str] = None

    comment_re = re.compile(r'^\s*//\s?(.*)$')
    bind_func_re = re.compile(r'\.set_function\s*\(\s*"([^"]+)"')
    module_field_assign_re = re.compile(r'\bmodule\s*\[\s*"([^"]+)"\s*\]\s*=')

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
                        modules.setdefault(name, ModuleDoc(name=name))
                    pending_doc = []
                    pending_overloads = []
                    pending_returns = None
                    continue

                if s == "End Module":
                    current_module = None
                    pending_doc = []
                    pending_overloads = []
                    pending_returns = None
                    continue

                if s.startswith("Object "):
                    name = s[len("Object "):].strip()
                    if name:
                        current_object = objects_by_name.get(name) or ObjectDoc(name=name)
                        objects_by_name[name] = current_object
                    pending_doc = []
                    pending_overloads = []
                    pending_returns = None
                    continue

                if s == "End Object":
                    current_object = None
                    pending_doc = []
                    pending_overloads = []
                    pending_returns = None
                    continue

                if current_module and s.startswith("Field "):
                    decl = parse_field_decl(s[len("Field "):])
                    if decl:
                        modules[current_module].fields.append(decl)
                    continue

                if current_object and s.startswith("Field "):
                    decl = parse_field_decl(s[len("Field "):])
                    if decl:
                        current_object.fields.append(decl)
                    continue

                if current_object and s.startswith("Constructor"):
                    rest = s[len("Constructor"):].strip()
                    current_object.ctors.append(parse_params(rest) if rest else [])
                    continue

                if s.startswith("Returns "):
                    pending_returns = s[len("Returns "):].strip() or None
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
                    pending_doc = []
                    pending_overloads = []
                    pending_returns = None
                    continue

            if current_module:
                am = module_field_assign_re.search(raw)
                if am:
                    key = am.group(1).strip()
                    if key:
                        md = modules.setdefault(current_module, ModuleDoc(name=current_module))
                        if not any(n == key for (n, _, _) in md.fields):
                            md.fields.append((key, "any", None))
                    continue

    objects = [objects_by_name[k] for k in sorted(objects_by_name.keys())]
    functions.sort(key=lambda f: (f.owner, f.name, 0 if f.is_method else 1))
    return modules, objects, functions

def ctor_sig(ctor: List[Param]) -> str:
    return ", ".join([f"{p.name}:{p.typ}" for p in ctor])

def fn_sig(params: List[Param]) -> str:
    parts: List[str] = []
    for p in params:
        parts.append(f"...:{p.typ}" if p.name == "..." else f"{p.name}:{p.typ}")
    return ", ".join(parts)

def emit_lua(modules: Dict[str, ModuleDoc], objects: List[ObjectDoc], functions: List[FunctionDoc]) -> str:
    out: List[str] = []

    for mname in sorted(modules.keys()):
        md = modules[mname]
        out.append(f"---@class {mname}")
        for fname, ftyp, fcomment in md.fields:
            if fcomment:
                out.append(f"--- {fcomment}")
            out.append(f"---@field {fname} {ftyp}")
        out.append(f"{mname} = {mname} or {{}}")
        out.append("")

    for obj in objects:
        out.append(f"---@class {obj.name}")
        for name, typ, comment in obj.fields:
            out.append(f"---@field {name} {typ} {comment}" if comment else f"---@field {name} {typ}")
        out.append(f"{obj.name} = {obj.name} or {{}}")

        ctors = obj.ctors or [[]]
        seen = set()
        for ctor in ctors:
            sig = ctor_sig(ctor)
            if sig in seen:
                continue
            seen.add(sig)
            out.append(f"---@overload fun({sig}): {obj.name}" if sig else f"---@overload fun(): {obj.name}")

        base_ctor = min(ctors, key=lambda c: len(c))
        out.append(f"---@return {obj.name}")
        out.append(f"function {obj.name}.new({', '.join([p.name for p in base_ctor])}) end")
        out.append("")

    for fn in functions:
        for line in fn.doc_lines:
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

    modules, objects, functions = parse_cpp_files(src)
    (out_dir / "Lime.lua").write_text(emit_lua(modules, objects, functions), encoding="utf-8")
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
