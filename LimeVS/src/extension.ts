import * as vscode from "vscode";
import * as path from "path";
import * as fs from "fs";
import { spawn, exec } from "child_process";

function getApiPath(context: vscode.ExtensionContext): string {
  return path.join(context.extensionPath, "api").replace(/\\/g, "/");
}

async function injectLuaLibrary(context: vscode.ExtensionContext): Promise<void> {
  const apiPath = getApiPath(context);
  const workspaceFolder = vscode.workspace.workspaceFolders?.[0]?.uri.fsPath;

  if (!workspaceFolder) {
    vscode.window.showWarningMessage("Lime: Open a folder to enable Lua API autocompletion.");
    return;
  }

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
  const appPath = path.join(workspaceFolder, "app.exe");
  spawn(appPath, [], {
    detached: true,
    stdio: "ignore",
    cwd: workspaceFolder,
  }).unref();
}

const terminals: Map<string, vscode.Terminal> = new Map();

function runBat(context: vscode.ExtensionContext, bat: string, name: string): void {
  const batPath = path.join(context.extensionPath, "cmd", bat);
  const workspaceFolder = vscode.workspace.workspaceFolders?.[0]?.uri.fsPath ?? "";

  const existing = terminals.get(name);
  if (existing && !existing.exitStatus) existing.dispose();

  const terminal = vscode.window.createTerminal({
    name,
    shellPath: "cmd.exe",
    shellArgs: ["/Q", "/K", batPath, workspaceFolder],
  });
  terminals.set(name, terminal);
  terminal.show(true);
}

export async function activate(context: vscode.ExtensionContext): Promise<void> {
  await injectLuaLibrary(context).catch((err) =>
    vscode.window.showWarningMessage(`Lime: Could not configure Lua library path: ${err}`)
  );

  context.subscriptions.push(
    vscode.window.onDidCloseTerminal((closed) => {
      for (const [key, terminal] of terminals) {
        if (terminal === closed) terminals.delete(key);
      }
    }),
    vscode.commands.registerCommand("lime.build", () => {
      runBat(context, "build.bat", "Lime: Build");
    }),
    vscode.commands.registerCommand("lime.buildAndRun", () => {
      const workspaceFolder = vscode.workspace.workspaceFolders?.[0]?.uri.fsPath ?? "";
      const batPath = path.join(context.extensionPath, "cmd", "build.bat");

      runBat(context, "build.bat", "Lime: Build + Run");

      exec(`"${batPath}" "${workspaceFolder}"`, (error) => {
        if (!error) {
          setTimeout(() => launchApp(workspaceFolder), 150);
        } else {
          vscode.window.showErrorMessage("Lime: Build failed.");
        }
      });
    })
  );
}

export function deactivate(): void {}