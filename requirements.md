 # Moonlander Game Requirements

## 1. Goal

Create a playable Lunar Lander for the 48K ZX Spectrum, implemented primarily in C with Z88DK. If supplied `Requirments.jpeg` is the visual reference for the screen composition: Moonlander title, telemetry panels, starfield, central lander, moon, lunar surface, landing pad, and flag.

## 2. Target and constraints

- Target: 48K ZX Spectrum or compatible emulator.
- Display: 256x192 pixels with normal Spectrum attribute limitations.
- Toolchain: Z88DK `zcc +zx`, using the existing `build.ps1` and Fuse workflow.
- Preferred language: C.
- Assembly may be added only for a measured rendering or timing bottleneck.
- Reuse `games.h`, `graphics.h`, and `conio.h` where suitable.
- Reuse the embedded 16x16 sprite-table format in `LLander.h` where practical.
- Do not use a blocking `getk()` loop for flight controls.

## 3. Controls

Use the controls shown in the reference screenshot:

- Any key starts a new flight from the title or ready screen.
- `Z` rotates the lander anticlockwise.
- `X` rotates the lander clockwise.
- Held `SPACE` fires rocket thrust.
- `P` pauses and resumes the current flight.
- `Q` quits the current flight and returns to the title or ready screen.

Input must be polled without blocking the physics loop. Rotation and thrust must work while a key is held, releasing a key must stop its effect, and multiple controls may be active in the same frame. The on-screen control legend must use `Z`, `X`, `SPACE`, `P`, and `Q` exactly as defined here.

## 4. Screen layout

The screen should follow the reference image while remaining readable on a 48K Spectrum:

- Upper area: Moonlander title/logo and 48K label.
- Left telemetry: `TIME`, `ALT`, `H.SPD`, `V.SPD`, and `FUEL`.
- Right telemetry: `THRUST`, `ANGLE`, and `SCORE`.
- Main playfield: starfield, moon accent, lander, and lunar terrain.
- Terrain: a high-contrast surface with at least one marked landing pad and flag.
- Lower area: compact control labels and status messages.

HUD text must not overlap the playfield or be redrawn unnecessarily during flight.

## 5. Shared technical foundation

Before implementing the phases, establish these common interfaces and rules:

- Store lander position, velocity, angle, fuel, score, and flight state in a compact game-state structure.
- Use fixed-point or integer subpixel coordinates so motion is smoother than one pixel per frame.
- Define a stable frame rate or frame pacing method suitable for the 48K Spectrum.
- Define constants for gravity, rotation speed, thrust power, maximum speeds, fuel capacity, collision margins, and landing tolerances.
- Use a deterministic random seed per round so terrain and bugs can be reproduced.
- Separate simulation from drawing: update state first, then render only changed regions.
- Define states for title, ready, flying, landed, crashed, and round complete.

## Phase 1: Draw the lander

### Objective

Render the lander and static scene. There is no gravity, thrust physics, terrain collision, or landing logic in this phase.

### Requirements

- Draw the static starfield once when entering the play screen.
- Initialize the complete Spectrum attribute area to colour `0` before drawing the playfield, so the sky starts black.
- Draw a fixed starfield after the black-sky initialization and before the lander. Include both normal-brightness white stars and bright white stars using separate attribute values.
- Keep the starfield static during the Phase 1 animation; do not redraw the full sky for each lander frame.
- Use the supplied `99.png` sprite sheet as the authoritative lander artwork.
- Treat the sheet as seven 8-pixel-aligned crops: `00` = cells `(0,0)` to `(2,5)`, `15` = `(3,0)` to `(6,5)`, `30` = `(7,0)` to `(10,5)`, `45` = `(11,0)` to `(15,4)`, `60` = `(16,0)` to `(20,3)`, `75` = `(21,0)` to `(26,3)`, and `90` = `(27,0)` to `(32,2)`.
- Preserve the artwork's rotation pivot at cell `(1,1)` in every crop.
- Store the bitmap rows directly in the program or an included assembly data table; ROM character printing, UDG drawing, and ASCII art are not acceptable rendering methods.
- Draw the selected lander centred on the playfield using the pivot, with Spectrum attributes applied where possible.
- For an angle from 0 to 90 degrees, round to the nearest 15 degrees and select the matching source crop.
- For speed, pre-make all 28 animation frames before the game runs: create four pre-mirrored variants of each of the seven supplied source angles. Store those 28 static bitmap frames in the program or its generated assembly include.
- Runtime drawing must only select and draw a ready-made frame. It must not mirror pixels, reverse bitmap bits, reverse rows, rotate data, or construct a temporary sprite during gameplay.
- Use the same `(1,1)` pivot convention for every pre-mirrored frame, adjusting the bitmap origin so the pivot remains fixed at the screen centre.
- Run a Phase 1 demonstration that starts at 0 degrees, increments by 1 degree, wraps at 360 degrees, and completes one revolution in approximately six seconds.
- Keep the lander inside the playfield and clip drawing at screen boundaries.
- Keep HUD and static background pixels intact while the lander is redrawn.
- Do not call `clg()` or clear the entire previous lander rectangle for each angle. Store the previous frame pointer and erase exactly its bitmap with `spr_xor`, then redraw only the new frame.
- If the erased bitmap covered a star, restore that star locally before drawing the new lander. The static sky and starfield must not be rebuilt every frame.
- Synchronize the erase/draw operation with a `halt` before rendering to reduce display tearing and flicker.

### Acceptance tests

- Building with `build.ps1` produces a TAP without compiler errors.
- Fuse shows the title/ready screen and lander at the expected location.
- The displayed 0-degree frame matches the `00` crop from `99.png` and is centred by its `(1,1)` pivot.
- The six-second demonstration selects the expected 15-degree frame at rounded angles and visibly uses the 28 precomputed frames through all four quadrants.
- The lander can be drawn repeatedly without leaving trails or corrupting the HUD.
- A still frame shows no visible flicker when the render loop runs.
- Rotation and thrust changes must not produce a full rectangular flash; only changed sprite pixels may be erased and redrawn.
- The lander graphic fits within the intended collision bounds.

## Phase 2: Keyboard input and thrust

### Objective

Add the screenshot controls and rocket thrust while keeping the lander under controlled test conditions. Gravity and landing remain disabled until Phase 3.

### Requirements

- Poll the keyboard once per frame using a non-blocking method supported by the target/toolchain.
- `Z` changes the angle anticlockwise.
- `X` changes the angle clockwise.
- Define `ROTATION_STEP` as `5` degrees. Each held rotation key changes the angle by this constant, rather than by a hard-coded one-degree increment.
- Angles wrap cleanly at the full rotation boundary.
- Process the supplied `98.png` using these seven 8-pixel-aligned crops: `00` = cells `(0,0)` to `(2,2)`, `15` = `(3,0)` to `(6,3)`, `30` = `(7,0)` to `(10,3)`, `45` = `(11,0)` to `(14,3)`, `60` = `(16,0)` to `(19,3)`, `75` = `(21,0)` to `(24,3)`, and `90` = `(27,0)` to `(29,2)`.
- Preserve the `(1,1)` rotation pivot in each `98.png` crop. Generate all 28 flame-off frames by creating four pre-mirrored quadrant variants of each crop and writing them to `lander_frames_no_flame.inc`: `q0` original, `q1` top-to-bottom mirror, `q2` 180-degree mirror, and `q3` left-to-right mirror.
- Keep the `98.png` frame dimensions equal to their crop dimensions; do not reuse the larger `99.png` flame-on frame bounds for flame-off drawing, collision bounds, or erasure.
- Keep the `generate_lander_no_flame.py` generator in the source folder. Running it with Pillow installed regenerates `lander_frames_no_flame.inc` from `98.png`.
- Keep both 28-frame sets in static program/assembly data: the `99.png` set includes the flame and the `98.png` set does not.
- Held `SPACE` selects the matching `99.png` frame and displays the thrust flame while applying thrust input to the lander state.
- When `SPACE` is released, select the matching `98.png` frame immediately so no flame is drawn.
- Runtime thrust graphics must only switch between the two precomputed frame pointers. Do not add or erase flame pixels manually and do not mirror or transform sprite data during gameplay.
- `P` toggles pause without losing the current flight state.
- Thrust direction follows the lander's current angle.
- Fuel decreases only while thrust is active and cannot become negative.
- Empty fuel prevents further thrust and updates the HUD/status when Phase 4 is present.
- The lander may be held at a fixed position or moved inside a bounded test area, but must not collide with terrain yet.

### Acceptance tests

- `Z` and `X` rotate in the correct directions.
- Holding a key produces continuous input without repeated blocking pauses.
- Holding `SPACE` shows the flame-on frame; releasing `SPACE` selects the corresponding flame-off frame and stops fuel consumption.
- `P` pauses and resumes the simulation without changing the lander's state while paused.
- Holding `Z` and `SPACE` together rotates and thrusts in the same frame.
- `Q` exits the current flight and returns to the title or ready state.
- Fuel decreases at a predictable rate and stops at zero.
- Rendering remains stable without full-screen clears or flicker.

## Phase 3: Terrain, gravity, and landing

### Objective

Add a playable flight model and procedural lunar terrain. Every generated level must contain at least one physically possible landing route.

### Terrain generation

- Generate terrain once at the start of each round; it remains static during flight.
- Use a deterministic seed during development and allow a different seed for later rounds.
- Generate a bounded piecewise-linear mountainous height profile with distinct peaks and valleys rather than gentle two-pixel random variation or unconstrained noise.
- Keep every terrain point between `TERRAIN_MIN_Y = 152` and `TERRAIN_MAX_Y = 184`.
- Clamp the final terrain segment to the visible bitmap range `x = 0..255`; never draw to `x = 256`.
- Rasterize every terrain segment with an explicit bounded pixel-line routine that sets each pixel, so local background restoration is idempotent and cannot leave gaps in the mountain outline.
- Set the white Spectrum attribute for every character cell touched by a terrain pixel, not only at segment endpoints; otherwise bitmap pixels in intermediate cells remain invisible against black ink.
- Fill the area under the mountain outline to `TERRAIN_FILL_BOTTOM = 189` with a deterministic 25% stipple pattern (`TERRAIN_FILL_MASK = 3`), preserving visible black space between fill pixels. Terrain can extend behind the control legend at row `184`; restore the legend after any dirty-region terrain redraw reaches that row.
- Generate terrain fill with bounded direct pixel loops, clipped to the dirty region when restoring behind the lander; do not use flood fill.
- Limit the maximum slope between adjacent terrain segments so there are no vertical walls or impossible spikes.
- Reserve at least one broad, nearly level landing pad.
- Make the pad wide enough for both landing legs, with a clearly defined centre and collision width.
- Keep a safe vertical and horizontal spawn corridor above the pad or open terrain.
- Reject and regenerate a profile if the pad is too narrow, obstructed, outside the playfield, or unreachable under the configured physics.
- Keep terrain clear of the HUD and place the flag on the landing pad.
- Validate every generated profile with bounded heights, bounded slopes, pad width, pad levelness, spawn clearance, and open approach corridor checks.

### Flight model

- Use fixed-point position and velocity with `FIXED_SCALE = 16` so the lander can move below one pixel per frame.
- Define the physics constants in one place: `GRAVITY_ACCEL = 1`, `THRUST_ACCEL = 4`, `THRUST_FUEL_RATE = 2`, and `STARTING_FUEL = 1000`.
- Define landing limits in one place: `MAX_LANDING_SPEED = 12` and `MAX_LANDING_ANGLE = 10` degrees.
- Apply constant downward gravity each simulation frame.
- Apply thrust along the lander's current angle while `SPACE` is held and fuel remains.
- Use the project sprite convention for thrust direction: at angle `0`, thrust moves the lander upward; at angle `10`, the horizontal component moves it right on screen. The horizontal lookup signs must match this convention.
- Integrate horizontal and vertical velocity using the chosen fixed-point representation.
- Track altitude, horizontal speed, vertical speed, angle, thrust, fuel, and elapsed time.
- Clamp values where necessary to prevent numeric overflow or uncontrolled escape from the playfield.
- Detect terrain contact using the lander's collision shape, not only its centre pixel.
- Treat only the lander's top three 8-pixel character rows as solid for landing collision. The bottom three character rows are thrust flame and must never trigger terrain contact.
- Calculate the solid-leg bottom from the selected 28-frame quadrant and pivot, including vertical mirroring; do not subtract a fixed flame height from the overall sprite bottom for every quadrant.
- A landing succeeds only when both landing legs are over the pad, angle is within the safe tolerance, speeds are below their limits, and the lander is not intersecting terrain.
- Any other terrain contact, excessive speed, or unsafe angle causes a crash.
- Show a stable landed or crashed state before accepting the next round.
- `Q` returns to the title or ready state from an active flight.
- Use a deterministic bounded terrain profile at round start. It must force columns `16` through `23` to a level landing pad at height `176`, keep columns `15` and `24` as controlled transition shoulders at height `180`, and shape the remaining surface with visible mountains and valleys within the configured playfield bounds.
- When terrain contact is classified, show `SUCCESS` for a safe landing or `CRASH` for any unsafe contact.
- On a successful landing, draw a two-character-high flag immediately to the right of the landed lander and animate it between pre-embedded bitmap frames while `SUCCESS` is displayed. Raise the flag by three 8-pixel character rows so its base aligns with the lander's solid feet/pad level, not the flame rows.
- Keep the success flag visible beside the result message until the restart `SPACE` press; a crash result does not draw the flag.
- On a crash, erase the full lander and draw only a flame-free 8-pixel strip representing the lander's second character row, positioned one additional character row lower. Leave the three flame rows and all other rows hidden while `CRASH` is displayed.
- After crash-sprite cleanup, restore any HUD and control text overlapped by the previous sprite rectangle before displaying `CRASH`; result cleanup must not erase or restyle screen text.
- After either result, wait for `SPACE` to be released and pressed, then reset the lander, velocity, fuel, angle, and flight state for a new round.
- At the start of every new flight, clear the complete bitmap and attribute screen before redrawing the black sky, static stars, terrain, and lander. The previous `SUCCESS` or `CRASH` message must not remain visible.
- In normal startup, perform the full clear and static-scene render only once through `StartFlight()`; do not draw the same scene in `main()` before calling it. The optional `TestDrawLander` diagnostic may initialize its own test scene.
- Render result messages through the working `PrintAt`/`PrintCenteredAt` text helpers. Do not use `gotoxy()` or emit raw cursor-control text for result messages.

### No-flicker rendering

- Never clear the full screen during flight.
- Restore only the lander's previous background region before drawing its new position.
- Before erasing or redrawing, compare the selected source graphic pointer and calculated sprite location with the previous frame. If both are unchanged, skip the entire lander redraw.
- Redraw only terrain/background regions affected by the lander or flame.
- Keep the static starfield, HUD, terrain, pad, and flag unchanged unless their state changes.
- A saved-background, dirty-rectangle, masked-sprite, or equivalent local redraw approach is acceptable.
- Profile the C implementation before replacing a small routine with assembly.

### Acceptance tests

- Repeated generation with the same seed produces identical terrain.
- Every generated terrain profile passes the pad width, slope, clearance, and approach checks.
- A known test seed permits a controlled successful landing.
- A known test seed and unsafe velocity produce a crash.
- Gravity moves the lander downward when thrust is not active.
- Thrust changes velocity in the direction indicated by the lander's angle.
- At angle `10`, thrust produces movement to the right on screen; angle `0` produces upward movement.
- Fuel decreases by `THRUST_FUEL_RATE` while thrust is active and the flame turns off when fuel reaches zero.
- The generated terrain always contains the eight-column level pad and remains unchanged during flight.
- A landing displays `SUCCESS`, a crash displays `CRASH`, and `SPACE` starts the next flight after either result.
- The lander cannot pass through terrain or leave the playable area.
- Flight remains readable and free of obvious trails or flicker in Fuse.

## Phase 4: Text, logos, and control parameters

### Objective

Complete the presentation shown in the reference image without compromising the flight loop or readability.

### Requirements

- Add the Moonlander title/logo and 48K label.
- Draw the top title row using the wide text layout: `MOONLANDER` at the left and `ZX SPECTRUM 48K` at the right, within the approximately 60-character text width supported by the current library.
- Render `MOONLANDER` as a native bitmap wordmark sampled from the supplied `Requirments.jpeg`, 80 pixels wide and two character cells high; keep `ZX SPECTRUM 48K` in the regular text font at the right.
- Render the title row as inverse text: bright white ink on black paper. Do not change the telemetry or control-row attributes when applying the title style.
- Add a small two-row rainbow logo to the left of `MOONLANDER`, formed from diagonal triangular 8x8 attribute cells. Adjacent cells use Spectrum ink/paper pairs such as red/black, yellow/red, green/yellow, and cyan/green to create diagonal color transitions without per-pixel attribute clash; preserve the reference's foreground/background order.
- Add telemetry fields: `TIME`, `ALT`, `H.SPD`, `V.SPD`, `FUEL`, `THRUST`, `ANGLE`, and `SCORE`.
- Place `TIME`, `ALT`, `H.SPD`, `V.SPD`, and `FUEL` on the left; place `THRUST`, `ANGLE`, and `SCORE` on the right, matching the reference image.
- Add the bottom control legend: `Z-LEFT X-RIGHT` and `SPACE-THRUST P-PAUSE Q-QUIT`.
- Update only values that changed since the previous frame.
- Print every numeric field into a fixed-width buffer with trailing spaces, so shorter new values overwrite all characters from the previous value without clearing the screen area.
- Derive `ALT` from the current terrain height minus the lander's solid-leg bottom, `H.SPD`/`V.SPD` from fixed-point velocity, `THRUST` from the active fuel-burning state, and `TIME` from elapsed flight frames.
- Because velocity uses `FIXED_SCALE = 16`, display `H.SPD` and `V.SPD` as signed tenths of a pixel per frame using integer conversion (`SPEED_DISPLAY_SCALE = 10`) rather than truncating by direct division and showing zero for subpixel motion.
- Display the actual controls: `Z - LEFT`, `X - RIGHT`, `SPACE - THRUST`, `P - PAUSE`, and `Q - QUIT`.
- Display landing, crash, out-of-fuel, and ready messages without clearing the playfield unnecessarily.
- Draw the landing pad and flag with clear contrast against the lunar surface.
- Use Spectrum-safe text positioning and avoid relying on a 64-column layout.
- Keep display strings short enough for the available character columns.
- Add a documented score calculation based on landing quality, remaining fuel, and time.

### Acceptance tests

- All telemetry fields fit within the HUD at maximum values.
- Values update without tearing, overwriting adjacent text, or disturbing the lander.
- Numeric fields overwrite old digits completely when their width decreases.
- `H.SPD` and `V.SPD` show non-zero subpixel movement, including an explicit `+` or `-` sign.
- Control labels match the screenshot and actual controls.
- Title, ready, landed, crash, quit, and restart messages are legible in Fuse.
- A complete flight can be played from start to landing or crash without screen corruption.

## 6. Suggested C interfaces

Names may change to match the final implementation, but responsibilities should remain separate:

- `InitGame()` — initialise state and seed a round.
- `GenerateTerrain(seed)` — create and validate the terrain profile and landing pad.
- `PollInput()` — update held-key state without blocking.
- `UpdatePhysics()` — apply input, gravity, thrust, and movement.
- `CheckTerrainCollision()` — classify safe landing, crash, or no contact.
- `DrawStaticScene()` — draw starfield, moon, terrain, pad, flag, and fixed HUD.
- `DrawLander()` — restore the previous region and draw the current lander/flame.
- `UpdateTelemetry()` — redraw changed numeric values only.
- `StartRound()` — reset the lander and generate a new valid terrain profile.
- `ShowFlightResult()` — display the landed or crashed state.

## 7. Build and test workflow

From the project directory:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\build.ps1 -Source LLander.c -Compiler zcc -Output LLander
```

Run the generated TAP in Fuse using `launch_fuse.bat` or the `Build and Run ZX Spectrum` task.

Each phase must be built and tested before the next phase starts. Keep deterministic test seeds and test constants available during development, even if the release build selects a new seed for each round.

## 8. Out of scope for the first version

- Sound effects or music.
- Multiple ships or animated explosions.
- Two-player controls.
- Persistent high scores across power cycles.
- A full-screen software back buffer if local redraw meets the flicker requirement.

These may be added later only after the four phases are stable on the 48K target.
