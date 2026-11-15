# MouseMend

Fix a worn-out mouse on Windows:

- Stops the scroll wheel jumping the wrong way.
- Stops one click turning into a double click.

Any mouse. Free and open source.

**[Download MouseMend.exe](https://github.com/igorfyago/MouseMend/releases/latest/download/MouseMend.exe)** · [Website](https://igorfyago.github.io/MouseMend/)

## How to use

1. Download MouseMend.exe. If your browser says it isn't commonly downloaded, choose **Keep**.
2. Put it in a folder you keep, not Downloads, and run it. Windows starts it from there. If Windows says "Windows protected your PC", click **More info**, then **Run anyway**.
3. That's it. It runs in the system tray and starts with Windows. The first time, the Tuner opens too.

Both warnings are because MouseMend is new and not code-signed. The code is all here.

Used fly-by-wire, this program's name up to 1.0.2? Run MouseMend: it closes fly-by-wire and takes over its settings and its Start with Windows entry. Then delete fly-by-wire.exe.

## The Tuner

Optional. Click the MouseMend icon by the clock to open it (it may be behind the ^ arrow), or run MouseMend.exe again. It opens in your browser but runs on your own PC, usually at http://127.0.0.1:8479.

- **Two mice?** Pick the worn one at the top, so the other one is left alone.
- **Tests** count the mistakes MouseMend fixed, and any that still got through.
- **Drags letting go halfway?** On the Rules tab, under Release delay, set Left to 20. Left releases then wait 20 ms, so a switch that opens for a moment mid-drag no longer ends the drag.
- **Want every scroll notch kept?** On the Rules tab, turn on Send corrected ticks: a wrong-way tick is then turned round instead of dropped, and the first notch of a turn is kept too. Leave it off if you play games that read the mouse directly, as they would get some ticks twice.

## Good to know

- When you reverse the wheel, the first notch is ignored: on its own it looks just like a misread.
- Works in games like Age of Empires II. Some games, often shooters, read the mouse directly and still see the faults.
- Programs running as administrator get the fixes only if MouseMend runs as administrator too.
- **Guard** in the tray menu switches the fixes off and on. Ctrl+Alt+Shift+F11 switches them off from anywhere.
- Nothing leaves your PC: no account, no internet, no tracking. MouseMend records the timing of every click and wheel tick, never keys or cursor positions, in %LOCALAPPDATA%\MouseMend, and deletes those recordings after 14 days.

## Remove it

Right click the MouseMend icon by the clock, untick **Start with Windows**, click **Quit**. Then delete MouseMend.exe and the folder %LOCALAPPDATA%\MouseMend.

## How it works

MouseMend sits between the mouse and your apps.

- **Wheel:** it remembers which way you scroll. A single tick the other way is a misread and is dropped. Two in a row the other way means you turned the wheel, and it scrolls that way from the second one.
- **Clicks:** a press less than 20 ms after a release of the same button is the switch bouncing, not a finger, and is dropped. Double clicks by hand are slower than that, so they still work.

## Build from source

Needs 64-bit Windows 10 or 11 and the free [Build Tools for Visual Studio](https://visualstudio.microsoft.com/downloads/) (2019 or newer) with "Desktop development with C++".

```powershell
powershell -ExecutionPolicy Bypass -File .\build.ps1
```

This runs the tests, then builds `build\MouseMend.exe`. The rules are in `src/filter.h`, their tests in `tests/`. `python tools/make_icon.py` redraws the icons.

Not affiliated with any mouse maker. MIT license, see [LICENSE](LICENSE).
