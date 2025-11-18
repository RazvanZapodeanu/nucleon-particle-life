# Nucleon

 Particle life simulator with spatial grid optimization and multithreading. Explore emergent patterns from simple interaction rules using an interactive UI.
## Features

- **Physics Simulation**
    - Spatial grid partitioning for efficient neighbor queries
    - OpenMP multithreading for parallel force calculations
    - Handles thousands of particles smoothly

- **Interactive Controls**
    - Real-time parameter adjustment via ImGui
    - Mouse interaction (attract/repel particles)
    - Configurable particle types
    - Randomize interaction rules

- **Visual Options**
    - Smooth circular particle rendering
    - Optional glow effect
    - Motion trail visualization
    - Adjustable simulation speed and particle size

- **Simulation Features**
    - Toroidal space (wrap-around boundaries)
    - Customizable attraction/repulsion matrices

## Technical Stack

- C++23
- SDL3
- Dear ImGui
- OpenMP
- CMake

## Build Instructions

### Windows (MinGW/CLion)

1. **Clone the repository**
```bash
git clone https://github.com/RazvanZapodeanu/nucleon-particle-life.git
cd nucleon-particle-life
```

2. **Download SDL3**
    - Download `SDL3-devel-mingw.tar.gz` from [SDL3 Releases](https://github.com/libsdl-org/SDL/releases)
    - Extract to project root, rename folder to `SDL3/`

3. **Download ImGui**
    - Download source code from [ImGui Releases](https://github.com/ocornut/imgui/releases)
    - Extract to project root, rename folder to `imgui/`

4. **Build**
    - Open project in CLion
    - CMake will configure automatically
    - Build and run

## Usage

- **Left Click + Hold:** Repel particles
- **Right Click + Hold:** Attract particles
- **Randomize Rules:** Generate random interaction patterns
- **Adjust sliders:** Fine-tune all parameters in real-time

## Acknowledgments

Inspired by [Jeffrey Ventrella's Clusters](http://www.ventrella.com/Clusters/)

---
