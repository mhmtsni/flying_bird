Flying Bird

Flying Bird is a 2D arcade-style game inspired by Flappy Bird, developed in C using SDL2. Players control a bird, navigating it through an endless series of pipes by making it jump. The goal is to avoid collisions and achieve the highest score possible.
Table of Contents

    Features
    Gameplay
    Installation
    Controls
    Screenshots
    Contributing
    License

Features

    Smooth physics-based movement.
    Procedurally generated obstacles (pipes).
    Real-time score tracking.
    Simple but addictive gameplay mechanics.
    Minimalistic UI with SDL_ttf for text rendering.

Gameplay

Control the bird and guide it through gaps between pipes. Each successful pass through a pair of pipes adds to your score. The game ends when the bird collides with a pipe or the ground.
Installation
Prerequisites

    C Compiler (e.g., GCC, MinGW, or Visual Studio)
    SDL2 library
    SDL2_image and SDL2_ttf libraries

Steps

    Clone the repository:

git clone https://github.com/yourusername/flying_bird.git
cd flying_bird

Install dependencies:

    On Linux:

sudo apt update
sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev

On macOS:

    brew install sdl2 sdl2_image sdl2_ttf

    On Windows:
    Download precompiled SDL2, SDL2_image, and SDL2_ttf binaries from their official sites, and ensure they are linked properly.

Compile the game:

gcc -o flying_bird flying_bird.c -lSDL2 -lSDL2_image -lSDL2_ttf

Run the game:

    ./flying_bird

Controls

    Spacebar: Make the bird jump.
    Escape: Exit the game.

Contributing

Contributions are welcome! Feel free to fork this repository, submit pull requests, or report issues.
License

This project is licensed under the MIT License.
Acknowledgments

    Developed using SDL2.
    Inspired by the classic game Flappy Bird.
