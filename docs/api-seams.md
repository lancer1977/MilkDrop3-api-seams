# Local API seams

This fork starts with a deliberately small, local-only control seam for companion apps and resident controllers.

## Why this shape

MilkDrop3 already has strong internal functions for visual control, but the stock UI path mostly reaches them through keyboard/menu events. The first fork seam exposes those internal calls directly through Windows `WM_COPYDATA` messages sent to the MilkDrop3 window.

This is not a public network API. Keep HTTP, WebSocket, Twitch, Stream Deck, or SignalR adapters in a separate local controller process that validates commands and then forwards only typed, narrow requests to MilkDrop3.

## Message transport

Send `WM_COPYDATA` to the MilkDrop3 window with UTF-16 text.
The payload is newline-separated `key=value` pairs, exactly one pair per line.

Protocol envelope:

- `version` — optional for legacy callers, `1` for the v1 envelope.
- `request_id` — optional correlation id echoed back in v1 replies.

Required field:

- `command` — one of the commands below.

Acknowledgement rules:

- The primary success signal is still the `WM_COPYDATA` return value: `TRUE` for accepted/executed commands, `FALSE` for invalid or unsupported commands.
- When `version=1` and `request_id` is present, the plugin also sends a best-effort `WM_COPYDATA` reply back to the sender HWND with a v1 response envelope (`status=ok|error`, `request_id`, `command`, and optional `detail`).
- That reply path is the query/readback channel for local automation; no separate socket, pipe, or polling endpoint is needed inside MilkDrop3.
- Legacy payloads without `version` continue to work and do not require the reply path.
- The seam intentionally does not add a network listener or callback socket.

## Implemented commands

These commands already exist in the seam handler and are safe to document as live behavior.

### Launch a sprite/image slot

```text
command=launch_sprite
sprite=01
slot=-1
```

- `sprite` is the `[imgNN]` number from `milk2_img.ini`.
- `slot` is optional; `-1` lets MilkDrop3 choose the slot like the keyboard path.
- `sprite` also accepts the legacy alias command `sprite`.

This calls `CPlugin::LaunchSprite(sprite, slot)` directly, avoiding the `k` + two-digit keyboard automation path. `slot` is guarded as `-1` or `0..NUM_TEX-1` before calling the existing loader.

### Kill a sprite slot

```text
command=kill_sprite
slot=0
```

- `slot` must identify an active sprite slot; empty and out-of-range slots are rejected.
- `kill_sprite` also accepts the legacy alias `sprite_kill`.

This calls `CPlugin::KillSprite(slot)`.


### Check protocol availability

```text
command=ping
```

Aliases: `ping`, `api_version`.

This returns `TRUE` when the v1 handler is installed and reachable. It is intended for smoke tests and local controller startup checks.

### Query the current preset and collection state

```text
command=preset_status
request_id=42
version=1
```

The v1 reply uses the existing best-effort `WM_COPYDATA` reply path and returns a compact JSON object in `detail`. It includes the current preset, preset collection counts, the current list cursor, preset directory, loading state, and history fences.

### Browse the preset list

```text
command=preset_list
start=0
limit=16
```

Aliases: `list_presets`.

This returns a JSON `detail` payload with a page of preset list entries. Each entry includes its list index, raw name, kind (`preset` or `directory`), and whether it is the running or highlighted entry. `limit` is capped to 32.

### Snapshot current state

```text
command=state_snapshot
request_id=42
version=1
```

Aliases: `snapshot`, `status`.

This returns a compact JSON object with the current preset, preset collection cursor, loading state, history fences, basic playback/control flags, and a `sprites` array for live sprite slots. It is read-only and uses the v1 `WM_COPYDATA` reply path, so local automation can query the running instance without a second transport.


### Load a preset by path or collection name

```text
command=load_preset
path=C:\MilkDrop3\presets\Example.milk
blend=1.7
```

Aliases: `load_preset_by_name`, `load_preset_by_path`.

- `path` accepts a full filesystem path, a relative path under the preset directory, or a bare collection filename.
- `preset`, `name`, and `file` are accepted as aliases for `path`.
- `blend` is optional, defaults to the user blend-time setting, and must be between `0.0` and `60.0` seconds.

The handler resolves the request against the live preset collection when it can, then calls `CPlugin::LoadPreset(path, blend)`.

### Step through the preset history

```text
command=next_preset
blend=1.7
```

Aliases: `preset_next`, `prev_preset`, `preset_prev`.

These call the existing `NextPreset` / `PrevPreset` control paths and keep the current history behavior intact. `blend` is optional and uses the same `0.0` to `60.0` guard as the other preset commands.

### Load a random preset

```text
command=random_preset
blend=1.7
```

- `blend` is optional, defaults to the user blend-time setting, and must be between `0.0` and `60.0` seconds.

This calls `CPlugin::LoadRandomPreset(blend)`.

### Update a sprite image

```text
command=set_sprite_image
sprite=1
slot=-1
path=C:\MilkDrop3\sprites\sparkle.png
colorkey=0
```

- `sprite` selects the `[imgNN]` section to rewrite.
- `slot` is optional; when supplied it must be `-1` or `0..NUM_TEX-1`, then the sprite is reloaded into that slot.
- `path` accepts `path`, `img`, or `image` as the file field alias and must fit the existing 512-character sprite image buffer.
- `colorkey` is optional; when present it updates both `colorkey` and `colorkey_lo`.

This updates the image path in `milk2_img.ini` and calls `CPlugin::LaunchSprite(sprite, slot)` so the new image is live immediately.

### Update sprite motion or live variables

```text
command=set_sprite_motion
sprite=1
slot=2
x=0.45
y=0.33
sx=1.1
sy=0.9
rot=0.25
flipx=false
flipy=true
blendmode=0
burn=false
done=false
```

Aliases: `set_sprite_motion`, `set_sprite_params`, `sprite_motion`, `sprite_params`.

- `slot` can target an already-loaded live sprite directly.
- `sprite`, `sprite_id`, or `id` can be used instead of `slot`; the handler finds the first live slot whose `nUserData` matches that sprite id.
- A supplied `slot` must be `0..NUM_TEX-1` and must currently hold a live sprite surface.
- Supported live fields include `x`, `y`, `sx`, `sy`, `rot`, `repeatx`, `repeaty`, `r`, `g`, `b`, `a`, `blendmode`, `flipx`, `flipy`, `burn`, and `done`, plus common aliases like `xpos`, `scale_x`, `alpha`, and `color_r`.

This command is intentionally narrow: it updates the existing per-slot EEL variables and does not expose a broad scripting API or rewrite sprite motion code.

### Update overlay/display flags

```text
command=set_overlay_flags
show_preset_info=true
show_fps=true
show_rating=false
show_debug_info=false
show_shader_help=true
show_song_title=true
show_song_time=true
show_song_len=false
```

Aliases: `set_overlay_flags`, `set_display_flags`, `set_view_flags`.

Each field is optional; omitted fields are left unchanged. The handler rejects malformed boolean values for the whole request.

This command maps directly to the existing overlay toggles behind the keyboard/menu paths, including the preset info, FPS, rating, debug, shader-help, and song-title overlays.

### Update runtime/lock flags

```text
command=set_runtime_flags
always_on_top=true
preset_locked=false
sequential_preset_order=true
hardcuts_disabled=false
rating_enabled=true
song_title_anims=true
```

Aliases: `set_runtime_flags`, `set_behavior_flags`, `set_window_flags`.

Each field is optional; omitted fields are left unchanged. `always_on_top` updates the topmost window state immediately, and `sequential_preset_order` preserves the existing history reset behavior so it stays compatible with the keyboard toggle.

## Planned command families

These are the next seams to expose. They are roadmap items, not implemented commands yet.

- `command=set_transition` — select a known transition/blend mode.
- `command=set_runtime_param` — narrow allowlisted parameters such as zoom/warp/wave values.
- `command=preset_search` — optional text search or filtered browse across the collection when the local controller wants richer discovery than `preset_list` pages.


The initial seam is implemented in:

- `code/vis_milk2/plugin.h` — declares `CPlugin::HandleApiCopyData`.
- `code/vis_milk2/plugin.cpp` — parses `WM_COPYDATA` payloads and routes to `LaunchSprite`, `KillSprite`, live sprite variable updates, `preset_status`, `preset_list`, `state_snapshot`, `LoadPreset`, and `LoadRandomPreset`.

## Runtime smoke prerequisites

To exercise the seam at runtime, the test environment needs:

- A Windows 10/11 target with the MilkDrop3 runtime installed, or a Wine/Xvfb reproduction environment for local smoke on Linux.
- Visual Studio/MSBuild if you need to rebuild the fork before running the smoke.
- A sender harness that can emit UTF-16 `WM_COPYDATA` payloads to the MilkDrop3 window. This fork includes `tools/send-milkdrop-copydata.ps1` for Windows smoke tests.
- A visible MilkDrop3 top-level window that stays alive long enough for the sender to find it by title.

Known-good payloads from the source seam:

- `command=launch_sprite\nsprite=01\nslot=-1`
- `command=kill_sprite\nslot=0`
- `command=load_preset\npath=C:\\MilkDrop3\\presets\\Example.milk\nblend=1.7`
- `command=random_preset\nblend=1.7`
- `command=ping`
- `command=set_sprite_motion\nslot=0\nx=0.5\ny=0.5\na=0.75`
- `command=set_sprite_image\nsprite=01\nslot=-1\npath=C:\\MilkDrop3\\sprites\\sparkle.png`

## Current local smoke status

A Linux/Wine reproduction was attempted from this repo using Wine 9.0, Xvfb, and the repo's `linux/MilkDrop 3 linux.exe` binary. After installing the documented Wine prerequisites (`d3dx9` and `vcrun2019`), the executable still exited immediately and never left a stable window available for `WM_COPYDATA` smoke, so the definitive Windows runtime verification remains pending.

## Windows runtime smoke plan

Use this exact runbook when a Windows target is available:

1. Rebuild the fork on Windows with Visual Studio/MSBuild if the runtime binary is stale.
2. Launch MilkDrop3 and keep the top-level window visible long enough for title lookup.
3. Run the sender harness from this repo:
   - `./tools/send-milkdrop-copydata.ps1 -WindowTitle MilkDrop -Payload "command=ping"`
   - `./tools/send-milkdrop-copydata.ps1 -WindowTitle MilkDrop -Payload "command=launch_sprite`nsprite=01`nslot=-1"`
   - `./tools/send-milkdrop-copydata.ps1 -WindowTitle MilkDrop -Payload "command=kill_sprite`nslot=0"`
   - `./tools/send-milkdrop-copydata.ps1 -WindowTitle MilkDrop -Payload "command=load_preset`npath=C:\\MilkDrop3\\presets\\Example.milk`nblend=1.7"`
   - `./tools/send-milkdrop-copydata.ps1 -WindowTitle MilkDrop -Payload "command=random_preset`nblend=1.7"`
4. Record, for each command, the exact payload, sender output (`accepted` or `rejected`), and the observed runtime behavior in the MilkDrop3 window.
5. If a command fails, capture the repro details needed for a follow-up card: Windows version, build configuration, exact payload, window title fragment, and any visible/logged behavior.

The local sender harness is intentionally narrow and local-only; it is the smoke tool, not a substitute for Windows runtime verification.

## Next seams to expose

- Linux xbuild is useful for checking repo structure and documentation snippets, but it does not validate the Windows C++ runtime path that actually executes `HandleApiCopyData`.
- Treat the Windows C++ compile/run path as the source of truth for seam behavior before promoting a planned command into the implemented section.
- Avoid adding a generic keystroke proxy or public unauthenticated network listener inside MilkDrop3.

## Companion control surface options

Recommended boundary: keep this fork focused on the local-only MilkDrop3 seam and the command contract, and put the user-facing companion surfaces in a separate local controller repo.

| Option | Keep in this fork? | Why / why not |
| --- | --- | --- |
| CLI sender | Yes, but only as a tiny local smoke-test utility; otherwise no | Fastest path to validate `WM_COPYDATA`, minimal UX scope, and easy to keep local-only. It should stay a thin client, not a general framework. |
| Local desktop UI | No | It is a full product surface with state, navigation, and status rendering. That is too much churn for the plugin fork. |
| OBS / TouchDesigner bridge | No | These are integration-heavy adapters that should live beside their host workflows, not inside the MilkDrop3 fork. |
| MIDI / OSC adapter | No | Good for performance control, but protocol handling and device mapping are separate concerns from the seam itself. |
| File-watch adapter | No, unless it is just a throwaway dev harness | Useful for script-driven control, but once it grows beyond a smoke-test helper it belongs in the companion controller repo. |

## Local sender harness

A narrow Windows smoke sender lives at `tools/send-milkdrop-copydata.ps1`. Example:

```powershell
./tools/send-milkdrop-copydata.ps1 -WindowTitle MilkDrop -Payload "command=ping"
./tools/send-milkdrop-copydata.ps1 -WindowTitle MilkDrop -Payload "command=launch_sprite`nsprite=01`nslot=-1"
```

It finds a top-level window by title fragment, sends UTF-16 `WM_COPYDATA`, and exits `0` with `accepted` when MilkDrop3 returns `TRUE`; rejected commands exit `2`.

## Suggested next implementation slice

The next repo-local slice should be a Windows build/runtime smoke on the actual MilkDrop3 window, followed by a richer status/query protocol with an explicit reply channel if the local-only `WM_COPYDATA` path proves stable. Keep companion UI, OBS, MIDI, OSC, and network-facing adapters outside this fork.
