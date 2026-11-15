# MouseMend

**Fixes a worn-out mouse: a scroll wheel that jumps the wrong way, and clicks that fire twice.** Any brand. Free and open source, for Windows. No driver, nothing to install.

**[Download MouseMend.exe](https://github.com/igorfyago/MouseMend/releases/latest/download/MouseMend.exe)** · [Website](https://igorfyago.github.io/MouseMend/) · [All releases](https://github.com/igorfyago/MouseMend/releases)

## What it fixes

- **The wheel scrolls back.** You scroll down and the page twitches up. A worn wheel sensor misreads a step now and then, more often the slower you turn it.
- **A click fires twice.** A worn switch bounces: one press arrives as a double click, or a drag breaks off.

MouseMend sits between the mouse and every program and catches both before anything sees them: browsers, Windows itself, and games that read ordinary mouse messages. Everything else passes untouched.

## Start in two minutes

1. [Download MouseMend.exe](https://github.com/igorfyago/MouseMend/releases/latest/download/MouseMend.exe) and run it. It is not code-signed, so Windows SmartScreen may ask once: **More info**, then **Run anyway**.
2. The Tuner opens in your browser and a tray icon appears. MouseMend now starts with Windows.
3. If you have more than one mouse, pick the worn one in the list at the top of the Tuner.

That is all it needs. The next part is optional.

## The Tuner in one minute

The Tuner is a page on your own PC (<http://127.0.0.1:8479>). Open it any time from the tray icon.

- **Tests:** run *Scroll up*, *Scroll down* and *Clicks*. Each one counts what your mouse sent wrong and what MouseMend fixed. Untick **Guard** at the top to see your mouse without it, so you get before and after.
- **Rules:** switch on **Send corrected ticks** to keep every notch of the wheel (see below for when not to). Every other value is already measured on a worn mouse.
- **Events:** every click and tick as it happens. Red rows never reached your programs the way the mouse sent them.
- **Tray icon:** right click for Guard, Start with Windows, Tuner and Quit. **Ctrl+Alt+Shift+F11** switches the guard off from anywhere.

## How it decides

**The wheel.** On the worn mouse this was built on, 3 to 7% of the ticks came out the wrong way when the wheel turned fast, and 8 to 12% when it turned slowly. Every one was a single tick, never two in a row. So MouseMend remembers which way you scroll, and a tick the other way waits for the next one:

- the next tick goes the same new way: you turned the wheel, both go through;
- the next tick goes the old way: the held tick was a misread;
- nothing follows within a second: the held tick is dropped.

The price: a single notch back and forth on its own is lost, and a turn needs two notches.

What happens to a misread depends on **Send corrected ticks**:

| | Off (the default) | On |
|---|---|---|
| A misread tick | dropped | turned the right way round, so you keep every notch |
| A turn of the wheel | its first notch is lost | its first notch arrives with the second |
| Games that read raw input | see the mouse as it is | also get the corrected ticks, so some ticks twice |

A wrong tick takes the place of a right one, so turning it round is the exact repair. It is off by default only because Windows hands every tick to programs that read raw input before any program like this can hold it back. Browsers, Windows and most games read ordinary mouse messages: if yours do, switch it on.

**Clicks.** A press less than 20 ms after the release of the same button is the switch bouncing, not a finger: it is dropped. A press shorter than 20 ms is a bounce on the way down: a re-press right after it continues the same press, so drags do not break. Double clicks by hand are never touched (the fastest measured from a hand left 30 ms between release and press), and normal clicks are never delayed.

## Measured

All of one day's scroll tests on a worn SteelSeries Rival 500 (15 runs, 4500 ticks, fast and slow), run back through the rules:

| | Wrong ticks reaching programs |
|---|---|
| No guard | 351 |
| MouseMend, default | 20, a notch lost for each misread |
| MouseMend, Send corrected ticks | 25, every notch kept |

All that is left comes from one run, stopped early, where the wheel went the other way 5 to 8 ticks at a time; no rule can tell that from a hand. The other 14 runs: 0 wrong ticks. The last two ran live: 34 misreads, none reached a program.

## Limits

- Games that read raw input see every tick and click as the mouse sent it. Nothing that runs above the mouse driver can change that.
- Windows that run as administrator are not covered unless MouseMend runs as administrator too.
- One notch back and forth on its own is lost (see above).
- The settings were fitted on one mouse. If yours is worse, the Rules tab has every value, and `replay.exe` (in each release) runs a recording through the rules with other values before you change anything.

## Privacy

Everything stays on your PC. MouseMend records the timing of button presses and wheel ticks (no cursor positions, no keys, no window titles) to `%LOCALAPPDATA%\MouseMend`, for the tests and the Tuner. Recordings older than 14 days are deleted. The Tuner is served to 127.0.0.1 only and refuses requests from web sites. No network use, no telemetry.

## Start with Windows, and removing it

From the first run on, MouseMend starts with Windows: a value under your user's Run key, nothing installed. Untick **Start with Windows** in the tray menu to stop that. To remove MouseMend: untick it, quit from the tray, delete the exe and the `%LOCALAPPDATA%\MouseMend` folder.

## Build from source

Needs 64-bit Windows 10 or 11 and the free [Build Tools for Visual Studio](https://visualstudio.microsoft.com/downloads/) (2019 or newer) with "Desktop development with C++". No other dependencies.

```powershell
powershell -ExecutionPolicy Bypass -File .\build.ps1
```

That runs the rule tests first, then builds `build\MouseMend.exe`, `build\replay.exe` and the test tools.

| Check | What it proves |
|---|---|
| `tests\cases.txt` via `build.ps1 -Only tests` | the rules give the right output for 201 event sequences, 84 of them from an independent reference model |
| `tests\mutation_check.ps1` | the tests can fail: each of 132 deliberately broken copies of the rules must be caught |
| `build\selftest.exe` | the real hook on the real desktop does what the rules say (takes over the mouse for 30 seconds) |
| `tests\verify_recorder.ps1` | the recorder counts a scripted faulty mouse exactly (takes over the mouse for a minute) |
| `node tests\page_test.js web\index.html` | the Tuner colours each held tick by what became of it (optional, needs Node.js) |

| File | What is in it |
|---|---|
| `src/filter.h` | the rules, plain C++ with no Windows in it, so they can be tested with made-up events |
| `src/engine.h` | the low level mouse hook, raw input, the thread that owns the rules |
| `src/whichmouse.h` | matching each event to the mouse that sent it |
| `src/store.h` | counts, tests, the recording |
| `src/main.cpp` | tray, settings, the Tuner's local server |
| `src/replay.cpp` | runs a recording through the rules with other values |
| `web/index.html` | the Tuner |
| `docs/` | the website |

MouseMend was called fly-by-wire up to version 1.0.2; starting it takes over that copy's settings and its place in Start with Windows. Not affiliated with any mouse maker.

## License

MIT, see [LICENSE](LICENSE).
