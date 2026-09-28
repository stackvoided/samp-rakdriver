<div align="center">

# ⚡ samp-rakdriver

**Production-grade, asynchronous, headless SA-MP 0.3.7-R1 RakNet network engine written in modern C++17.**

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

**samp-rakdriver** — это лёгкий headless C++17 network engine для работы с сетевым протоколом **SA-MP 0.3.7-R1** на уровне RakNet.

Проект позволяет работать с SA-MP без запуска полноценного клиента GTA: San Andreas. Вместо графического движка, RenderWare и DirectX 9 используется собственный сетевой слой, который напрямую взаимодействует с UDP/RakNet и реализует необходимые части SA-MP protocol stack.

Такой подход делает проект подходящим для:

- низкоуровневого исследования SA-MP/RakNet;
- создания автономных headless-клиентов;
- автоматизации сетевых сценариев;
- тестирования серверной логики;
- работы с RPC и бинарными пакетами;
- экспериментов с синхронизацией игроков;
- многопоточной инфраструктуры с несколькими клиентскими экземплярами.

Проект ориентирован на минимальный runtime footprint и разделение транспортного уровня, протокольной логики и прикладного состояния.

> **Примечание:** README описывает архитектуру и возможности проекта согласно предоставленной спецификации. Конкретное поведение отдельных серверов может зависеть от их реализации и настроек.

---

## ✨ Key Features

### ⚡ Асинхронный сетевой цикл

Сетевой слой построен вокруг неблокирующего UDP socket abstraction:

- BSD sockets на Linux;
- Winsock на Windows;
- отдельный low-latency I/O worker;
- асинхронная обработка входящих и исходящих данных;
- отсутствие необходимости запускать графический клиент GTA:SA.

### 📦 BitStream binary serialization

Встроенный `BitStream` предназначен для точной бинарной сериализации:

- запись данных на уровне байтов и битов;
- чтение произвольных bit-length последовательностей;
- динамическое расширение буфера;
- контроль границ потока;
- подготовка бинарных payload'ов, совместимых с серверным форматом.

Это особенно важно для RakNet/SA-MP, где структура пакета и точное расположение полей имеют значение.

### 🌐 Cross-platform

Проект рассчитан на сборку под:

- Linux + GCC;
- Linux + Clang;
- Windows + MSVC;
- Windows через MinGW-w64;
- CMake как единая система конфигурации.

### 💬 RPC и Dialog subsystem

В прикладном слое присутствует обработка SA-MP RPC:

- диспетчеризация RPC;
- обработка `ShowDialog`;
- разбор строковых payload'ов;
- построение ответов;
- работа с бинарными структурами протокола.

### 🏃 Player synchronization

Реализован генератор OnFoot synchronization frames.

Система формирует raw synchronization packets с заданным интервалом, в исходной спецификации — примерно **10 Hz**.

### 🔐 Authentication / handshake

Сетевой слой содержит необходимые механизмы для SA-MP handshake:

- challenge-response flow;
- преобразование AuthKey;
- CRC32;
- генерацию Version 4057 handshake;
- ClientJoin payload.

---

# 🏗 Architecture

Архитектура проекта построена по принципу разделения ответственности: низкоуровневый transport layer не смешивается с SA-MP state machine и прикладной логикой.

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

## 🧩 Основные модули

| Модуль | Назначение |
|---|---|
| `core/` | Runtime context, конфигурация и orchestration потоков |
| `net/` | UDP/RakNet transport и бинарная сериализация |
| `samp/` | SA-MP protocol logic, RPC, dialogs и sync |
| `utils/` | Вспомогательная математика и криптографические операции |

### `core/`

Содержит управляющий слой приложения.

- `Application.h` — основной execution loop и управление состояниями.
- `Config.h` — CLI flags и runtime configuration.
- `Logger.h` — потокобезопасный цветной console logger.

### `net/`

Низкоуровневый сетевой слой.

- `Socket.h` — cross-platform non-blocking UDP wrapper.
- `RakClient.h` — управление RakNet connection state и dispatch.
- `BitStream.h` — binary reader/writer.
- `NetworkTypes.h` — network address и packet abstractions.
- `PacketEnumerations.h` — packet IDs и priority/reliability definitions.

### `samp/`

SA-MP domain layer.

- `DialogManager.h` — обработка входящих dialog payloads.
- `RPCManager.h` — RPC dispatch matrix.
- `PlayerPed.h` — локальное состояние игрока.
- `SAMPPacket.h` — конструкторы handshake-пакетов.
- `SyncStructures.h` — OnFoot/Vehicle sync structures.
- `SAMPDefines.h` — версии NetGame и RPC ID mappings.

### `utils/`

Вспомогательные алгоритмы.

- `Crypto.h` — AuthKey XOR transformation и CRC32.
- `Vector3.h` — 3D coordinates и vector operations.

---

# 🔬 Protocol Deep-Dive

## 🤝 Handshake Sequence

`Samp-rakdriver` реализует последовательность подключения, соответствующую SA-MP 0.3.7-R1 в описанной спецификации.

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

Последовательность можно концептуально разделить на следующие стадии:

1. **RakNet connection request**
2. **Connection acceptance**
3. **SA-MP ClientJoin**
4. **Authentication / AuthKey**
5. **Server initialization через `RPC_InitGame`**
6. **Обработка optional dialog**
7. **Отправка player synchronization**
8. **Продолжение сетевого цикла**

---

# 📦 Binary Serialization

Все важные outgoing payload structures используют packed layouts, чтобы бинарное представление соответствовало ожидаемому сервером формату.

Пример отправки OnFoot synchronization frame:

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

### Почему BitStream важен?

Обычная сериализация C++-структуры не всегда подходит для сетевого протокола. В SA-MP/RakNet критичны:

- точный порядок полей;
- размер каждого поля;
- alignment;
- количество передаваемых бит;
- порядок байтов;
- тип reliability/priority;
- структура packet header.

Поэтому `BitStream` выступает промежуточным уровнем между C++ structures и фактическим UDP payload.

---

# 🧱 Synchronization

Для OnFoot synchronization используется отдельная структура состояния игрока.

Поток синхронизации формирует пакет примерно **10 раз в секунду**, то есть с интервалом порядка 100 мс.

Типичный цикл выглядит следующим образом:

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

В исходной спецификации размер raw OnFoot synchronization frame указан как **68 bytes**.

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

Клонирование проекта:

```bash
git clone https://github.com/your-username/samp-rakdriver.git
cd samp-rakdriver/cpp
```

Создание отдельного build directory:

```bash
mkdir build
cd build
```

Конфигурация Release build:

```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
```

Компиляция:

```bash
make -j$(nproc)
```

После успешной сборки executable `rakbot` будет находиться в `build/`.

---

## 🪟 Windows через MinGW-w64

Установите `mingw-w64`, затем:

```bash
cd cpp
mkdir build_win
cd build_win
```

Настройте CMake под Windows target:

```bash
cmake \
  -DCMAKE_SYSTEM_NAME=Windows \
  -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ \
  -DCMAKE_BUILD_TYPE=Release \
  ..
```

Соберите проект:

```bash
make -j$(nproc)
```

В результате будет создан:

```text
rakbot.exe
```

---

## 🪟 Native Windows / Visual Studio

Откройте **Developer Command Prompt for Visual Studio**:

```dos
cd cpp
mkdir build
cd build
```

Сгенерируйте Makefiles:

```dos
cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release ..
```

Соберите проект:

```dos
nmake
```

---

# 💻 Usage

После сборки приложение запускается с параметрами целевого SA-MP сервера:

```bash
./rakbot <IPv4_Address> <Port> <Nickname>
```

### Example

Подключение к локальному серверу на стандартном порту:

```bash
./rakbot 127.0.0.1 7777 Driver_Node
```

## CLI Arguments

| Argument | Type | Default | Description |
|---|---|---|---|
| `argv[1]` | `std::string` | `127.0.0.1` | IPv4-адрес SA-MP сервера |
| `argv[2]` | `uint16_t` | `7777` | UDP port сервера |
| `argv[3]` | `std::string` | `RakBot` | Nickname / account identity |

Nickname передаётся в `ClientJoin` payload как идентификатор клиента.

---

# 🔄 Runtime Flow

Упрощённая модель работы приложения:

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

Такое разделение позволяет изолировать transport concerns от SA-MP-specific logic и облегчает расширение проекта.

---

# 📊 Performance

Согласно предоставленной спецификации проекта:

| Metric | Value |
|---|---:|
| Memory usage | ~3.8 MB RSS / client |
| CPU footprint | <0.1% single-thread core load |
| Network I/O | ~1.2 KB/s during active sync |
| Sync frequency | ~10 Hz |

Фактические значения зависят от ОС, компилятора, настроек оптимизации, количества клиентов и конкретного сетевого сценария.

---

# 🛡 Design Goals

Проект ориентирован на несколько ключевых принципов:

### Low-level control

Минимум абстракций между приложением и сетевым протоколом позволяет исследовать и контролировать бинарные packets непосредственно.

### Low overhead

Отсутствие полноценного GTA:SA runtime позволяет существенно уменьшить потребление ресурсов по сравнению с запуском обычного игрового клиента.

### Modularity

Transport, serialization, SA-MP logic и utilities разделены по отдельным модулям.

### Portability

CMake и platform-specific socket abstraction позволяют использовать одну кодовую базу на Linux и Windows.

### Extensibility

RPC manager, dialog subsystem и synchronization structures позволяют добавлять новые protocol handlers без изменения базового socket layer.

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

Для GitHub репозитория рекомендуется сохранить следующую структуру:

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

README предполагает, что основной C++ проект находится внутри директории `cpp/`.

---

# 📜 License

This project is licensed under the **MIT License**.

Подробности находятся в файле [`LICENSE`](LICENSE).

---

<div align="center">

**Built with precision C++17 for low-latency SA-MP/RakNet network research and automation.**

</div>
