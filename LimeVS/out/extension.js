"use strict";
var __createBinding = (this && this.__createBinding) || (Object.create ? (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    var desc = Object.getOwnPropertyDescriptor(m, k);
    if (!desc || ("get" in desc ? !m.__esModule : desc.writable || desc.configurable)) {
      desc = { enumerable: true, get: function() { return m[k]; } };
    }
    Object.defineProperty(o, k2, desc);
}) : (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    o[k2] = m[k];
}));
var __setModuleDefault = (this && this.__setModuleDefault) || (Object.create ? (function(o, v) {
    Object.defineProperty(o, "default", { enumerable: true, value: v });
}) : function(o, v) {
    o["default"] = v;
});
var __importStar = (this && this.__importStar) || (function () {
    var ownKeys = function(o) {
        ownKeys = Object.getOwnPropertyNames || function (o) {
            var ar = [];
            for (var k in o) if (Object.prototype.hasOwnProperty.call(o, k)) ar[ar.length] = k;
            return ar;
        };
        return ownKeys(o);
    };
    return function (mod) {
        if (mod && mod.__esModule) return mod;
        var result = {};
        if (mod != null) for (var k = ownKeys(mod), i = 0; i < k.length; i++) if (k[i] !== "default") __createBinding(result, mod, k[i]);
        __setModuleDefault(result, mod);
        return result;
    };
})();
Object.defineProperty(exports, "__esModule", { value: true });
exports.activate = activate;
exports.deactivate = deactivate;
const vscode = __importStar(require("vscode"));
const path = __importStar(require("path"));
const fs = __importStar(require("fs"));
const child_process_1 = require("child_process");
function getApiPath(context) {
    return path.join(context.extensionPath, "api").replace(/\\/g, "/");
}
async function injectLuaLibrary(context, workspaceFolder) {
    const apiPath = getApiPath(context);
    const emmyrcPath = path.join(workspaceFolder, ".emmyrc.json");
    let config = {};
    if (fs.existsSync(emmyrcPath)) {
        try {
            config = JSON.parse(fs.readFileSync(emmyrcPath, "utf-8"));
        }
        catch { }
    }
    config.workspace = config.workspace ?? {};
    config.workspace.library = config.workspace.library ?? [];
    if (!config.workspace.library.includes(apiPath)) {
        config.workspace.library.push(apiPath);
        fs.writeFileSync(emmyrcPath, JSON.stringify(config, null, 2), "utf-8");
    }
}
function launchApp(workspaceFolder) {
    const proc = (0, child_process_1.spawn)("cmd.exe", ["/c", "start", "", "app.exe"], {
        cwd: workspaceFolder,
        detached: true,
        stdio: "ignore",
    });
    proc.on("close", (code) => {
        if (code !== 0) {
            vscode.window.showErrorMessage(`Lime: Application exited with error (code ${code}).`);
        }
    });
}
const terminals = new Map();
function runBat(context, bat, name) {
    const batPath = path.join(context.extensionPath, "cmd", bat);
    const workspaceFolder = vscode.workspace.workspaceFolders?.[0]?.uri.fsPath ?? "";
    const existing = terminals.get(name);
    if (existing && !existing.exitStatus)
        existing.dispose();
    const terminal = vscode.window.createTerminal({
        name,
        shellPath: "cmd.exe",
        shellArgs: ["/Q", "/K", batPath, workspaceFolder],
    });
    terminals.set(name, terminal);
    terminal.show(true);
}
function loadIgnoreList(workspaceFolder) {
    const entries = new Set([
        ".limepkg",
        ".ico",
        ".exp",
        ".lib",
        ".pdb",
        ".log",
        ".emmyrc.json",
        ".vscode",
        ".ignore",
    ]);
    const ignorePath = path.join(workspaceFolder, ".ignore");
    if (!fs.existsSync(ignorePath))
        return entries;
    const lines = fs.readFileSync(ignorePath, "utf-8").split(/\r?\n/);
    for (const line of lines) {
        const trimmed = line.trim();
        if (trimmed && !trimmed.startsWith("#"))
            entries.add(trimmed);
    }
    return entries;
}
function isIgnored(fileName, ignoreList) {
    for (const entry of ignoreList) {
        if (entry.startsWith(".")) {
            if (fileName.endsWith(entry))
                return true;
        }
        else {
            if (fileName === entry)
                return true;
        }
    }
    return false;
}
function copyRecursive(src, dest, ignoreList, binFolder) {
    const entries = fs.readdirSync(src, { withFileTypes: true });
    for (const entry of entries) {
        const srcPath = path.join(src, entry.name);
        const destPath = path.join(dest, entry.name);
        if (srcPath === binFolder)
            continue;
        if (isIgnored(entry.name, ignoreList))
            continue;
        if (entry.isDirectory()) {
            fs.mkdirSync(destPath, { recursive: true });
            copyRecursive(srcPath, destPath, ignoreList, binFolder);
        }
        else {
            fs.copyFileSync(srcPath, destPath);
        }
    }
}
function copyTemplate(src, dest) {
    const entries = fs.readdirSync(src, { withFileTypes: true });
    for (const entry of entries) {
        const srcPath = path.join(src, entry.name);
        const destPath = path.join(dest, entry.name);
        if (entry.isDirectory()) {
            fs.mkdirSync(destPath, { recursive: true });
            copyTemplate(srcPath, destPath);
        }
        else {
            fs.copyFileSync(srcPath, destPath);
        }
    }
}
async function packageProject(workspaceFolder) {
    const binFolder = path.join(workspaceFolder, "bin");
    const ignoreList = loadIgnoreList(workspaceFolder);
    if (fs.existsSync(binFolder))
        fs.rmSync(binFolder, { recursive: true, force: true });
    fs.mkdirSync(binFolder);
    copyRecursive(workspaceFolder, binFolder, ignoreList, binFolder);
    vscode.window.showInformationMessage("Lime: Application packaged to bin/");
}
async function createNewProject(context) {
    const picked = await vscode.window.showOpenDialog({
        canSelectFiles: false,
        canSelectFolders: true,
        canSelectMany: false,
        title: "Create new Lime project",
        openLabel: "Select Folder",
    });
    if (!picked || picked.length === 0)
        return;
    const dest = picked[0].fsPath;
    const templatePath = path.join(context.extensionPath, "template");
    if (!fs.existsSync(templatePath)) {
        vscode.window.showErrorMessage("Lime: Template folder not found in extension.");
        return;
    }
    copyTemplate(templatePath, dest);
    await injectLuaLibrary(context, dest);
    vscode.commands.executeCommand("vscode.openFolder", vscode.Uri.file(dest), false);
}
async function activate(context) {
    const workspaceFolder = vscode.workspace.workspaceFolders?.[0]?.uri.fsPath;
    if (workspaceFolder) {
        await injectLuaLibrary(context, workspaceFolder).catch((err) => vscode.window.showWarningMessage(`Lime: Could not configure Lua library path: ${err}`));
    }
    context.subscriptions.push(vscode.window.onDidCloseTerminal((closed) => {
        for (const [key, terminal] of terminals) {
            if (terminal === closed)
                terminals.delete(key);
        }
    }), vscode.commands.registerCommand("lime.build", () => {
        runBat(context, "build.bat", "Lime: Build");
    }), vscode.commands.registerCommand("lime.buildAndRun", () => {
        const workspaceFolder = vscode.workspace.workspaceFolders?.[0]?.uri.fsPath ?? "";
        const batPath = path.join(context.extensionPath, "cmd", "build.bat");
        runBat(context, "build.bat", "Lime: Run");
        (0, child_process_1.exec)(`"${batPath}" "${workspaceFolder}"`, (error) => {
            if (!error) {
                setTimeout(() => launchApp(workspaceFolder), 300);
            }
            else {
                vscode.window.showErrorMessage("Lime: Build failed.");
            }
        });
    }), vscode.commands.registerCommand("lime.package", () => {
        const workspaceFolder = vscode.workspace.workspaceFolders?.[0]?.uri.fsPath;
        if (!workspaceFolder) {
            vscode.window.showWarningMessage("Lime: Open a folder to package a project.");
            return;
        }
        packageProject(workspaceFolder).catch((err) => vscode.window.showErrorMessage(`Lime: Packaging failed: ${err}`));
    }), vscode.commands.registerCommand("lime.newProject", () => {
        createNewProject(context).catch((err) => vscode.window.showErrorMessage(`Lime: Could not create project: ${err}`));
    }));
}
function deactivate() { }
