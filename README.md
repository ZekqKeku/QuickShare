# QuickShare

[![License: CC BY-NC 4.0](https://img.shields.io/badge/License-CC%20BY--NC%204.0-lightgrey.svg)](https://creativecommons.org/licenses/by-nc/4.0/)

> ⚠️ **EARLY DEVELOPMENT WARNING** ⚠️  
> This project is currently in its early stages of development. Functionality is not guaranteed, and until this message is removed, the software should be considered unstable, potentially faulty, and prone to throwing errors. Use at your own risk!

Sharing a quick screenshot, a short video, or a zipped project folder with friends shouldn't be a hassle. We've all been there: you try to drop a file into a chat on a certain widely used communication platform, only to be hit with a hard block because your file is slightly over their laughable 8MB free-tier limit. 

**QuickShare** was born out of that exact frustration. It is a lightweight, blazing-fast, cross-platform C++ desktop application that integrates seamlessly with your operating system to let you upload and share files via Pixeldrain in seconds. No more arbitrary blocks, no more forced subscriptions just to share a 9MB video.

## How it Works

Whether you prefer a visual interface or working straight from your file manager, QuickShare is designed to stay out of your way:

- **Drop & Go:** Open the clean, modern Qt-based UI and drag-and-drop any file or folder directly into the window.
- **Headless Context Menu Integration:** Right-click any file directly in Windows Explorer, macOS Finder, or your Linux file manager, and hit "Share". QuickShare runs entirely in the background, uploads the file, and instantly copies the Pixeldrain link directly to your clipboard.
- **Smart Auto-Zipping:** Need to share a whole directory or a batch of files? QuickShare automatically detects multiple files or folders and bundles them into a clean ZIP archive before uploading.

*Note: While Pixeldrain allows anonymous uploads, you can configure your personal API key in the app settings to link uploads to your account and bypass anonymous size limits (e.g., the 10GB restriction).*

## Installation

Pre-compiled, ready-to-run binaries for **Windows**, **macOS**, and **Linux** are automatically generated and available on the [Releases](../../releases) page.

## Building from Source

If you prefer to compile the application yourself, ensure you have **CMake** (3.19+) and the **Qt 6 Framework** installed on your system.

```bash
# Clone the repository
git clone https://github.com/ZekqKeku/QuickShare.git
cd QuickShare

# Create a build directory
mkdir build && cd build

# Configure and build the project
cmake ..
cmake --build . -j 4
```

## Project Goals & Motivation

Beyond solving a daily annoyance, this project serves as a hands-on technical sandbox to experiment with:
- Developing robust, cross-platform desktop software using modern **C++** and **Qt 6**.
- Orchestrating automated multi-os build pipelines and artifact generation using **GitHub Actions**.
- Testing the limits and capabilities of Gemini CLI as an autonomous AI peer-programmer assistent throughout the entire software engineering lifecycle.

## License

This project is licensed under the **Creative Commons Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)** license.

- **Commercial Use:** Commercial use of this software is strictly restricted to the original author.
- **Forks and Derivatives:** Any forks or derivative works must credit the original repository and maintain the same license.

For the full legal text, see the [LICENSE](LICENSE) file.
