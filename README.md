# Orbis
**A 64-bit cross-platform operating system combining macOS-like architecture with Linux + Windows + Mach-O app compatibility.**

---

## Overview

Orbis is a next-generation 64-bit OS designed from scratch to run on **x86_64, ARM64, and RISC-V architectures**. Inspired by macOS, Orbis combines a modern UI with robust system fundamentals, while supporting a wide variety of application formats:

- **PE / ELF loaders** for Windows and Linux compatibility  
- **.app bundles / Mach-O executables** for macOS-style apps  
- **Custom kernel and multitasking support**  
- **Cross-platform abstraction layer** for seamless hardware support  

Currently developed by a small, passionate team of three contributors, with active collaboration and modular design principles.

---

## Features

### Kernel & OS Core
- Fully custom **64-bit kernel** with multitasking and memory management  
- Cross-platform support for **x86, ARM, and RISC-V**  
- Modular drivers and device support for a clean architecture  
- Custom bootloader to initialize the OS on multiple architectures  

### Application Compatibility
- **PE loader**: Run Windows executables  
- **ELF loader**: Run Linux binaries  
- **Mach-O loader**: Support `.app` bundles and macOS-style apps  

### User Interface
- Modern, **macOS-inspired UI**  
- 3D-rendered window manager with smooth animations  
- Multi-monitor support and scalable resolution  

### Development & Extensibility
- Written primarily in **C/C++ with Assembly for low-level modules**  
- Git-based modular project structure for easy contribution  
- Documentation and design diagrams for internal architecture  

---

## Getting Started

> Note: Orbis is currently in alpha development. Bootable images are available for testing in QEMU or other virtual machines.

1. Clone the repository:
```bash
git clone https://github.com/Driftless-Pvt-Ltd/Orbis-Kernel.git
```

2. Navigate to the build folder
```bash
cd Orbis-Kernel
```

3. Follow the platform-specific build instructions to compile the Kernel and UI.

## Team
- Kap Petrov: Lead Developer, Kernel & System Architecture
- Vincent C.: UI/UX Design & System Research
- Mohammed Abdelkafi: Documentation & Contribution Management

## Contact & Contributions
Orbis is **open for contribution.** Interested developers can fork the repository, submit pull requests, and join discussions in the GitHub issues.

For professional inquiries or collaboration, reach out via email: minicomputersoftware@gmail.com
