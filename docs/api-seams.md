# Local API seams

This fork starts with a deliberately small, local-only control seam for companion apps and resident controllers.

## Why this shape

MilkDrop3 already has strong internal functions for visual control, but the stock UI path mostly reaches them through keyboard/menu events. The first fork seam exposes those internal calls directly through Windows `WM_COPYDATA` messages sent to the MilkDrop3 window.

This is not a public network API. Keep HTTP, WebSocket, Twitch, Stream Deck, or SignalR adapters in a separate local controller process that validates commands and then forwards only typed, narrow requests to MilkDrop3.

## Message transport

Send `WM_COPYDATA` to the MilkDrop3 window with UTF-16 text. The payload is newline-separated `key=value` pairs.

Required field:

- `command` — one of the commands below.

The message return value is `TRUE` for accepted/executed commands and `FALSE` for invalid or unsupported commands.

## Commands

### Launch a sprite/image slot

```text
command=launch_sprite
sprite=01
slot=-1
```

- `sprite` is the `[imgNN]` number from `milk2_img.ini`.
- `slot` is optional; `-1` lets MilkDrop3 choose the slot like the keyboard path.

This calls `CPlugin::LaunchSprite(sprite, slot)` directly, avoiding the `k` + two-digit keyboard automation path.

### Kill a sprite slot

```text
command=kill_sprite
slot=0
```

This calls `CPlugin::KillSprite(slot)`.

### Load a preset

```text
command=load_preset
path=C:\\MilkDrop3\\presets\\Example.milk
blend=1.7
```

- `path` is a `.milk` or `.milk2` preset path.
- `blend` is optional and defaults to the user blend-time setting.

This calls `CPlugin::LoadPreset(path, blend)`.

### Load a random preset

```text
command=random_preset
blend=1.7
```

This calls `CPlugin::LoadRandomPreset(blend)`.

## Current source-level seam

The initial seam is implemented in:

- `code/vis_milk2/plugin.h` — declares `CPlugin::HandleApiCopyData`.
- `code/vis_milk2/plugin.cpp` — parses `WM_COPYDATA` payloads and routes to `LaunchSprite`, `KillSprite`, `LoadPreset`, and `LoadRandomPreset`.

## Next seams to expose

Good next typed commands:

- `command=set_sprite_image` — rewrite or reload a specific `[imgNN]` image path and optional movement code.
- `command=set_sprite_motion` — update sprite init/per-frame code or live sprite variables when safe.
- `command=set_transition` — select a known transition/blend mode.
- `command=set_runtime_param` — narrow allowlisted parameters such as zoom/warp/wave values.
- `command=status` — return a current preset/sprite/status snapshot through a reply window or a small local controller protocol.

Avoid adding a generic keystroke proxy or public unauthenticated network listener inside MilkDrop3.
