# Potluck

**Potluck** is an original 3D cultivation-and-business progression game built in C++17 with raylib. This vertical slice begins with a short late-1960s origin, moves into a 1970s outdoor farm chapter, and unlocks a separate hydroponics workshop where the player assembles and validates their first indoor system.

The long-term direction is a grow-to-empire journey through cultivation technology, genetics, legalization, products, branding, public markets, and eventually billionaire-scale strategy. This milestone focuses on proving the first outdoor-to-indoor transition.

Potluck uses procedural low-poly geometry and raylib's built-in font. It requires no external art, model, audio, shader, or data assets.

## Quick start

Build and launch the debug game with one command:

```sh
make
```

Other useful shortcuts:

```sh
make build        # Build without launching
make test         # Build and run tests
make release-run  # Build and launch the release version
make help         # List all shortcuts
```

## Build

Requirements:

- CMake 3.20+
- Ninja
- A C++17 compiler
- raylib discoverable through `pkg-config`

On macOS with Homebrew raylib:

```sh
cmake --preset dev
cmake --build --preset dev
ctest --preset dev
./build/dev/potluck
```

Release build:

```sh
cmake --preset release
cmake --build --preset release
ctest --preset release
```

## Controls

| Input | Outdoor farm | Hydroponics workshop |
|---|---|---|
| `W A S D` | Move | Move |
| `Shift` | Sprint | Sprint |
| Mouse or arrow keys | Orbit camera | Orbit camera |
| Mouse wheel | Zoom | Zoom |
| `Tab` | Capture/release mouse | Capture/release mouse |
| `1`–`4` | Select tool | Select reservoir, pump, channel, or light |
| `Z / X` | Cycle cultivar | — |
| `F` or left click | Use selected tool | Place selected equipment |
| `E` | Interact | Validate at console / use exit |
| `R` | Refill at water tank | Remove targeted equipment |
| `I` | Inventory | Inventory |
| `J` | Quest journal | Quest journal |
| `F5 / F9` | Quick save/load | Quick save/load |
| `Esc` | Pause/back | Pause/back |

## Vertical-slice flow

1. Advance through or skip the late-1960s origin.
2. Till, plant, and water three outdoor plots.
3. Harvest two flowers, place them in the overnight shipping bin, and sleep to settle the sale.
4. Speak with Tessa to unlock the hydroponics workshop.
5. Enter the new indoor 3D environment.
6. Install exactly one reservoir, one pump, at least two reachable grow channels, and matching lights.
7. Validate the build at the console while staying within the `$650` grant and `500 W` power limit.
8. Complete the prototype and continue inspecting either environment.

The left pump supports the left and center channels; the right pump supports the center and right channels. A valid two-channel build costs `$630` and consumes `480 W`.

## Saves

The game autosaves after each outdoor day and supports quick save/load. Potluck saves are stored at:

```text
~/Library/Application Support/Potluck/saves/slot1.save
```

A backup save is maintained beside the primary file. Potluck uses a clean, versioned save identity and intentionally does not migrate old Ultradope prototype saves. The codec validates progression, environment, quest, and hydroponics invariants before replacing the live session.

## Architecture

- `src/domain`: raylib-free game state, progression, and hydroponics rules
- `src/systems`: focused state transitions for farming, economy, quests, days, and hydro completion
- `src/world`: outdoor and hydro layout, targeting, interactions, and collision
- `src/render`: procedural 3D drawing for both environments
- `src/ui`: title, origin, environment-specific HUDs, dialogue, and milestone screens
- `src/save`: versioned codec and filesystem service
- `src/app`: application lifecycle, environment dispatch, and orchestration
- `tests`: dependency-free tests for pure systems and persistence

The internal C++ namespace remains `ultradope` during this milestone to avoid a mechanical rename unrelated to player-facing functionality. Product text, build artifacts, documentation, and save identity use Potluck.
