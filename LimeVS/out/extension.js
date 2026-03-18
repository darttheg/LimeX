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
async function injectLuaLibrary(context) {
    const apiPath = getApiPath(context);
    const workspaceFolder = vscode.workspace.workspaceFolders?.[0]?.uri.fsPath;
    if (!workspaceFolder) {
        vscode.window.showWarningMessage("Lime: Open a folder to enable Lua API autocompletion.");
        return;
    }
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
    const appPath = path.join(workspaceFolder, "app.exe");
    (0, child_process_1.spawn)(appPath, [], {
        detached: true,
        stdio: "ignore",
        cwd: workspaceFolder,
    }).unref();
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
async function activate(context) {
    await injectLuaLibrary(context).catch((err) => vscode.window.showWarningMessage(`Lime: Could not configure Lua library path: ${err}`));
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
        runBat(context, "build.bat", "Lime: Build + Run");
        (0, child_process_1.exec)(`"${batPath}" "${workspaceFolder}"`, (error) => {
            if (!error) {
                setTimeout(() => launchApp(workspaceFolder), 150);
            }
            else {
                vscode.window.showErrorMessage("Lime: Build failed.");
            }
        });
    }));
}
function deactivate() { }
