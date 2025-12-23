import re
import sys
from pathlib import Path

API_EXT = ".api.inl"

TYPE_MAP = {
    "void": None,
    "int": "integer",
    "integer": "integer",
    "float": "number",
    "double": "number",
    "number": "number",
    "bool": "boolean",
    "boolean": "boolean",
    "string": "string",
}

def parse_api_file(path: Path):
    module_path = None
    functions = []
    enums = []

    lines = path.read_text().splitlines()
    i = 0
    pending_docs = []

    while i < len(lines):
        line = lines[i].strip()

        if line.startswith("LIME_MODULE"):
            module_path = re.search(r"\(([^)]+)\)", line).group(1).split(".")

        elif line.startswith("LIME_ENUM"):
            enum_name = re.search(r"LIME_ENUM\s*\(\s*(\w+)", line).group(1)
            values = {}

            i += 1
            while not lines[i].strip().startswith(")"):
                m = re.search(r"LIME_ENUM_VALUE\s*\(\s*(\w+)\s*,\s*(\d+)\s*\)", lines[i])
                if m:
                    values[m.group(1)] = int(m.group(2))
                i += 1

            enums.append((enum_name, values))

        elif line.startswith("LIME_DOC"):
            pending_docs.append(re.search(r'\("(.+)"\)', line).group(1))

        elif line.startswith("LIME_FUNC"):
            sig = line
            while not sig.endswith(")"):
                i += 1
                sig += " " + lines[i].strip()

            head = re.search(r"LIME_FUNC\s*\(\s*(\w+)\s*,\s*(\w+)", sig)
            ret_cpp = head.group(1)
            name = head.group(2)
            ret_lua = TYPE_MAP.get(ret_cpp)

            raw_args = re.findall(r"(\w+\??)\s+(\w+)", sig)
            args = []

            for t, n in raw_args:
                optional = t.endswith("?")
                base = t[:-1] if optional else t
                lua_t = TYPE_MAP.get(base) if base in TYPE_MAP else None
                args.append((base, lua_t, optional, n))

            functions.append((name, args, ret_lua, pending_docs))
            pending_docs = []

        i += 1

    return module_path, enums, functions

def emit_module(module_path, enums, functions):
    out = []

    for i in range(len(module_path)):
        full = ".".join(module_path[:i + 1])
        out.append(f"---@class {full}")
        out.append(f"{full} = {full} or {{}}")

    mod = ".".join(module_path)

    for enum_name, values in enums:
        alias = f"{mod}.{enum_name}"
        enum_table = f"{alias}Enum"
        out.append(f"---@alias {alias} integer")
        out.append(f"---@class {enum_table}")
        for k in values:
            out.append(f"---@field {k} {alias}")
        out.append(f"{alias} = {{")
        for k, v in values.items():
            out.append(f"{k} = {v},")
        out.append("}")

    for name, args, ret, docs in functions:
        for d in docs:
            out.append(f"--- {d}")
        for base, lua_t, opt, n in args:
            if lua_t:
                t = lua_t
            else:
                t = f"{mod}.{base}"
            if opt:
                t += "?"
            out.append(f"---@param {n} {t}")
        if ret:
            out.append(f"---@return {ret}")
        params = ", ".join(n for _, _, _, n in args)
        out.append(f"function {mod}.{name}({params}) end")

    return "\n".join(out)

def main():
    out_dir = Path(sys.argv[1]) if len(sys.argv) > 1 else Path("emmylua")
    out_dir.mkdir(parents=True, exist_ok=True)

    api_files = list(Path(".").rglob(f"*{API_EXT}"))
    if not api_files:
        print("No .api.inl files found")
        return

    for api in api_files:
        module_path, enums, functions = parse_api_file(api)
        lua_code = emit_module(module_path, enums, functions)
        out_path = out_dir / (api.stem.replace(".api", "") + ".lua")
        out_path.write_text(lua_code)
        print(f"Generated {out_path}")

if __name__ == "__main__":
    main()