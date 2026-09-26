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
    const luarcPath = path.join(workspaceFolder, ".luarc.json");
    let config = {};
    if (fs.existsSync(luarcPath)) {
        try {
            config = JSON.parse(fs.readFileSync(luarcPath, "utf-8"));
        }
        catch { }
    }
    config["workspace.library"] = config["workspace.library"] ?? [];
    if (!config["workspace.library"].includes(apiPath)) {
        config["workspace.library"].push(apiPath);
        fs.writeFileSync(luarcPath, JSON.stringify(config, null, 2), "utf-8");
    }
}
function launchApp(workspaceFolder) {
    (0, child_process_1.spawn)("cmd.exe", ["/c", "start", "", "app.exe"], {
        cwd: workspaceFolder,
        windowsVerbatimArguments: true,
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
        cwd: workspaceFolder,
        shellPath: "cmd.exe",
    });
    terminals.set(name, terminal);
    terminal.show(true);
    terminal.sendText(`call "${batPath}" "${workspaceFolder}"`, true);
}
function loadIgnoreList(workspaceFolder) {
    const entries = new Set([
        ".limepkg",
        ".ico",
        ".exp",
        ".lib",
        ".pdb",
        ".log",
        ".luarc.json",
        ".vscode",
        ".ignore",
        "bin",
        ".git",
        ".gitignore",
        ".gitmodules",
        ".gitattributes",
        "bin-android",
        ".android-build",
        "android.json"
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
function copyRecursive(src, dest, ignoreList) {
    const entries = fs.readdirSync(src, { withFileTypes: true });
    for (const entry of entries) {
        if (isIgnored(entry.name, ignoreList))
            continue;
        const srcPath = path.join(src, entry.name);
        const destPath = path.join(dest, entry.name);
        if (entry.isDirectory()) {
            fs.mkdirSync(destPath, { recursive: true });
            copyRecursive(srcPath, destPath, ignoreList);
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
    copyRecursive(workspaceFolder, binFolder, ignoreList);
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
const winVersionInfo = __importStar(require("win-version-info"));
const getFileVersion = winVersionInfo.default ?? winVersionInfo;
async function checkEngineVersion(context, workspaceFolder) {
    const templateDll = path.join(context.extensionPath, "template/lib", "LimeEngine.dll");
    const candidates = [
        path.join(workspaceFolder, "LimeEngine.dll"),
        path.join(workspaceFolder, "lib", "LimeEngine.dll"),
    ];
    const projectDll = candidates.find(p => fs.existsSync(p));
    if (!projectDll) {
        await checkMissingDlls(context, workspaceFolder);
        return;
    }
    const projectVersion = getFileVersion(projectDll).ProductVersion;
    const templateVersion = getFileVersion(templateDll).ProductVersion;
    if (projectVersion === templateVersion) {
        await checkMissingDlls(context, workspaceFolder);
        return;
    }
    const choice = await vscode.window.showInformationMessage(`Lime: Update engine to ${templateVersion} (from ${projectVersion})?`, "Update", "Ignore");
    if (choice === "Update") {
        fs.copyFileSync(templateDll, projectDll);
        vscode.window.showInformationMessage("Lime: LimeEngine.dll updated successfully.");
        await checkMissingDlls(context, workspaceFolder);
    }
}
async function checkMissingDlls(context, workspaceFolder) {
    const templateDir = path.join(context.extensionPath, "template/lib");
    const templateEntries = fs.readdirSync(templateDir, { withFileTypes: true });
    const missingDlls = templateEntries
        .filter(e => e.isFile() && e.name.endsWith(".dll"))
        .map(e => e.name)
        .filter(name => {
        const inRoot = fs.existsSync(path.join(workspaceFolder, name));
        const inLib = fs.existsSync(path.join(workspaceFolder, "lib", name));
        return !(inRoot || inLib);
    });
    if (missingDlls.length === 0)
        return;
    const choice = await vscode.window.showInformationMessage(`Lime: Missing DLL(s): ${missingDlls.join(", ")}. Update?`, "Update", "Ignore");
    if (choice === "Update") {
        for (const name of missingDlls) {
            const destDir = name.startsWith("ikp") ? workspaceFolder : path.join(workspaceFolder, "lib");
            fs.mkdirSync(destDir, { recursive: true });
            fs.copyFileSync(path.join(templateDir, name), path.join(destDir, name));
        }
        vscode.window.showInformationMessage(`Lime: Added ${missingDlls.length} DLL(s).`);
    }
}
let platformItem;
function getPlatform(context) {
    return context.workspaceState.get("lime.platform", "windows");
}
function updatePlatformItem(context) {
    platformItem.text = getPlatform(context) === "android" ? "$(device-mobile) Lime: Android" : "$(device-desktop) Lime: Windows";
    platformItem.tooltip = "Select the platform Lime builds for";
}
async function selectPlatform(context) {
    const items = [
        { label: "$(device-desktop) Windows", value: "windows" },
        { label: "$(device-mobile) Android", value: "android" },
    ];
    const pick = await vscode.window.showQuickPick(items, { placeHolder: "Build for" });
    if (!pick)
        return;
    await context.workspaceState.update("lime.platform", pick.value);
    updatePlatformItem(context);
    if (pick.value === "android") {
        createAndroidJson();
        await checkAndroidPaths();
    }
}
function createAndroidJson() {
    const workspaceFolder = vscode.workspace.workspaceFolders?.[0]?.uri.fsPath;
    if (!workspaceFolder)
        return;
    const file = path.join(workspaceFolder, "android.json");
    if (fs.existsSync(file))
        return;
    const folderName = path.basename(workspaceFolder);
    let id = folderName.toLowerCase().replace(/[^a-z0-9_]/g, "_").replace(/_+/g, "_").replace(/^_|_$/g, "");
    if (!/^[a-z]/.test(id))
        id = "game" + id;
    const settings = {
        package: `com.lime.${id}`,
        name: folderName,
        version: "1.0",
        versionCode: 1,
        orientation: "landscape",
        backButton: "escape",
    };
    fs.writeFileSync(file, JSON.stringify(settings, null, 2) + "\n", "utf-8");
    vscode.window.showInformationMessage("Lime: Created android.json with the app's Android settings.");
}
async function checkAndroidPaths() {
    const cfg = vscode.workspace.getConfiguration("lime.android");
    const missing = [];
    if (!cfg.get("sdkPath") && !process.env.ANDROID_HOME)
        missing.push("Android SDK path");
    if (!cfg.get("javaHome") && !process.env.JAVA_HOME)
        missing.push("Java path");
    if (missing.length === 0)
        return;
    const choice = await vscode.window.showWarningMessage(`Lime: Building for Android needs the ${missing.join(" and ")} set in Settings.`, "Open Settings");
    if (choice === "Open Settings")
        vscode.commands.executeCommand("workbench.action.openSettings", "lime.android");
}
function androidSdk() {
    const sdk = vscode.workspace.getConfiguration("lime.android").get("sdkPath");
    return sdk || process.env.ANDROID_HOME || path.join(process.env.LOCALAPPDATA ?? "", "Android", "Sdk");
}
function androidEnv() {
    const cfg = vscode.workspace.getConfiguration("lime.android");
    const env = {};
    const sdk = cfg.get("sdkPath");
    const java = cfg.get("javaHome");
    if (sdk)
        env.ANDROID_HOME = sdk;
    if (java)
        env.JAVA_HOME = java;
    return env;
}
function listAndroidDevices() {
    const adb = path.join(androidSdk(), "platform-tools", "adb.exe");
    return new Promise((resolve) => {
        (0, child_process_1.exec)(`"${adb}" devices`, (err, stdout) => {
            if (err) {
                resolve([]);
                return;
            }
            resolve(stdout.split(/\r?\n/).slice(1).filter(l => /\tdevice$/.test(l)).map(l => l.split("\t")[0]));
        });
    });
}
async function pickAndroidDevice() {
    const devices = await listAndroidDevices();
    if (devices.length === 0) {
        vscode.window.showErrorMessage("Lime: No Android device or emulator connected.");
        return undefined;
    }
    if (devices.length === 1)
        return devices[0];
    return vscode.window.showQuickPick(devices, { placeHolder: "Run on" });
}
// args go in as-is so switches like -RunOnly stay switches
function runAndroid(context, name, args) {
    const script = path.join(context.extensionPath, "android", "buildApk.ps1");
    const workspaceFolder = vscode.workspace.workspaceFolders?.[0]?.uri.fsPath ?? "";
    const existing = terminals.get(name);
    if (existing && !existing.exitStatus)
        existing.dispose();
    const terminal = vscode.window.createTerminal({
        name,
        cwd: workspaceFolder,
        shellPath: "powershell.exe",
        shellArgs: ["-NoProfile", "-ExecutionPolicy", "Bypass"],
        env: androidEnv(),
    });
    terminals.set(name, terminal);
    terminal.show(true);
    terminal.sendText(`& '${script}' -Project '${workspaceFolder.replace(/'/g, "''")}' ${args}`, true);
}
const logLinkProvider = {
    provideTerminalLinks(ctx) {
        const match = /Details in "([^"]+)"/.exec(ctx.line);
        if (!match)
            return [];
        const start = match.index + match[0].indexOf('"');
        return [{ startIndex: start, length: match[1].length + 2, tooltip: "Open build.log", file: match[1] }];
    },
    handleTerminalLink(link) {
        vscode.window.showTextDocument(vscode.Uri.file(link.file));
    },
};
async function activate(context) {
    const workspaceFolder = vscode.workspace.workspaceFolders?.[0]?.uri.fsPath;
    if (workspaceFolder) {
        await injectLuaLibrary(context, workspaceFolder).catch((err) => vscode.window.showWarningMessage(`Lime: Could not configure Lua library path: ${err}`));
        await checkEngineVersion(context, workspaceFolder).catch((err) => vscode.window.showWarningMessage(`Lime: Could not check engine version: ${err}`));
    }
    platformItem = vscode.window.createStatusBarItem(vscode.StatusBarAlignment.Left, 100);
    platformItem.command = "lime.selectPlatform";
    updatePlatformItem(context);
    if (workspaceFolder)
        platformItem.show();
    context.subscriptions.push(platformItem);
    context.subscriptions.push(vscode.window.registerTerminalLinkProvider(logLinkProvider));
    context.subscriptions.push(vscode.window.onDidCloseTerminal((closed) => {
        for (const [key, terminal] of terminals) {
            if (terminal === closed)
                terminals.delete(key);
        }
    }), vscode.commands.registerCommand("lime.build", () => {
        if (getPlatform(context) === "android") {
            runAndroid(context, "Lime: Build", "");
            return;
        }
        runBat(context, "build.bat", "Lime: Build");
    }), vscode.commands.registerCommand("lime.run", async () => {
        if (getPlatform(context) === "android") {
            const device = await pickAndroidDevice();
            if (device)
                runAndroid(context, "Lime: Run", `-RunOnly -Device '${device}'`);
            return;
        }
        const workspaceFolder = vscode.workspace.workspaceFolders?.[0]?.uri.fsPath ?? "";
        try {
            launchApp(workspaceFolder);
        }
        catch (e) {
            vscode.window.showErrorMessage("Lime: Run failed.");
        }
    }), vscode.commands.registerCommand("lime.selectPlatform", () => selectPlatform(context)), vscode.commands.registerCommand("lime.package", () => {
        if (getPlatform(context) === "android") {
            runAndroid(context, "Lime: Package", "");
            return;
        }
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
