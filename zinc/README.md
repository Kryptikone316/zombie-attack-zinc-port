# Zombie Attack — Zinc SF3500 port

This is a Zinc-language rewrite of the original GPLv3 C/SDL Zombie Attack
source. Zinc owns the game state, timers, movement, spawning, combat, score,
upgrades, and main loop. A small C host layer supplies the proven SF3500/H.OS
display, joystick, timing, and audio boundaries.

Controls:

- D-pad: move
- A: fire/select upgrade
- B: cancel the upgrade menu; Start pauses/resumes gameplay, or reopens a
  pending upgrade offer
- Upgrades open automatically when score reaches the next level threshold
- Select + Start: emergency exit to the frontend

The executable opens at a Zinc-built main menu with Play Classic Game, Play
Enhanced Game, Settings, How To Play, and Exit Game. Settings are stored as
`data/settings.cfg` so volume choices survive a return to the frontend. The
current host has generated sound effects but no music stream, so the music
value is retained for future audio work.

Classic mode keeps the original three humanoid enemy classes, straight
unlimited shots, and one-time upgrade limits. Enhanced mode enables the added
farm enemies, mutations, pickups, rescue events, support systems, and ranked
upgrades.

The build keeps the original BMP and WAV assets in `data/` for this first
hardware validation build.

The current baseline uses score as experience/currency: tapping Start cannot
bypass the threshold. Bullets use the original red sprite sheet and their
collision position follows the player’s battlefield position, while sprite
depth is drawn in front/behind the player according to Y position.

Enhanced mode also includes typed walkers, feral zombies, heavy tank zombies,
and diseased/zombified farm animals: chickens, pigs, sheep, goats, cows,
uncommon llamas, and exceedingly rare unicorn elites. Sheep, goats, and llamas
occupy distinct stat tiers between pigs and cows. Occasional tinted mutations
can inflict poison damage-over-time or temporarily cripple movement; status
icons appear above the commando. Rare farm-themed pickups provide health
restoration, damage, movement speed, firing-rate, or ammunition replenishment.
Rifle ammunition is finite; at zero, the commando falls back to a weak,
slower pistol shot. Every shot has a cooldown and ordinary fire has bounded
spread, while Laser Aim improves both accuracy and damage.

Rare zombie crows fly in irregular vertical weaving patterns while drifting
toward the commando's area. They have low health and contact damage, making
them a nuisance that is advantageous to clear rather than a major threat.

A neutral squirrel periodically emerges beside the tree hole. It throws small
"bad nuts" that lightly damage nearby things, then retreats back into the
tree. The squirrel cannot be targeted or killed; its nuts are the only active
hazard.

Enhanced `BEAGLE BUDDY` has five ranks. The beagle fights nearby enemies,
shows a health bar, retreats to the broken-fence position when exhausted, and
returns after a rank-scaled recovery period.

After the rescue event begins, three piglets flee zombies and the Big Bad Wolf
strongly while lightly avoiding the commando. Carry them to the barn one at a
time; the HUD tracks the saved count. The wolf arrives after a short search
delay, hunts the piglets, and becomes vulnerable to gunfire only after all
three are safely inside.

Cows occasionally leave visible cow-pie land-mines behind them. A pie lasts
10 seconds, deals 2 damage on contact, and is subject to an eight-hazard cap.

All farm-animal enemies use visibly undead artwork: sickly coloration, dark
faces, red eyes, and wound marks. The humanoid enemy classes retain the
original zombie artwork.

The primary HUD uses larger, separated text and displays a sun icon during
daytime or a crescent moon at night.

Enemy sprites face the commando based on their position, and the depth renderer
draws every active enemy consistently.

The Laser Aim upgrade displays a thin light-red line that ends at the first
aligned target and modestly improves projectile accuracy and damage.

All ten Enhanced upgrades have five ranks. Wood Chipper and Lawn Mower improve
their duration, damage, and cadence by rank; Time Stop periodically freezes
hostile enemies with a longer freeze and shorter cooldown at higher ranks.
The rescue-pig event repeats after its wolf encounter ends.
Start pauses the game, and Select+Start remains the emergency frontend exit.
