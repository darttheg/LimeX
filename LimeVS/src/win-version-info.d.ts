declare module 'win-version-info' {
  interface VersionInfo {
    ProductVersion?: string;
    FileVersion?: string;
    [key: string]: string | undefined;
  }
  function getFileVersion(filePath: string): VersionInfo;
  export { getFileVersion };
}