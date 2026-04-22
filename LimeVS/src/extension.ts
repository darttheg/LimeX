import * as vscode from "vscode";
import * as path from "path";
import * as fs from "fs";
import { spawn, exec } from "child_process";

function getApiPath(context: vscode.ExtensionContext): string {
  return path.join(context.extensionPath, "api").replace(/\\/g, "/");
}

async function injectLuaLibrary(context: vscode.ExtensionContext, workspaceFolder: string): Promise<void> {
  const apiPath = getApiPath(context);
  const emmyrcPath = path.join(workspaceFolder, ".emmyrc.json");

  let config: any = {};
  if (fs.existsSync(emmyrcPath)) {
    try {
      config = JSON.parse(fs.readFileSync(emmyrcPath, "utf-8"));
    } catch {}
  }

  config.workspace = config.workspace ?? {};
  config.workspace.library = config.workspace.library ?? [];

  if (!config.workspace.library.includes(apiPath)) {
    config.workspace.library.push(apiPath);
    fs.writeFileSync(emmyrcPath, JSON.stringify(config, null, 2), "utf-8");
  }
}

function launchApp(workspaceFolder: string): void {
  spawn("cmd.exe", ["/c", "start", "", "app.exe"], {
    cwd: workspaceFolder,
    windowsVerbatimArguments: true,
  });
}

const terminals: Map<string, vscode.Terminal> = new Map();

function runBat(context: vscode.ExtensionContext, bat: string, name: string): void {
  const batPath = path.join(context.extensionPath, "cmd", bat);
  const workspaceFolder = vscode.workspace.workspaceFolders?.[0]?.uri.fsPath ?? "";

  const existing = terminals.get(name);
  if (existing && !existing.exitStatus) existing.dispose();

  const terminal = vscode.window.createTerminal({
    name,
    cwd: workspaceFolder,
    shellPath: "cmd.exe",
  });

  terminals.set(name, terminal);
  terminal.show(true);
  terminal.sendText(`call "${batPath}" "${workspaceFolder}"`, true);
}

function loadIgnoreList(workspaceFolder: string): Set<string> {
  const entries = new Set<string>([
    ".limepkg",
    ".ico",
    ".exp",
    ".lib",
    ".pdb",
    ".log",
    ".emmyrc.json",
    ".vscode",
    ".ignore",
    "bin",
    ".git",
    ".gitignore",
    ".gitmodules",
    ".gitattributes"
  ]);

  const ignorePath = path.join(workspaceFolder, ".ignore");
  if (!fs.existsSync(ignorePath)) return entries;

  const lines = fs.readFileSync(ignorePath, "utf-8").split(/\r?\n/);
  for (const line of lines) {
    const trimmed = line.trim();
    if (trimmed && !trimmed.startsWith("#")) entries.add(trimmed);
  }

  return entries;
}

function isIgnored(fileName: string, ignoreList: Set<string>): boolean {
  for (const entry of ignoreList) {
    if (entry.startsWith(".")) {
      if (fileName.endsWith(entry)) return true;
    } else {
      if (fileName === entry) return true;
    }
  }
  return false;
}

function copyRecursive(src: string, dest: string, ignoreList: Set<string>): void {
  const entries = fs.readdirSync(src, { withFileTypes: true });

  for (const entry of entries) {
    if (isIgnored(entry.name, ignoreList)) continue;

    const srcPath = path.join(src, entry.name);
    const destPath = path.join(dest, entry.name);

    if (entry.isDirectory()) {
      fs.mkdirSync(destPath, { recursive: true });
      copyRecursive(srcPath, destPath, ignoreList);
    } else {
      fs.copyFileSync(srcPath, destPath);
    }
  }
}

function copyTemplate(src: string, dest: string): void {
  const entries = fs.readdirSync(src, { withFileTypes: true });
  for (const entry of entries) {
    const srcPath = path.join(src, entry.name);
    const destPath = path.join(dest, entry.name);
    if (entry.isDirectory()) {
      fs.mkdirSync(destPath, { recursive: true });
      copyTemplate(srcPath, destPath);
    } else {
      fs.copyFileSync(srcPath, destPath);
    }
  }
}

async function packageProject(workspaceFolder: string): Promise<void> {
  const binFolder = path.join(workspaceFolder, "bin");
  const ignoreList = loadIgnoreList(workspaceFolder);

  if (fs.existsSync(binFolder)) fs.rmSync(binFolder, { recursive: true, force: true });
  fs.mkdirSync(binFolder);

  copyRecursive(workspaceFolder, binFolder, ignoreList);

  vscode.window.showInformationMessage("Lime: Application packaged to bin/");
}

async function createNewProject(context: vscode.ExtensionContext): Promise<void> {
  const picked = await vscode.window.showOpenDialog({
    canSelectFiles: false,
    canSelectFolders: true,
    canSelectMany: false,
    title: "Create new Lime project",
    openLabel: "Select Folder",
  });

  if (!picked || picked.length === 0) return;

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

import * as winVersionInfo from "win-version-info";
const getFileVersion = (winVersionInfo as any).default ?? winVersionInfo;

async function checkEngineVersion(context: vscode.ExtensionContext, workspaceFolder: string): Promise<void> {
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

  const choice = await vscode.window.showInformationMessage(
    `Lime: Update engine to ${templateVersion} (from ${projectVersion})?`,
    "Update", "Ignore"
  );

  if (choice === "Update") {
    fs.copyFileSync(templateDll, projectDll);
    vscode.window.showInformationMessage("Lime: LimeEngine.dll updated successfully.");
    await checkMissingDlls(context, workspaceFolder);
  }
}

async function checkMissingDlls(context: vscode.ExtensionContext, workspaceFolder: string): Promise<void> {
  const templateDir = path.join(context.extensionPath, "template/lib");

  const templateEntries = fs.readdirSync(templateDir, { withFileTypes: true });
  const missingDlls = templateEntries
    .filter(e => e.isFile() && e.name.endsWith(".dll"))
    .map(e => e.name)
    .filter(name => {
      const inRoot = fs.existsSync(path.join(workspaceFolder, name));
      const inLib  = fs.existsSync(path.join(workspaceFolder, "lib", name));
      return !(inRoot || inLib);
    });
  if (missingDlls.length === 0) return;

  const choice = await vscode.window.showInformationMessage(
    `Lime: Missing DLL(s): ${missingDlls.join(", ")}. Update?`,
    "Update", "Ignore"
  );

  if (choice === "Update") {
    for (const name of missingDlls) {
      const destDir = name.startsWith("ikp") ? workspaceFolder : path.join(workspaceFolder, "lib");
      fs.mkdirSync(destDir, { recursive: true });
      fs.copyFileSync(path.join(templateDir, name), path.join(destDir, name));
    }
    vscode.window.showInformationMessage(`Lime: Added ${missingDlls.length} DLL(s).`);
  }
}

export async function activate(context: vscode.ExtensionContext): Promise<void> {
  const workspaceFolder = vscode.workspace.workspaceFolders?.[0]?.uri.fsPath;
  if (workspaceFolder) {
    await injectLuaLibrary(context, workspaceFolder).catch((err) =>
      vscode.window.showWarningMessage(`Lime: Could not configure Lua library path: ${err}`)
    );
    await checkEngineVersion(context, workspaceFolder).catch((err) =>
      vscode.window.showWarningMessage(`Lime: Could not check engine version: ${err}`)
    );
  }

  context.subscriptions.push(
    vscode.window.onDidCloseTerminal((closed) => {
      for (const [key, terminal] of terminals) {
        if (terminal === closed) terminals.delete(key);
      }
    }),
    vscode.commands.registerCommand("lime.build", () => {
      const workspaceFolder = vscode.workspace.workspaceFolders?.[0]?.uri.fsPath ?? "";
      const batPath = path.join(context.extensionPath, "cmd", "build.bat");

      runBat(context, "build.bat", "Lime: Build");
      
      exec(`"${batPath}" "${workspaceFolder}"`, (error) => {
        if (error) {
          vscode.window.showErrorMessage("Lime: Build failed.");
          return;
        }
      });
    }),
    vscode.commands.registerCommand("lime.run", () => {
      const workspaceFolder = vscode.workspace.workspaceFolders?.[0]?.uri.fsPath ?? "";
      try {
        launchApp(workspaceFolder);
      } catch (e) {
        vscode.window.showErrorMessage("Lime: Run failed.");
      }
    }),
    vscode.commands.registerCommand("lime.buildAndRun", () => {
      const workspaceFolder = vscode.workspace.workspaceFolders?.[0]?.uri.fsPath ?? "";
      const batPath = path.join(context.extensionPath, "cmd", "build.bat");

      runBat(context, "build.bat", "Lime: Run");

      exec(`"${batPath}" "${workspaceFolder}"`, (error) => {
        if (error) {
          vscode.window.showErrorMessage("Lime: Build failed.");
          return;
        }

        let finished = false;
        const timeout = setTimeout(() => {
          if (!finished) {
            vscode.window.showErrorMessage("Lime: Run timed out.");
          }
        }, 5000);

        try {
          setTimeout(() => {
            launchApp(workspaceFolder);
          }, 200);
          finished = true;
          clearTimeout(timeout);
        } catch (e) {
          clearTimeout(timeout);
          vscode.window.showErrorMessage("Lime: Run failed.");
        }
      });
    }),
    vscode.commands.registerCommand("lime.package", () => {
      const workspaceFolder = vscode.workspace.workspaceFolders?.[0]?.uri.fsPath;
      if (!workspaceFolder) {
        vscode.window.showWarningMessage("Lime: Open a folder to package a project.");
        return;
      }
      packageProject(workspaceFolder).catch((err) =>
        vscode.window.showErrorMessage(`Lime: Packaging failed: ${err}`)
      );
    }),
    vscode.commands.registerCommand("lime.newProject", () => {
      createNewProject(context).catch((err) =>
        vscode.window.showErrorMessage(`Lime: Could not create project: ${err}`)
      );
    })
  );
}

export function deactivate(): void {}