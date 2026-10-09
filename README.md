# SmartTrafficSimulation


## Module Concurrency / Thread

─É├óy l├á module **Ng╞░ß╗¥i 3 ΓÇô Concurrency / Thread** cß╗ºa project m├┤ phß╗Ång giao th├┤ng th├┤ng minh. Module chß╗ïu tr├ích nhiß╗çm quß║ún l├╜ thread m├┤ phß╗Ång v├á ─æß╗ông bß╗Ö dß╗» liß╗çu giß╗»a Simulation Core vß╗¢i c├íc th├ánh phß║ºn ─æß╗ìc snapshot.

Module n├áy kh├┤ng sß╗ƒ hß╗»u logic ch├¡nh cß╗ºa `Vehicle`, `TrafficLight` hoß║╖c luß║¡t giao th├┤ng. Core/Logic vß║½n l├á n╞íi cß║¡p nhß║¡t trß║íng th├íi thß║¡t; module Concurrency chß╗ë quß║ún l├╜ luß╗ông, l╞░u trß╗» v├á cung cß║Ñp bß║ún sao dß╗» liß╗çu an to├án.

## Chß╗⌐c n─âng ch├¡nh

Module cung cß║Ñp:

- `SimulationThread`: chß║íy callback m├┤ phß╗Ång tr├¬n worker thread.
- `SnapshotStore`: l╞░u v├á ─æß╗ông bß╗Ö snapshot bß║▒ng `mutex` v├á `lock_guard`.
- `SimulationSnapshot`: hß╗úp ─æß╗ông dß╗» liß╗çu giß╗»a Simulation Core v├á th├ánh phß║ºn hiß╗ân thß╗ï.
- `Types.h`: n╞íi ─æß╗ïnh ngh─⌐a tß║¡p trung c├íc enum d├╣ng chung.
- `TurnCurve.h`: t├¡nh quß╗╣ ─æß║ío quay ─æß║ºu bß║▒ng ─æ╞░ß╗¥ng cong B├⌐zier bß║¡c hai.
- C╞í chß║┐ dß╗½ng v├á `join()` thread an to├án.
- Bß║úo vß╗ç tr╞░ß╗¥ng hß╗úp gß╗ìi `start()` nhiß╗üu lß║ºn.
- Kiß╗âm tra race condition bß║▒ng ThreadSanitizer.

## Cß║Ñu tr├║c th╞░ mß╗Ñc

```text
SmartTrafficSimulation/
Γö£ΓöÇΓöÇ .gitignore
Γö£ΓöÇΓöÇ README.md
Γö£ΓöÇΓöÇ CONCURRENCY.md
Γö£ΓöÇΓöÇ examples/
Γöé   ΓööΓöÇΓöÇ ConcurrencyPrototype.cpp
ΓööΓöÇΓöÇ src/
    Γö£ΓöÇΓöÇ ConcurrencyModule.cpp
    Γö£ΓöÇΓöÇ common/
    Γöé   Γö£ΓöÇΓöÇ Types.h
    Γöé   ΓööΓöÇΓöÇ TurnCurve.h
    ΓööΓöÇΓöÇ concurrency/
        Γö£ΓöÇΓöÇ CMakeLists.txt
        Γö£ΓöÇΓöÇ SimulationSnapshot.h
        Γö£ΓöÇΓöÇ SnapshotStore.h
        Γö£ΓöÇΓöÇ SnapshotStore.cpp
        Γö£ΓöÇΓöÇ SimulationThread.h
        ΓööΓöÇΓöÇ SimulationThread.cpp
```

`README.md` l├á t├ái liß╗çu tß╗òng quan cß╗ºa project. `CONCURRENCY.md` l├á t├ái liß╗çu chi tiß║┐t ri├¬ng cho module Concurrency nß║┐u nh├│m giß╗» file n├áy.

## Quy ╞░ß╗¢c kiß╗âu dß╗» liß╗çu d├╣ng chung

C├íc enum d├╣ng chung chß╗ë ─æ╞░ß╗úc ─æß╗ïnh ngh─⌐a mß╗Öt lß║ºn trong:

```text
src/common/Types.h
```

Nß╗Öi dung hiß╗çn tß║íi:

```cpp
#pragma once

enum class LightColor { Red, Yellow, Green };
enum class Direction { North, South, East, West };
enum class Movement { Straight, Left, Right, UTurn };
```

C├íc file kh├íc phß║úi include:

```cpp
#include "common/Types.h"
```

Kh├┤ng ─æß╗ïnh ngh─⌐a lß║íi `LightColor`, `Direction` hoß║╖c `Movement` trong c├íc module kh├íc, v├¼ sß║╜ g├óy lß╗ùi ─æß╗ïnh ngh─⌐a tr├╣ng khi t├¡ch hß╗úp.

## Snapshot data contract

### `VehicleSnapshot`

```cpp
struct VehicleSnapshot {
    int vehicleId = 0;
    double x = 0.0;
    double y = 0.0;
    Direction direction = Direction::North;
    Movement movement = Movement::Straight;
    bool signalingLeft = false;
    double rotationAngleDegrees = 0.0;
};
```

├¥ ngh─⌐a c├íc tr╞░ß╗¥ng:

- `vehicleId`: m├ú xe.
- `x`, `y`: vß╗ï tr├¡ xe.
- `direction`: h╞░ß╗¢ng di chuyß╗ân ch├¡nh.
- `movement`: ─æi thß║│ng, rß║╜ tr├íi, rß║╜ phß║úi hoß║╖c quay ─æß║ºu.
- `signalingLeft`: xe c├│ ─æang bß║¡t xi-nhan tr├íi hay kh├┤ng.
- `rotationAngleDegrees`: g├│c xoay cß╗ºa sprite/xe khi hiß╗ân thß╗ï.

Khi tß║ío snapshot tß╗½ `Vehicle` thß║¡t:

```cpp
VehicleSnapshot snapshot{
    vehicle.getId(),
    vehicle.getX(),
    vehicle.getY(),
    vehicle.getDirection(),
    vehicle.getMovement(),
    vehicle.isSignalingLeft(),
    vehicle.getRotationAngleDegrees()
};
```

### `LightSnapshot`

```cpp
struct LightSnapshot {
    Direction direction = Direction::North;
    bool turnArrow = false;
    LightColor color = LightColor::Red;
};
```

`turnArrow` phß║úi ─æ╞░ß╗úc ─æß║╖t tr╞░ß╗¢c `color` khi d├╣ng aggregate initializer:

```cpp
LightSnapshot lightSnapshot{
    Direction::North,
    true,
    LightColor::Green
};
```

`turnArrow` biß╗âu thß╗ï m┼⌐i t├¬n rß║╜ ─æang ─æ╞░ß╗úc ph├⌐p theo pha ─æ├¿n. Logic quyß║┐t ─æß╗ïnh gi├í trß╗ï n├áy thuß╗Öc vß╗ü Traffic Light/Core.

### `SimulationSnapshot`

```cpp
struct SimulationSnapshot {
    std::size_t tick = 0;
    std::vector<VehicleSnapshot> vehicles;
    std::vector<LightSnapshot> lights;
};
```

`SnapshotStore::getSnapshot()` trß║ú vß╗ü bß║ún sao dß╗» liß╗çu. Th├ánh phß║ºn ─æß╗ìc snapshot kh├┤ng giß╗» mutex trong l├║c xß╗¡ l├╜ hoß║╖c render.

## `SimulationThread`

API ch├¡nh:

```cpp
class SimulationThread {
public:
    using TickFunction = std::function<void(std::size_t)>;

    SimulationThread(
        TickFunction tickFunction,
        std::chrono::milliseconds tickInterval
    );

    ~SimulationThread();

    void start();
    void requestStop();
    void join();
    bool isRunning() const;
};
```

### Khß╗ƒi ─æß╗Öng v├á dß╗½ng

```cpp
SimulationThread simulationThread(
    [&engine](std::size_t tick) {
        engine.update(tick);
    },
    std::chrono::milliseconds(20)
);

simulationThread.start();

// Khi ─æ├│ng ß╗⌐ng dß╗Ñng:
simulationThread.requestStop();
simulationThread.join();
```

Destructor c┼⌐ng gß╗ìi `requestStop()` v├á `join()` ─æß╗â tr├ính worker thread c├▓n chß║íy khi object bß╗ï hß╗ºy.

### Gß╗ìi `start()` lß║ºn hai

Nß║┐u thread ─æang chß║íy, gß╗ìi `start()` lß║ºn hai sß║╜ bß╗ï tß╗½ chß╗æi bß║▒ng exception:

```cpp
try {
    simulationThread.start();
} catch (const std::logic_error& error) {
    std::cerr << error.what() << '\n';
}
```

C├ích n├áy tr├ính viß╗çc tß║ío thread tr├╣ng hoß║╖c l├ám ch╞░╞íng tr├¼nh gß╗ìi `std::terminate()`.

Sau khi ─æ├ú dß╗½ng v├á `join()`, thread c├│ thß╗â ─æ╞░ß╗úc khß╗ƒi ─æß╗Öng lß║íi an to├án:

```cpp
simulationThread.requestStop();
simulationThread.join();

simulationThread.start();
```

## `SnapshotStore`

API sß╗¡ dß╗Ñng:

```cpp
snapshotStore.publishTick(currentTick);
snapshotStore.publishVehicles(vehicleSnapshots);
snapshotStore.publishLights(lightSnapshots);
```

Lß║Ñy snapshot:

```cpp
const SimulationSnapshot snapshot =
snapshotStore.getSnapshot();
```

`SnapshotStore` sß╗¡ dß╗Ñng:

- `vehicleMutex` ─æß╗â bß║úo vß╗ç danh s├ích xe.
- `lightMutex` ─æß╗â bß║úo vß╗ç danh s├ích ─æ├¿n.
- `lock_guard` ─æß╗â kh├│a tß╗▒ ─æß╗Öng v├á mß╗ƒ kh├│a khi ra khß╗Åi scope.
- `atomic` ─æß╗â l╞░u tick hiß╗çn tß║íi.

Khi lß║Ñy cß║ú xe v├á ─æ├¿n, thß╗⌐ tß╗▒ kh├│a cß╗æ ─æß╗ïnh l├á:

```text
lightMutex -> vehicleMutex
```

Mß╗ìi code mß╗ƒ rß╗Öng `SnapshotStore` phß║úi giß╗» c├╣ng thß╗⌐ tß╗▒ ─æß╗â tr├ính deadlock.

## Luß╗ông cß║¡p nhß║¡t dß╗» liß╗çu

Luß╗ông dß╗» liß╗çu dß╗▒ kiß║┐n:

```text
SimulationThread
        |
        v
SimulationEngine::update()
        |
        +--> Vehicle / TrafficLight thß║¡t
        |
        +--> publishVehicles()
        +--> publishLights()
        +--> publishTick()
        |
        v
SnapshotStore
        |
        v
getSnapshot()
```

V├¡ dß╗Ñ trong Simulation Engine:

```cpp
void SimulationEngine::update(std::size_t currentTick) {
    updateVehicles();
    updateTrafficLights();

    std::vector<VehicleSnapshot> vehicleSnapshots;
    std::vector<LightSnapshot> lightSnapshots;

    for (const Vehicle& vehicle : vehicles) {
        vehicleSnapshots.push_back({
            vehicle.getId(),
            vehicle.getX(),
            vehicle.getY(),
            vehicle.getDirection(),
            vehicle.getMovement(),
            vehicle.isSignalingLeft(),
            vehicle.getRotationAngleDegrees()
        });
    }

    for (const TrafficLight& light : trafficLights) {
        lightSnapshots.push_back({
            light.getDirection(),
            light.hasTurnArrow(),
            light.getColor()
        });
    }

    snapshotStore.publishTick(currentTick);
    snapshotStore.publishVehicles(vehicleSnapshots);
    snapshotStore.publishLights(lightSnapshots);
}
```

## Quß╗╣ ─æß║ío xe quay ─æß║ºu

File:

```text
src/common/TurnCurve.h
```

`TurnCurve` sß╗¡ dß╗Ñng ─æ╞░ß╗¥ng cong B├⌐zier bß║¡c hai:

```cpp
const TurnCurveSample sample =
    TurnCurve::sampleQuadraticBezier(
        startPoint,
        controlPoint,
        endPoint,
        turnProgress
    );
```

Trong ─æ├│ `turnProgress` nß║▒m trong khoß║úng `0.0` ─æß║┐n `1.0`.

- `0.0`: bß║»t ─æß║ºu quay.
- `1.0`: kß║┐t th├║c quay.
- `controlPoint`: quyß║┐t ─æß╗ïnh ─æß╗Ö cong cß╗ºa quß╗╣ ─æß║ío.

Kß║┐t quß║ú trß║ú vß╗ü:

```cpp
struct TurnCurveSample {
    Point2D position;
    double rotationAngleDegrees = 0.0;
};
```

Cß║¡p nhß║¡t v├áo snapshot:

```cpp
vehicleSnapshot.x = sample.position.x;
vehicleSnapshot.y = sample.position.y;
vehicleSnapshot.rotationAngleDegrees =
    sample.rotationAngleDegrees;
```

G├│c xoay ─æ╞░ß╗úc t├¡nh tß╗½ vector tiß║┐p tuyß║┐n cß╗ºa ─æ╞░ß╗¥ng cong, v├¼ vß║¡y xe thay ─æß╗òi h╞░ß╗¢ng tß╗▒ nhi├¬n trong l├║c di chuyß╗ân.

## Build trß╗▒c tiß║┐p bß║▒ng g++

Do project module hiß╗çn kh├┤ng giß╗» `CMakeLists.txt` ß╗ƒ th╞░ mß╗Ñc gß╗æc, c├│ thß╗â build prototype trß╗▒c tiß║┐p bß║▒ng g++.

### Prototype module h├│a

Linux/macOS/WSL:

```bash
g++ -std=c++17 -pthread -Wall -Wextra -Wpedantic \
  src/concurrency/SimulationThread.cpp \
  src/concurrency/SnapshotStore.cpp \
  examples/ConcurrencyPrototype.cpp \
  -Isrc \
  -o concurrency_prototype
```

Chß║íy:

```bash
./concurrency_prototype
```

Windows PowerShell d├╣ng MinGW:

```powershell
g++ -std=c++17 -pthread -Wall -Wextra -Wpedantic `
  src/concurrency/SimulationThread.cpp `
  src/concurrency/SnapshotStore.cpp `
  examples/ConcurrencyPrototype.cpp `
  -Isrc `
  -o concurrency_prototype.exe
```

Chß║íy:

```powershell
.\concurrency_prototype.exe
```

### Prototype mß╗Öt file

```bash
g++ -std=c++17 -pthread -Wall -Wextra -Wpedantic \
  src/ConcurrencyModule.cpp \
  -Isrc \
  -o concurrency_single_file
```

## ThreadSanitizer

Build bß║ún kiß╗âm tra race condition:

```bash
g++ -fsanitize=thread \
  -fno-omit-frame-pointer \
  -g \
  -std=c++17 \
  -pthread \
  src/concurrency/SimulationThread.cpp \
  src/concurrency/SnapshotStore.cpp \
  examples/ConcurrencyPrototype.cpp \
  -Isrc \
  -o concurrency_tsan
```

Chß║íy:

```bash
./concurrency_tsan
```

Kß║┐t quß║ú ─æß║ít y├¬u cß║ºu l├á ch╞░╞íng tr├¼nh kh├┤ng xuß║Ñt hiß╗çn:

```text
WARNING: ThreadSanitizer: data race
```

## CMake khi t├¡ch hß╗úp v├áo project ch├¡nh

File module:

```text
src/concurrency/CMakeLists.txt
```

Project ch├¡nh cß╗ºa nh├│m cß║ºn tß╗▒ khai b├ío:

```cmake
find_package(Threads REQUIRED)
add_subdirectory(src/concurrency)

target_link_libraries(SmartTrafficSimulation
    PRIVATE concurrency
)
```

Kh├┤ng th├¬m `examples/ConcurrencyPrototype.cpp` hoß║╖c `src/ConcurrencyModule.cpp` v├áo executable ch├¡nh nß║┐u executable ch├¡nh ─æ├ú c├│ `main.cpp` ri├¬ng.

## Quy tß║»c t├¡ch hß╗úp Git

Branch l├ám viß╗çc cß╗ºa module:

```text
feature/concurrency
```

Cß║¡p nhß║¡t branch:

```bash
git checkout feature/concurrency
git fetch origin
git pull --rebase origin feature/concurrency
```

Commit thay ─æß╗òi:

```bash
git add .
git commit -m "feat: update concurrency snapshot contract"
```

Push branch:

```bash
git push origin feature/concurrency
```

Sau ─æ├│ tß║ío Pull Request:

```text
base: main
compare: feature/concurrency
```

Nß║┐u Pull Request b├ío conflict, cß║¡p nhß║¡t `main` v├áo branch c├í nh├ón:

```bash
git checkout feature/concurrency
git fetch origin
git merge origin/main
```

Sß╗¡a c├íc file conflict, sau ─æ├│:

```bash
git add .
git commit -m "merge: resolve conflicts with main"
git push origin feature/concurrency
```

Kh├┤ng sß╗¡ dß╗Ñng `git push --force` l├¬n `main`.

## Checklist b├án giao

- [ ] `Types.h` l├á n╞íi duy nhß║Ñt ─æß╗ïnh ngh─⌐a c├íc enum d├╣ng chung.
- [ ] `SimulationSnapshot.h` include `common/Types.h`.
- [ ] `VehicleSnapshot` c├│ `signalingLeft`.
- [ ] `LightSnapshot` c├│ `turnArrow` tr╞░ß╗¢c `color`.
- [ ] `SimulationThread` c├│ `start()`, `requestStop()` v├á `join()` an to├án.
- [ ] Gß╗ìi `start()` lß║ºn hai ─æ╞░ß╗úc xß╗¡ l├╜ bß║▒ng exception, kh├┤ng crash.
- [ ] Snapshot ─æ╞░ß╗úc publish bß║▒ng `SnapshotStore`.
- [ ] Kh├┤ng truy cß║¡p trß╗▒c tiß║┐p vector d├╣ng chung tß╗½ thread kh├íc.
- [ ] Kh├┤ng c├▓n `CMakeLists.txt` ß╗ƒ th╞░ mß╗Ñc gß╗æc nß║┐u project ch├¡nh ─æ├ú quß║ún l├╜ CMake.
- [ ] Chß║íy build vß╗¢i `-Wall -Wextra -Wpedantic` th├ánh c├┤ng.
- [ ] Chß║íy ThreadSanitizer kh├┤ng ph├ít hiß╗çn data race.
- [ ] Commit ─æ├ú ─æ╞░ß╗úc push l├¬n `feature/concurrency`.
=======
Phß║ºn mß╗üm **m├┤ phß╗Ång giao th├┤ng ─æ├┤ thß╗ï thß╗¥i gian thß╗▒c** viß║┐t bß║▒ng C++ cho m├┤n Lß║¡p tr├¼nh H╞░ß╗¢ng ─æß╗æi t╞░ß╗úng (OOP).

Xe, ─æ├¿n giao th├┤ng v├á giao lß╗Ö ─æ╞░ß╗úc m├┤ h├¼nh h├│a th├ánh c├íc ─æß╗æi t╞░ß╗úng tß╗▒ vß║¡n h├ánh, c├│ khß║ú n─âng tß╗▒ ─æiß╗üu chß╗ënh h├ánh vi.

## Kiß║┐n tr├║c v├á kß╗╣ thuß║¡t ch├¡nh

- **State Pattern**: trß║íng th├íi ─æ├¿n giao th├┤ng v├á h├ánh vi cß╗ºa xe
- **Composite Pattern**: cß║Ñu tr├║c mß║íng l╞░ß╗¢i ─æ╞░ß╗¥ng
- **Multithreading**: xß╗¡ l├╜ ─æß╗ông thß╗¥i thß║¡t bß║▒ng `std::thread`, `mutex`, `lock_guard`
- **SFML**: hiß╗ân thß╗ï ─æß╗ô hß╗ìa

## C├íc class ch├¡nh

| Phß║ºn | Class |
|---|---|
| Xe | `Vehicle` |
| ─É├¿n giao th├┤ng | `TrafficLight` |
| Trß║íng th├íi xe / ─æ├¿n | `IVehicleState` / `ILightState` |
| ─É╞░ß╗¥ng / Ng├ú t╞░ | `Road` / `Intersection` |
| Th├ánh phß║ºn ─æ╞░ß╗¥ng (Composite) | `IRoadComponent` |
| Mß║íng ─æ╞░ß╗¥ng | `RoadNetwork` |
| ─Éß╗ông hß╗ô m├┤ phß╗Ång | `SimulationClock` |
| Hß╗ç thß╗æng sß╗▒ kiß╗çn | `EventBus` |
| Bß╗Ö ─æiß╗üu khiß╗ân m├┤ phß╗Ång | `SimulationEngine` |
| Sinh xe | `TrafficGenerator` |
| Hiß╗ân thß╗ï | `Renderer` |

**State cß╗ºa xe:** `AcceleratingState`, `BrakingState`, `WaitingState`, `TurningState`, `CruisingState`
**State cß╗ºa ─æ├¿n:** `RedState`, `YellowState`, `GreenState`

## M├┤ h├¼nh ─æ├ú chß╗æt (theo bi├¬n bß║ún hß╗ìp)

| Hß║íng mß╗Ñc | Nß╗Öi dung |
|---|---|
| Loß║íi m├┤ h├¼nh | Ng├ú t╞░ cß║»t ngang bß╗ƒi ─æ╞░ß╗¥ng ─æ├┤i |
| Sß╗æ ng├ú t╞░ | 1 |
| ─É╞░ß╗¥ng ─æ╞ín (h╞░ß╗¢ng Bß║»c-Nam) | 2 chiß╗üu, mß╗ùi chiß╗üu 1 l├án (tß╗òng 2 l├án). Mß╗ùi l├án ─æß╗º rß╗Öng cho 2 xe ─æß╗⌐ng song song: mß╗Öt h├áng cho rß║╜ tr├íi + quay ─æß║ºu, mß╗Öt h├áng cho ─æi thß║│ng + rß║╜ phß║úi |
| ─É╞░ß╗¥ng ─æ├┤i (h╞░ß╗¢ng ─É├┤ng-T├óy) | C├│ dß║úi ph├ón c├ích, mß╗ùi chiß╗üu 3 l├án (tß╗òng 6 l├án). L├án tr├íi: rß║╜ tr├íi + quay ─æß║ºu, l├án giß╗»a: ─æi thß║│ng, l├án phß║úi: rß║╜ phß║úi |
| H╞░ß╗¢ng xe | ─Éi thß║│ng, rß║╜ tr├íi, rß║╜ phß║úi, quay ─æß║ºu |
| Chuyß╗ân l├án | Kh├┤ng cho ph├⌐p, xe ─æi theo l├án cß╗æ ─æß╗ïnh |
| Loß║íi xe | Chß╗ë ├┤ t├┤ |
| Sß╗æ xe | 13 xe ban ─æß║ºu, cß╗æ ─æß╗ïnh, kh├┤ng sinh th├¬m |
| Tß╗æc ─æß╗Ö xe | T├╣y chß╗ënh ─æ╞░ß╗úc |
| C├ích rß║╜ | Rß║╜ theo ─æ╞░ß╗¥ng cong |
| ╞»u ti├¬n l╞░u th├┤ng | Theo t├¡n hiß╗çu ─æ├¿n |
| B╞░ß╗¢c m├┤ phß╗Ång | 1 tick = 0.02 gi├óy (50 tick = 1 gi├óy) |
| EventBus | ─Éß╗â sau, ch╞░a d├╣ng ß╗ƒ bß║ún MVP |
| Luß╗ông chß║íy | Luß╗ông m├┤ phß╗Ång (cß║¡p nhß║¡t xe + ─æ├¿n) v├á luß╗ông ch├¡nh (SFML: cß╗¡a sß╗ò + vß║╜) |
| Hiß╗ân thß╗ï | SFML, c├│ xi-nhan cho rß║╜ tr├íi v├á quay ─æß║ºu |
| Phß║ím vi bß║ún ─æß║ºu (MVP) | M├┤ h├¼nh chß║íy ─æ╞░ß╗úc vß╗¢i c├íc mß╗Ñc tr├¬n |

### Quy tß║»c xe theo ─æ├¿n

- Xe ch╞░a qua vß║ích dß╗½ng khi ─æ├¿n chuyß╗ân pha: giß║úm tß╗æc v├á dß╗½ng lß║íi.
- Xe ─æ├ú qua vß║ích dß╗½ng: tiß║┐p tß╗Ñc ─æi, kh├┤ng giß║úm tß╗æc.
- Xe chß╗¥ ─æ├¿n ─æß╗Å xß║┐p h├áng tß║íi vß║ích dß╗½ng theo l├án.

### Chu kß╗│ ─æ├¿n (38 gi├óy)

Thß╗⌐ tß╗▒ mß╗ùi h╞░ß╗¢ng: **Xanh, V├áng, ─Éß╗Å**. Hai nh├│m h╞░ß╗¢ng ─æi chung pha: **thß║│ng + rß║╜ phß║úi** v├á **rß║╜ tr├íi + quay ─æß║ºu**.
K├╜ hiß╗çu: **BN** = Bß║»c-Nam, **─ÉT** = ─É├┤ng-T├óy.
Thß╗¥i l╞░ß╗úng ─æ├¿n theo bi├¬n bß║ún hß╗ìp. N├¬n khai b├ío th├ánh hß║▒ng sß╗æ ─æß╗â sau n├áy ─æß╗òi sß╗æ m├á kh├┤ng sß╗¡a logic.

| Gi├óy | Pha ─æang chß║íy |
|---|---|
| 0-5 | BN thß║│ng + rß║╜ phß║úi: Xanh |
| 6-7 | BN thß║│ng + rß║╜ phß║úi: V├áng |
| 8-9 | ─Éß╗Å to├án bß╗Ö |
| 10-14 | BN rß║╜ tr├íi + quay ─æß║ºu: Xanh |
| 15-16 | BN rß║╜ tr├íi + quay ─æß║ºu: V├áng |
| 17-18 | ─Éß╗Å to├án bß╗Ö |
| 19-24 | ─ÉT thß║│ng + rß║╜ phß║úi: Xanh |
| 25-26 | ─ÉT thß║│ng + rß║╜ phß║úi: V├áng |
| 27-28 | ─Éß╗Å to├án bß╗Ö |
| 29-33 | ─ÉT rß║╜ tr├íi + quay ─æß║ºu: Xanh |
| 34-35 | ─ÉT rß║╜ tr├íi + quay ─æß║ºu: V├áng |
| 36-37 | ─Éß╗Å to├án bß╗Ö |
| 38 | Quay lß║íi gi├óy 0 |

### Ch╞░a chß╗æt

Hiß╗çn kh├┤ng c├▓n mß╗Ñc n├áo.

## Ph├ón c├┤ng

| Th├ánh vi├¬n | Phß║ºn viß╗çc | Branch |
|---|---|---|
| Ng╞░ß╗¥i 1 | Core Simulation (Vehicle, TrafficLight), slide thuyß║┐t tr├¼nh | `feature/vehicle` |
| Ng╞░ß╗¥i 2 | Logic / Thuß║¡t to├ín ─æiß╗üu phß╗æi giao th├┤ng | `feature/logic` |
| Ng╞░ß╗¥i 3 | Concurrency / Thread | `feature/concurrency` |
| Ng╞░ß╗¥i 4 | T├¡ch hß╗úp, kiß╗âm thß╗¡, review code | `feature/integration` |
| Ng╞░ß╗¥i 5 | GUI (SFML) | `feature/gui` |

> Ghi t├¬n th├ánh vi├¬n v├áo bß║úng n├áy khi nh├│m chß╗æt.

## Mß╗æc thß╗¥i gian

| Mß╗æc | Nß╗Öi dung |
|---|---|
| 21/9 | Khß╗ƒi ─æß╗Öng, thß╗æng nhß║Ñt cß║Ñu tr├║c v├á quy ╞░ß╗¢c |
| 28/9 | Cß║Ñu tr├║c project v├á prototype thread chß║íy ─æß╗Öc lß║¡p |
| 12/10 | Core Simulation ß╗òn ─æß╗ïnh, b├án giao t├¡ch hß╗úp |
| 19/10 | T├¡ch hß╗úp lß║ºn 1, GUI kß║┐t nß╗æi dß╗» liß╗çu thß║¡t |
| 25/10 | Test to├án hß╗ç thß╗æng, sß╗¡a lß╗ùi |
| 30/10 | Freeze bß║ún ch├¡nh thß╗⌐c |
| 31/10 | Demo v├á thuyß║┐t tr├¼nh |

## Cß║Ñu tr├║c th╞░ mß╗Ñc

```
SmartTrafficSimulation/
Γö£ΓöÇΓöÇ src/        # m├ú nguß╗ôn (.h v├á .cpp)
Γö£ΓöÇΓöÇ docs/       # b├ío c├ío, slide
Γö£ΓöÇΓöÇ .gitignore
ΓööΓöÇΓöÇ README.md
```

## C├ích build

**Y├¬u cß║ºu:** C++17 (GCC 13 trß╗ƒ l├¬n hoß║╖c MSVC 2022), CMake 3.16 trß╗ƒ l├¬n, Ninja (Windows: `winget install Ninja-build.Ninja`).

```bash
cmake -S . -B build -G Ninja -DBUILD_GUI=OFF
cmake --build build
ctest --test-dir build --output-on-failure
```

- `-DBUILD_GUI=OFF`: bß╗Å qua phß║ºn ─æß╗ô hß╗ìa. Khi cß║ºn GUI, bß╗Å tham sß╗æ n├áy, CMake sß║╜ tß╗▒ tß║úi **SFML 3** (lß║ºn ─æß║ºu cß║ºn c├│ mß║íng).
- Dß╗▒ ├ín d├╣ng chung kiß╗âu dß╗» liß╗çu trong `src/common/Types.h`. Kh├┤ng tß╗▒ ─æß╗ïnh ngh─⌐a lß║íi `Direction`, `Movement`, `LightColor`.
- Kh├┤ng commit th╞░ mß╗Ñc `build/` v├á file `.exe`.

## Quy ╞░ß╗¢c ─æß║╖t t├¬n

| Th├ánh phß║ºn | Quy tß║»c | V├¡ dß╗Ñ |
|---|---|---|
| File | PascalCase, tr├╣ng t├¬n class | `Vehicle.h`, `Vehicle.cpp` |
| Class | PascalCase | `TrafficLight` |
| State class | PascalCase + hß║¡u tß╗æ `State` | `RedState` |
| H├ám | camelCase | `update()`, `addComponent()` |
| Biß║┐n | camelCase | `vehicleId`, `currentTick` |
| Hß║▒ng sß╗æ | CHß╗«_HOA_Gß║áCH_D╞»ß╗ÜI | `MAX_VEHICLES` |
| Enum | PascalCase | `enum class Direction { North, South, East, West };` |

- File `.h` ─æß╗â khai b├ío, `.cpp` ─æß╗â c├ái ─æß║╖t.
- D├╣ng `#pragma once` cho mß╗ìi header.
- Chß╗ë `#include` th╞░ viß╗çn cß║ºn thiß║┐t.
- Comment ─æß╗â giß║úi th├¡ch logic kh├│ hiß╗âu, kh├┤ng comment nhß╗»ng d├▓ng qu├í hiß╗ân nhi├¬n.

## Quy tß║»c Git

1. Mß╗ùi ng╞░ß╗¥i l├ám tr├¬n **branch ri├¬ng**, chß╗ë push v├áo branch cß╗ºa m├¼nh.
2. **Kh├┤ng push thß║│ng l├¬n `main`.** Muß╗æn ─æ╞░a code v├áo `main`, tß║ío **Pull Request** v├á chß╗¥ Ng╞░ß╗¥i 4 duyß╗çt.
3. **Kh├┤ng push code ch╞░a compile ─æ╞░ß╗úc.**
4. Commit ghi r├╡ nß╗Öi dung thay ─æß╗òi (v├¡ dß╗Ñ `Add TrafficLight state classes`).
5. Kh├┤ng tß╗▒ ├╜ sß╗¡a hoß║╖c x├│a code cß╗ºa ng╞░ß╗¥i kh├íc khi ch╞░a trao ─æß╗òi.
6. Kh├┤ng tß╗▒ ├╜ ─æß╗òi t├¬n class, file, interface ─æ├ú thß╗æng nhß║Ñt. Muß╗æn ─æß╗òi phß║úi b├ío nh├│m tr╞░ß╗ƒng.
7. Tr╞░ß╗¢c khi merge phß║úi kiß╗âm tra code v├á build lß║íi project.

### Lß║Ñy code vß╗ü v├á l├ám viß╗çc

```bash
git clone https://github.com/KhanhVys/SmartTrafficSimulation-1-.git
cd SmartTrafficSimulation-1-
git checkout <branch cß╗ºa bß║ín>

# sau khi sß╗¡a code
git add .
git commit -m "Nß╗Öi dung thay ─æß╗òi"
git push
```

