<div align="center">

# ⚡ samp-rakdriver

**Production-grade, asynchronous, headless SA-MP 0.3.7 RakNet network engine written in modern C++17.**

[![C++17](https://img.shields.io/badge/Language-C%2B%2B17-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)](https://en.cppreference.com/w/cpp/17)
[![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20Windows-000000?style=for-the-badge&logo=linux&logoColor=white)](https://cmake.org)
[![Build](https://img.shields.io/badge/Build-CMake%203.16%2B-064F8C?style=for-the-badge&logo=cmake&logoColor=white)](https://cmake.org)
[![License](https://img.shields.io/badge/License-MIT-green.svg?style=for-the-badge)](LICENSE)

<p align="center">
  <a href="#-overview">Overview</a> •
  <a href="#-key-features">Features</a> •
  <a href="#-architecture">Architecture</a> •
  <a href="#-protocol-deep-dive">Protocol</a> •
  <a href="#-getting-started">Getting Started</a> •
  <a href="#-building">Building</a> •
  <a href="#-usage">Usage</a> •
  <a href="#-performance">Performance</a>
</p>

</div>

---

## 📌 Overview

**samp-rakdriver** — is a lightweight headless C++17 network engine for working with the **SA-MP 0.3.7** at the RakNet level.

The project allows working with SA-MP without launching the full GTA: San Andreas client. Instead of the graphics engine, RenderWare, and DirectX 9, it uses its own network layer that communicates directly with UDP/RakNet and implements the required parts of the SA-MP protocol stack.

This approach makes the project suitable for:

- low-level SA-MP/RakNet research;
- building standalone headless clients;
- network scenario automation;
- server-side logic testing;
- working with RPCs and binary packets;
- experiments with player synchronization;
- multi-threaded infrastructure with multiple client instances.

The project is focused on a minimal runtime footprint and separation of the transport layer, protocol logic, and application state.

> **Note:** This README describes the project's architecture and capabilities according to the provided specification. The behavior of individual servers may depend on their implementation and configuration.

---

## ✨ Key Features

### ⚡ Asynchronous Network Loop

The network layer is built around a non-blocking UDP socket abstraction:

- BSD sockets on Linux;
- Winsock on Windows;
- a dedicated low-latency I/O worker;
- asynchronous processing of incoming and outgoing data;
- no need to launch the graphical GTA:SA client.

### 📦 BitStream Binary Serialization

The built-in `BitStream` is designed for precise binary serialization:

- writing data at the byte and bit level;
- reading arbitrary bit-length sequences;
- dynamic buffer expansion;
- stream bounds checking;
- preparing binary payloads compatible with the server format.

This is especially important for RakNet/SA-MP, where packet structure and exact field placement matter.

### 🌐 Cross-platform

The project is designed to build on:

- Linux + GCC;
- Linux + Clang;
- Windows + MSVC;
- Windows via MinGW-w64;
- CMake as the unified configuration system.

### 💬 RPC and Dialog Subsystem

The application layer includes SA-MP RPC handling:

- RPC dispatching;
- handling `ShowDialog`;
- parsing string payloads;
- building responses;
- working with binary protocol structures.

### 🏃 Player Synchronization

An OnFoot synchronization frame generator is implemented.

The system generates raw synchronization packets at a configured interval, approximately **10 Hz** in the original specification.

### 🔐 Authentication / Handshake

The network layer contains the mechanisms required for the SA-MP handshake:

- challenge-response flow;
- AuthKey transformation;
- CRC32;
- generation of the Version 4057 handshake;
- ClientJoin payload.

---

# 🏗 Architecture

The project architecture follows separation of concerns: the low-level transport layer is kept separate from the SA-MP state machine and application logic.

```text
cpp/
├── CMakeLists.txt
│
├── include/
│   ├── core/
│   │   ├── Application.h
│   │   ├── Config.h
│   │   └── Logger.h
│   │
│   ├── net/
│   │   ├── BitStream.h
│   │   ├── NetworkTypes.h
│   │   ├── PacketEnumerations.h
│   │   ├── RakClient.h
│   │   └── Socket.h
│   │
│   ├── samp/
│   │   ├── DialogManager.h
│   │   ├── PlayerPed.h
│   │   ├── RPCManager.h
│   │   ├── SAMPDefines.h
│   │   ├── SAMPPacket.h
│   │   └── SyncStructures.h
│   │
│   └── utils/
│       ├── Crypto.h
│       └── Vector3.h
│
└── src/
    └── ...
```

## 🧩 Core Modules

| Module | Purpose |
|---|---|
| `core/` | Runtime context, configuration, and thread orchestration |
| `net/` | UDP/RakNet transport and binary serialization |
| `samp/` | SA-MP protocol logic, RPCs, dialogs, and synchronization |
| `utils/` | Utility mathematics and cryptographic operations |

### `core/`

Contains the application control layer.

- `Application.h` — main execution loop and state management.
- `Config.h` — CLI flags and runtime configuration.
- `Logger.h` — thread-safe colored console logger.

### `net/`

Low-level network layer.

- `Socket.h` — cross-platform non-blocking UDP wrapper.
- `RakClient.h` — RakNet connection state and dispatch management.
- `BitStream.h` — binary reader/writer.
- `NetworkTypes.h` — network address and packet abstractions.
- `PacketEnumerations.h` — packet IDs and priority/reliability definitions.

### `samp/`

SA-MP domain layer.

- `DialogManager.h` — handling incoming dialog payloads.
- `RPCManager.h` — RPC dispatch matrix.
- `PlayerPed.h` — local player state.
- `SAMPPacket.h` — handshake packet builders.
- `SyncStructures.h` — OnFoot/Vehicle synchronization structures.
- `SAMPDefines.h` — NetGame versions and RPC ID mappings.

### `utils/`

Utility algorithms.

- `Crypto.h` — AuthKey XOR transformation and CRC32.
- `Vector3.h` — 3D coordinates and vector operations.

---

# 🔬 Protocol Deep-Dive

## 🤝 Handshake Sequence

`samp-rakdriver` implements the connection sequence corresponding to SA-MP 0.3.7-R1 in the described specification.

```text
Client (samp-rakdriver)                  Server
        |                                   |
        | ---- ID_CONNECTION_REQUEST -----> |
        |                                   |
        | <--- ID_CONNECTION_REQUEST_ACCEPTED
        |                                   |
        | ---- ID_SAMP_PACKET ------------> |
        |      ClientJoin / AuthKey         |
        |                                   |
        | <--- ID_RPC / RPC_InitGame ------ |
        |                                   |
        | <=== ID_RPC / RPC_ShowDialog ==== |
        |                                   |
        | === ID_RPC / RPC_DialogResponse > |
        |                                   |
        | ---- ID_PLAYER_SYNC ------------> |
        |      OnFoot Sync @ ~10Hz          |
        |                                   |
```

The sequence can be conceptually divided into the following stages:

1. **RakNet connection request**
2. **Connection acceptance**
3. **SA-MP ClientJoin**
4. **Authentication / AuthKey**
5. **Server initialization via `RPC_InitGame`**
6. **Optional dialog handling**
7. **Sending player synchronization**
8. **Continuing the network loop**

---

# 📦 Binary Serialization

All important outgoing payload structures use packed layouts so that the binary representation matches the format expected by the server.

Example of sending an OnFoot synchronization frame:

```cpp
BitStream bs;

bs.Write<uint8_t>(ID_PLAYER_SYNC);
bs.WriteBits(
    reinterpret_cast<const uint8_t*>(&m_syncData),
    sizeof(PlayerSyncData) * 8
);

m_client->Send(
    &bs,
    HIGH_PRIORITY,
    UNRELIABLE_SEQUENCED,
    0
);
```

### Why is BitStream Important?

Ordinary C++ structure serialization is not always suitable for a network protocol. In SA-MP/RakNet, the following are critical:

- exact field order;
- size of each field;
- alignment;
- number of transmitted bits;
- byte order;
- reliability/priority type;
- packet header structure.

Therefore, `BitStream` serves as the intermediate layer between C++ structures and the actual UDP payload.

---

# 🧱 Synchronization

A separate player state structure is used for OnFoot synchronization.

The synchronization loop generates a packet approximately **10 times per second**, i.e. at an interval of about 100 ms.

A typical cycle looks as follows:

```text
Player State
     │
     ▼
SyncStructures
     │
     ▼
BitStream serialization
     │
     ▼
RakNet packet
     │
     ▼
UDP socket
     │
     ▼
SA-MP Server
```

The original specification states that the raw OnFoot synchronization frame size is **68 bytes**.

---

# ⚙️ Getting Started

## Requirements

| Component | Recommended | Purpose |
|---|---|---|
| C++ Compiler | GCC 8+ / Clang 7+ / MSVC 2019+ | C++17 compilation |
| CMake | 3.16+ | Project generation |
| Linux | `build-essential`, pthread | Native Linux build |
| Windows cross-toolchain | `mingw-w64` | Cross-compilation |

---

# 🔨 Building

## 🐧 Linux

Clone the project:

```bash
git clone https://github.com/stackvoided/samp-rakdriver.git
cd samp-rakdriver/cpp
```

Create a separate build directory:

```bash
mkdir build
cd build
```

Configure the Release build:

```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
```

Compile:

```bash
make -j$(nproc)
```

After a successful build, the `rakbot` executable will be located in `build/`.

---

## 🪟 Windows via MinGW-w64

Install `mingw-w64`, then:

```bash
cd cpp
mkdir build_win
cd build_win
```

Configure CMake for the Windows target:

```bash
cmake \
  -DCMAKE_SYSTEM_NAME=Windows \
  -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ \
  -DCMAKE_BUILD_TYPE=Release \
  ..
```

Build the project:

```bash
make -j$(nproc)
```

The result will be:

```text
rakbot.exe
```

---

## 🪟 Native Windows / Visual Studio

Open the **Developer Command Prompt for Visual Studio**:

```dos
cd cpp
mkdir build
cd build
```

Generate Makefiles:

```dos
cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release ..
```

Build the project:

```dos
nmake
```

---

# 💻 Usage

After building, the application is launched with the target SA-MP server parameters:

```bash
./rakbot <IPv4_Address> <Port> <Nickname>
```

### Example

Connect to a local server on the default port:

```bash
./rakbot 127.0.0.1 7777 Driver_Node
```

## CLI Arguments

| Argument | Type | Default | Description |
|---|---|---|---|
| `argv[1]` | `std::string` | `127.0.0.1` | IPv4 address of the SA-MP server |
| `argv[2]` | `uint16_t` | `7777` | Server UDP port |
| `argv[3]` | `std::string` | `RakBot` | Nickname / account identity |

The nickname is passed in the `ClientJoin` payload as the client identifier.

---

# 🔄 Runtime Flow

Simplified application workflow:

```text
                    ┌─────────────────────┐
                    │    Application      │
                    └──────────┬──────────┘
                               │
                               ▼
                    ┌─────────────────────┐
                    │     RakClient       │
                    └──────────┬──────────┘
                               │
                    ┌──────────▼──────────┐
                    │  Non-blocking UDP   │
                    │       Socket        │
                    └──────────┬──────────┘
                               │
                ┌──────────────┴──────────────┐
                │                             │
                ▼                             ▼
        Incoming Packets              Outgoing Packets
                │                             │
                ▼                             ▼
          RPC Manager                  BitStream
                │                             │
        ┌───────┴───────┐                     ▼
        │               │                RakNet Send
        ▼               ▼
     Dialog          InitGame
     Manager
        │
        ▼
 Dialog Response
```

This separation isolates transport concerns from SA-MP-specific logic and makes the project easier to extend.

---

# 📊 Performance

According to the provided project specification:

| Metric | Value |
|---|---:|
| Memory usage | ~3.8 MB RSS / client |
| CPU footprint | <0.1% single-thread core load |
| Network I/O | ~1.2 KB/s during active sync |
| Sync frequency | ~10 Hz |

Actual values depend on the OS, compiler, optimization settings, number of clients, and the specific network scenario.

---

# 🛡 Design Goals

The project is focused on several key principles:

### Low-level Control

A minimum of abstractions between the application and the network protocol makes it possible to directly inspect and control binary packets.

### Low Overhead

The absence of the full GTA:SA runtime significantly reduces resource consumption compared with running the standard game client.

### Modularity

Transport, serialization, SA-MP logic, and utilities are separated into individual modules.

### Portability

CMake and platform-specific socket abstractions allow the same codebase to be used on Linux and Windows.

### Extensibility

The RPC manager, dialog subsystem, and synchronization structures allow new protocol handlers to be added without changing the base socket layer.

---

# 🧪 Protocol Components

| Component | Responsibility |
|---|---|
| `Socket` | UDP transport |
| `RakClient` | RakNet connection lifecycle |
| `BitStream` | Binary serialization |
| `PacketEnumerations` | Packet IDs / priorities |
| `RPCManager` | RPC dispatch |
| `DialogManager` | Dialog parsing / responses |
| `SAMPPacket` | SA-MP handshake packets |
| `SyncStructures` | Player synchronization |
| `Crypto` | AuthKey transformation / CRC32 |

---

# 📁 Recommended Repository Layout

For a GitHub repository, it is recommended to keep the following structure:

```text
samp-rakdriver/
├── cpp/
│   ├── CMakeLists.txt
│   ├── include/
│   └── src/
│
├── LICENSE
├── README.md
└── .gitignore
```

The README assumes that the main C++ project is located inside the `cpp/` directory.

---

# 📜 License

This project is licensed under the **Apache License 2.0**.

Details are available in the [`LICENSE`](LICENSE) file.

---

<div align="center">

**Built with precision C++17 for low-latency SA-MP/RakNet network research and automation.**

</div>
