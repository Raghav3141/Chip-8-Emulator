# =============================
# CHIP-8 Emulator Build Script
# =============================

# Paths to SDL3
$SDLInclude = "C:\Users\Raghav\Dropbox\SDL3-devel-3.2.12-mingw\SDL3-3.2.12\x86_64-w64-mingw32\include"
$SDLLib     = "C:\Users\Raghav\Dropbox\SDL3-devel-3.2.12-mingw\SDL3-3.2.12\x86_64-w64-mingw32\lib"

# Output name
$Output = "chip8.exe"

# Compiler command
$Command = "g++ main.cpp Chip8.cpp platform.cpp -o $Output -I`"$SDLInclude`" -L`"$SDLLib`" -lSDL3 -std=c++17 -Wall"

Write-Host "Building CHIP-8 emulator..."
Write-Host $Command

# Run the compiler
Invoke-Expression $Command

if ($LASTEXITCODE -eq 0) {
    Write-Host "✅ Build successful. Starting debugger..."
    # Run the program in gdb
    gdb ./chip8.exe
} else {
    Write-Host "Build failed. Check the errors above."
}
