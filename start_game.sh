#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT_DIR"

echo "=== CatBomb Game Launcher ==="

echo "Detecting environment..."

EXE_NAME="catbomb"
if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" || "$OSTYPE" == "win32" ]]; then
  EXE_NAME="catbomb.exe"
fi

install_package() {
  local pkg="$1"
  if command -v apt-get >/dev/null 2>&1; then
    sudo apt-get update
    sudo apt-get install -y "$pkg"
    return
  fi
  if command -v dnf >/dev/null 2>&1; then
    sudo dnf install -y "$pkg"
    return
  fi
  if command -v pacman >/dev/null 2>&1; then
    sudo pacman -Sy --noconfirm "$pkg"
    return
  fi
  if command -v brew >/dev/null 2>&1; then
    brew install "$pkg"
    return
  fi
  if command -v zypper >/dev/null 2>&1; then
    sudo zypper install -y "$pkg"
    return
  fi
  echo "Package manager not found. Please install '$pkg' manually."
  exit 1
}

install_deps() {
  echo "Installing required dependencies..."
  if ! command -v cmake >/dev/null 2>&1; then
    echo "- Installing cmake"
    install_package cmake
  fi
  if ! command -v pkg-config >/dev/null 2>&1; then
    echo "- Installing pkg-config"
    install_package pkg-config
  fi
  if ! pkg-config --exists raylib 2>/dev/null; then
    echo "- Installing raylib"
    if command -v apt-get >/dev/null 2>&1; then
      install_package libraylib-dev
    elif command -v dnf >/dev/null 2>&1; then
      install_package raylib-devel
    elif command -v pacman >/dev/null 2>&1; then
      install_package raylib
    elif command -v brew >/dev/null 2>&1; then
      install_package raylib
    elif command -v zypper >/dev/null 2>&1; then
      install_package raylib-devel
    else
      echo "Could not automatically install raylib. Please install raylib manually."
      exit 1
    fi
  fi
}

if ! command -v cmake >/dev/null 2>&1 || ! command -v pkg-config >/dev/null 2>&1 || ! pkg-config --exists raylib 2>/dev/null; then
  echo "Dependencies missing or not configured."
  install_deps
else
  echo "Dependencies already installed."
fi

if ! pkg-config --exists raylib 2>/dev/null; then
  echo "raylib is still not found by pkg-config. Please install raylib and ensure pkg-config can find it."
  exit 1
fi

mkdir -p build
cmake -S . -B build
cmake --build build

echo "Launching game..."

if [[ -f "build/$EXE_NAME" ]]; then
  exec "./build/$EXE_NAME"
else
  echo "Executable build/$EXE_NAME not found. Build may have failed."
  exit 1
fi
