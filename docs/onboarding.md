# Developer Onboarding (macOS and Windows)

Goal: every teammate can clone the repo, build it, and launch the app on their own laptop. Work through this document top to bottom. Do not skip the verification step at the end.

> **Team rule:** everyone uses the **same Qt version**. Mixed Qt versions cause more problems than mixed operating systems.
> **Pinned version:** Qt 6.9 (team to confirm before Sprint 1 and update this line if changed).

---

## 1. What everyone must do (both platforms)

1. Create a free GitHub account (if you don't have one) and ask the repo owner to add you as a collaborator.
2. Create a free Qt account (the installer requires login): https://login.qt.io
3. Install the tools for your OS (Section 2 or 3).
4. Clone the repo and run the smoke test (Sections 4 and 5).
5. Tick off the checklist (Section 8) and post a screenshot of the running smoke test in the team channel.

---

## 2. macOS setup

### 2.1 Command line tools (compiler)

```bash
xcode-select --install
```

This installs Apple Clang and Git. Accept the prompts and wait for it to finish.

### 2.2 Qt

1. Download the **Qt Online Installer** from https://www.qt.io/download-open-source (sign in with your Qt account).
2. In the installer, expand the pinned Qt version (6.9) and tick:
   - **macOS** (the desktop kit)
   - **Additional Libraries:** Qt Charts, Qt Positioning, Qt Network Authorization is not needed
   - **Qt Debug Information Files** (optional, helps debugging)
3. Under **Developer and Designer Tools**, tick:
   - **Qt Creator**
   - **CMake**
   - **Ninja**
4. Install to the default location (`~/Qt`).

### 2.3 Apple Silicon vs Intel

Qt installs the correct build for your Mac automatically (universal on recent versions). If you see architecture errors, tell the team which chip you have (Apple menu → About This Mac).

---

## 3. Windows setup

### 3.1 Compiler (MSVC)

The team standard on Windows is **MSVC**, not MinGW.

1. Download **Build Tools for Visual Studio** (or Visual Studio Community) from https://visualstudio.microsoft.com/downloads/
2. In the installer, select the workload **Desktop development with C++** and install.

### 3.2 Git

Install Git for Windows from https://git-scm.com/download/win. Use the default options, except choose **"Checkout as-is, commit as-is"** or leave the default and rely on the repo's `.gitattributes` (see Section 6).

### 3.3 Qt

1. Download the **Qt Online Installer** from https://www.qt.io/download-open-source and sign in.
2. Expand the pinned Qt version (6.9) and tick:
   - **MSVC 2022 64-bit** (the desktop kit; the year must match your Visual Studio compiler)
   - **Additional Libraries:** Qt Charts, Qt Positioning
3. Under **Developer and Designer Tools**, tick:
   - **Qt Creator**
   - **CMake**
   - **Ninja**
4. Install to the default location (`C:\Qt`).

### 3.4 Common Windows pitfalls

- Use the **MSVC** kit in Qt Creator, not MinGW, or your build will differ from teammates'.
- Keep the project folder path **short and without spaces** (e.g. `C:\dev\datascope`).
- If a built app fails to start with a missing `Qt6*.dll` error, run `windeployqt` on the executable, or run it from inside Qt Creator.

---

## 4. Clone the repo

Replace the URL with the team repo's address.

```bash
git clone https://github.com/USERNAME/REPO.git
cd REPO
```

Folder layout (expected):

```
/core         app shell, plugin loader, layout manager
/interfaces   shared plugin interfaces (IDataParser, IVisualizationPlugin)
/plugins      one folder per plugin
/data         sample datasets
/docs         documentation (this file lives here)
```

---

## 5. Build and run the smoke test

The smoke test is a minimal Qt window used only to prove your environment works. It is not the dashboard.

### 5.1 Using Qt Creator (recommended for everyone)

1. Open Qt Creator → **File → Open File or Project** → select the top-level `CMakeLists.txt`.
2. When asked to configure, select the **Desktop** kit for your platform:
   - macOS: "Desktop Qt 6.9.x clang 64-bit"
   - Windows: "Desktop Qt 6.9.x MSVC2022 64-bit"
3. Click **Configure Project**.
4. Press the green **Run** button (Ctrl/Cmd + R).
5. A window titled "Hello Qt" should appear.

### 5.2 Using the command line (optional)

macOS or Windows (from a Qt/MSVC developer prompt on Windows):

```bash
cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH=/path/to/Qt/6.9.x/<kit>
cmake --build build
```

Run the produced executable from the `build` folder.

### 5.3 Reference smoke-test files (if the repo doesn't have them yet)

`CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.21)
project(HelloQt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_add_executable(HelloQt main.cpp)
target_link_libraries(HelloQt PRIVATE Qt6::Widgets)
```

`main.cpp`:

```cpp
#include <QApplication>
#include <QLabel>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QLabel label("Hello Qt - environment works");
    label.resize(320, 120);
    label.setWindowTitle("Hello Qt");
    label.show();
    return app.exec();
}
```

---

## 6. Repo settings that prevent Mac/Windows problems

These files must exist in the repo root (add them if missing):

`.gitattributes` (stops line-ending conflicts between Windows and Mac):

```
* text=auto
*.png binary
*.jpg binary
*.pem binary
```

`.gitignore`:

```
build/
build-*/
CMakeLists.txt.user
.vscode/
.idea/
*.pem
```

Code habits for cross-platform safety:

- Never hardcode paths. Use `QDir`, `QFileInfo`, and `QStandardPaths`.
- Never hardcode plugin extensions (`.so`, `.dll`, `.dylib`). Use `QLibrary::isLibrary()`.
- Watch file name case: Mac is usually case-insensitive, Linux CI is not. Match the exact case in `#include` lines.
- Do not commit build output, personal IDE settings, or the AWS `.pem` key.

---

## 7. Git workflow

1. `git pull` before starting work.
2. Create a branch per task: `git checkout -b feature/short-name`
3. Commit small and often with clear messages.
4. Push the branch and open a **pull request**; another teammate reviews before merging to `main`.
5. GitHub Actions builds the project on macOS and Windows for every push and pull request. Do not merge if the build fails.

---

## 8. Setup checklist (each teammate)

- [ ] GitHub account created and added to the repo
- [ ] Qt account created
- [ ] Compiler installed (Xcode command line tools on Mac, MSVC Build Tools on Windows)
- [ ] Qt 6.9 installed with Qt Creator, CMake, Ninja, and Qt Charts
- [ ] Repo cloned
- [ ] Smoke test builds and the "Hello Qt" window appears
- [ ] Screenshot of the running window posted to the team channel
- [ ] Read Sections 6 and 7

---

## 9. Troubleshooting

| Problem | Likely cause / fix |
|---|---|
| Qt Creator shows no kits | Qt or the compiler isn't installed correctly. On Windows, confirm "Desktop development with C++" is installed; on Mac, run `xcode-select --install`. |
| "Could not find Qt6" in CMake | Qt isn't on the prefix path. In Qt Creator pick the right kit; on the command line set `CMAKE_PREFIX_PATH`. |
| App closes instantly on Windows with a missing DLL error | Run from Qt Creator, or run `windeployqt` on the executable. |
| Whole-file diffs in Git on files nobody edited | Line-ending mismatch. Confirm `.gitattributes` is committed and run `git add --renormalize .` |
| Build works on Mac but fails on Windows (or reverse) | Likely a case-sensitive include, a hardcoded path, or a missing include. Check the CI log for the exact error. |

If you are stuck for more than 30 minutes, ask in the team channel with the exact error text and your OS.

---

## 10. After setup

Once all four smoke tests pass, the team builds the first working slice together in Sprint 1: one parser, one data model, and one visualization, end to end. Nobody should start separate features before that slice exists.
