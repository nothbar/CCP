# Chromium Component Package

Chromium Component Package (CCP for short) is a brand new Chromium redistribution package. Unlike traditional Chromium-based browsers, CCP is dedicated to the Open Web Runtime project. By converting the chrome.exe project into chrome_rt.dll, the Chromium browser can be directly loaded by general applications.

## Features
In addition to not being able to run directly as an executable program, CCP retains all the functionality of the Chromium project. In addition, CCP also provides the following mechanisms:

### Custom HTML tags
CCP enables developers to define their own HTML tags and parse them themselves.

### Draw native interface to webpage area
CCP enables developers to draw native UI components (such as .NET Form, UserControl) directly to the web page area. The native UI components drawn in the web page area are controlled by the current page.

### Subwindow mode
CCP enables the browser window to be drawn into the application interface as a sub-window. When displayed as a sub-window mode, CCP retains the browser's tab bar but removes the address bar.

## How to compile your own CCP?

CCP only supports the Windows platform. Please check out the specific version of the source code according to the official Chromium documentation requirements.

[Checking out and Building Chromium for Windows](https://chromium.googlesource.com/chromium/src/+/master/docs/windows_build_instructions.md)

Note that in the `Get the code` section, the `fetch chromium` command will check out the latest source code of the chromium repository. Please use the following command instead:

```
gclient config https://chromium.googlesource.com/chromium/src.git
```

This command creates a `.gclient` file. If you need to use a cache directory, add the `--cache-dir` parameter.

```
gclient config --cache-dir C:\src\chromium_git_cache https://chromium.googlesource.com/chromium/src.git
```

Then use the following command to get the source code of the specified version, `rev` is the version number.

```
gclient sync --revision src@{rev} --with_tags --with_branch_heads
```

