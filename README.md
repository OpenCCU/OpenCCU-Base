# OpenCCU-Base

`OpenCCU-Base` provides the base source code and binary files for the major runtime components required by the OpenCCU project.

It serves as the foundational layer for core Homematic / Homematic IP services, including native daemons, shared libraries, WebUI assets, startup scripts, and configuration templates that are assembled into OpenCCU firmware and runtime environments.

## OpenCCU project links

- OpenCCU GitHub organization: https://github.com/OpenCCU
- OpenCCU main repository: https://github.com/OpenCCU/OpenCCU
- OpenCCU website: https://www.openccu.de/

## Purpose
With the release of OpenCCU-Base and thus essential components of the CCU source code, new and extended possibilities arise for OpenCCU and OpenCCU-LTS users regarding individual customizations, extensions, and participation in the OpenCCU projects.
For example, adjustments to the WebUI are significantly simplified, and modifications to core components of the CCU software from eQ-3—such as the Homematic radio and wired services—are now possible.
Furthermore, in keeping with the spirit of the OCCU project, components that cannot be published as source code, as well as the Homematic IP service, will continue to be updated regularly by eQ-3 and contributors.

This repository is used as the common base package that delivers:

- Native component binaries for multiple target architectures.
- C/C++ source modules for key OpenCCU services and libraries.
- WebUI resources and build/generation assets.
- System scripts and default configuration templates.
- Additional runtime artifacts used by Homematic IP server components.

## Repository layout (high level)

- `src/` - source code and module-local build logic (Makefiles) for core components.
  Closed-source components (`src/ReGaHss/`, `src/hmipserver/`) only contain a `README.md` (and a `ChangeLog`) there, their binaries are part of `bin/` and `opt/`.
- `bin/` - architecture-specific executable binaries and helper/startup scripts.
- `www/` - WebUI and Tcl-based runtime resources.
- `etc/` - configuration templates used during deployment/runtime setup.
- `opt/` - bundled server/runtime artifacts (including HMServer/HmIP related files).
- `lib/`, `usr/`, `firmware/` - additional runtime libraries and firmware-related content.

## How this repository is typically used

`OpenCCU-Base` is primarily consumed as part of the larger OpenCCU build and packaging pipeline.  
Its module outputs are built/staged and then integrated into final OpenCCU images and update artifacts.

In short: this repository is the component base layer that OpenCCU depends on for essential binaries, libraries, scripts, and configuration defaults.

## License information

This repository contains components under different licenses.

- Main license references are available in `licenses/licenses.md`.
- Additional bundled license texts are available in `licenses/` (for example `gpl-2.0.txt`, `lgpl-2.1.txt`, and `HMSL2.txt`).
