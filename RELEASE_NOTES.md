# Tank Arena 0.1.0

First playable Windows x64 release.

- 30 by 20 arena with solid walls and destructible crates.
- Five enemy tanks with pathfinding, line-of-sight aiming and shooting.
- Three armor points, enemy counter, victory and defeat screens.
- Restart with R; move with WASD or arrow keys; shoot with Space; quit with Esc.

Download GameEngine-0.1.0-windows-x64.zip, extract the entire archive, and run GameEngine.exe. Keep the res folder beside the executable.

Requires Windows 10/11 x64 and an OpenGL 4.6 capable graphics driver. The C/C++ runtime is linked statically; Visual Studio and CMake are not needed to play.

Validation: Release x64 build and WorldRules tests. Windows-only scoreboard rendering uses GDI.
