# QuickShare

[![License: CC BY-NC 4.0](https://img.shields.io/badge/License-CC%20BY--NC%204.0-lightgrey.svg)](https://creativecommons.org/licenses/by-nc/4.0/)

> ⚠️ **EARLY DEVELOPMENT WARNING** ⚠️  
> This project is currently in its early stages of development. Functionality is not guaranteed, and until this message is removed, the software should be considered unstable, potentially faulty, and prone to throwing errors. Use at your own risk!

Sharing a quick screenshot, a short video, or a zipped project folder with friends shouldn't be a hassle. We've all been there: you try to drop a file into a chat on a certain widely used communication platform, only to be hit with a hard block because your file is slightly over their laughable 8MB free-tier limit. 

**QuickShare** was born out of that exact frustration. It is a lightweight, blazing-fast, cross-platform C++ desktop application that integrates seamlessly with your operating system to let you upload and share files via Pixeldrain in seconds.

## How it Works

Whether you prefer a visual interface or working straight from your file manager, QuickShare is designed to stay out of your way:

- **Drop & Go:** Open the clean, modern Qt-based UI and drag-and-drop any file or folder directly into the window.
- **Headless Context Menu Integration:** Right-click any file directly in Windows Explorer, macOS Finder, or your Linux file manager, and hit "Share". QuickShare runs entirely in the background, uploads the file, and instantly copies the Pixeldrain link directly to your clipboard.
- **Smart Auto-Zipping:** Need to share a whole directory or a batch of files? QuickShare automatically detects multiple files or folders and bundles them into a clean ZIP archive before uploading.
- **Portable & Zero-Hassle:** QuickShare is a single, lightweight binary. No bloated installers, no hidden background services, and no complex uninstallation. Just download it, run it, and if you ever want to remove it, simply delete the file. It's that simple.

*Note: While Pixeldrain allows anonymous uploads, you can configure your personal API key in the app settings to link uploads to your account and bypass anonymous size limits (e.g., the 10GB restriction).*

## CLI Usage

The CLI mode allows for powerful automation and quick uploads directly from the terminal.

### Commands
- `qs config <option> <value>`: Modify application settings.
    - `api <key>`: Sets your Pixeldrain API key.
    - `hide-in-taskbar <true|false>`: Toggles visibility in the system tray/dock.
    - `autostart <true|false>`: Enables or disables launch on system startup.

### Flags
- `-f, --file <path>`: Specify the file or directory to upload.
- `-c, --compress`: Forces the program to compress the specified path into a ZIP archive before uploading.
- `-s, --solo`: If a directory is specified, it uploads each file individually instead of zipping them.
- `-a, --api <key>`: Uses a temporary API key for this specific command execution (overrides saved config).
- `-h, --help`: Displays the help message with all available options.
- `-v, --version`: Displays the current version of QuickShare.

### Usage Examples

**1. Basic file upload:**
```bash
qs my_photo.png
```

**2. Upload a directory as a single ZIP archive:**
```bash
qs -c ./my_project_folder
```

**3. Upload every file in a folder separately using a one-time API key:**
```bash
qs -s -a YOUR_TEMP_KEY ./vacation_photos
```

**4. Update your API key permanently:**
```bash
qs config api 12345-abcde-67890
```

### Directory Handling Logic
When you pass a directory without `-c` or `-s` flags, the program will interactively ask:
1. **"Compress into ZIP? [Y/n]"**: If you choose Yes (default), a single ZIP is created and uploaded.
2. **"Upload each file separately? [Y/n]"**: If you choose Yes, every file in the directory (recursive) is uploaded one by one. A report file `qs-upload-data(date).txt` will be created in the current directory containing all links.

## Installation

You can download pre-compiled binaries for **Windows**, **macOS**, and **Linux** from the [Releases](../../releases) page.

## Building from Source

### Standard Build (CMake)
Ensure you have **CMake** (3.19+) and the **Qt 6 Framework** installed.

```bash
git clone https://github.com/ZekqKeku/QuickShare.git
cd QuickShare
mkdir build && cd build
cmake ..
cmake --build . -j 4
```

### Building for Flatpak (Linux)
To build and install QuickShare as a Flatpak, you need `flatpak` and `flatpak-builder` installed on your system.

```bash
# Install the required KDE runtime and SDK
flatpak install flathub org.kde.Platform//6.6 org.kde.Sdk//6.6

# Build the Flatpak
flatpak-builder --force-clean build-dir flatpak/com.quickshare.app.yml

# Install the build locally
flatpak-builder --user --install --force-clean build-dir flatpak/com.quickshare.app.yml

# Run QuickShare
flatpak run com.quickshare.app
```

## License

This project is licensed under the **Creative Commons Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)** license. See the [LICENSE](LICENSE) file for details.
