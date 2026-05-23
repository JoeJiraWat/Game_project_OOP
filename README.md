# Cat Bomb

A local 2-4 player pass-and-play C++ GUI card game inspired by the provided Exploding Kittens class diagram.

## Build

```sh
cmake -S . -B build
cmake --build build
```

## Run

```sh
./build/catbomb
```

## Controls

- Click Start Game on the title screen.
- Choose 2, 3, or 4 players, click each name box, type a name, then click Begin Match.
- Click a card in the active player's hand to play it.
- Click Draw to draw from the top of the deck.
- Skip ends the turn without drawing.
- Draw From Bottom draws from the bottom of the deck and ends the turn.
- Favor and cat-combo rules ask for a target player.
- Defuse is used automatically when a Boom card is drawn.
- When a target player has Nope, the GUI gives that target a chance to cancel the action.

The implementation keeps the class names and responsibilities from the diagram: `GameManager`, `Player`, `NetworkManager`, `Card` plus derived card classes, `CardData`, `CardType`, `Effect`, `CatType`, `SpecialRule`, and the three special cat rules.

## Project Layout

- `src/main.cpp` starts raylib and runs the app loop.
- `src/GameModel.h` contains the OOP game model and card rules.
- `src/GameUi.cpp` / `src/GameUi.h` contain the GUI screens.
- `src/AssetManager.cpp` / `src/AssetManager.h` load card images, background, and music from `Card/`, `Background/`, and `Sound/`.

## Card Image Filenames

The game currently loads these card images when present:

- `Card/bomb.png`
- `Card/defuse.png`
- `Card/skip.png`
- `Card/favor.png` or the existing `Card/flover.png`
- `Card/shuffle.png`
- `Card/nope.png`
- `Card/draw_from_bottom.png`, `Card/draw_from_the_bottom.png`, or the existing `Card/draw_from_the_buttom.png`
- `Card/see_the_future.png`
- `Card/cat1.png` through `Card/cat5.png`
