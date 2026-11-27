# Zombie Attack mode and change log

This file is the working record of differences between the supplied original
Zombie Attack source and the H.OS Zinc modes. The original source remains
under `Installer/ZombieAttack/` and is never edited by the port.

## Mode contract

| Mode | Contract | Status |
| --- | --- | --- |
| Classic | Preserve the supplied game's rules, upgrade model, timing, and one-time upgrade flags. | In progress |
| Enhanced | Preserve the original as a baseline while adding bounded ranked stat upgrades and optional configuration knobs. | In progress |

## Recorded differences

| Area | Original behavior | Enhanced behavior | Classic requirement |
| --- | --- | --- | --- |
| Platform | SDL 1.2 desktop executable | Zinc game logic with a small H.OS C ABI host | H.OS rendering must not alter game rules |
| Display | 640x480, 8-bit SDL surface | 640x480 RGB565 software framebuffer presented by `hwdisp` | Same logical viewport |
| Input | Keyboard events | SF3500 `joykey`; D-pad/A/B/Start/Select mapping | Map controls without changing rules |
| Exit | SDL quit / escape path | Select+Start emergency exit | Preserve safe return path |
| Bullets | Original red 6x3 directional sheet | Finite rifle ammunition, slower pistol fallback, cooldown, bounded spread, nearest-target collision, and Laser Aim accuracy/damage assistance | Match original collision model |
| Upgrades | Ten one-time flags; no rank system | All ten Enhanced choices have five meaningful ranks; packed rank state remains Zinc-side | Ten one-time flags |
| Upgrade menu | Blocking level-up menu | Threshold-triggered menu; B/Start cancel; owned ranks disabled | Preserve original selection semantics where possible |
| Audio | SDL mixed WAV channels | H.OS boundary; generated tones in current baseline | Original WAV behavior is a separate fidelity task |
| Enemies | Three base zombie types with shared life behavior | Typed zombie, farm-animal, and aerial crow profiles, player-level scaling, armor, contact damage, rare mutations, and status effects | Preserve original three-type behavior |
| Pickups | None | Rare weighted health, damage, speed, rapid-fire, and ammo-crate pickups plus cow-pie hazards with farm-themed sprites | No pickups |

## Enhanced rank policy

All ten Enhanced upgrades now have five meaningful ranks:

- Fast Walk, More Shots, Fast Shoot, More Life, and More Damage improve player
  statistics at each rank;
- Laser Aim improves shot accuracy and bonus damage at each rank;
- Wood Chipper and Lawn Mower improve support duration, cadence, and damage;
- Time Stop increases freeze duration while shortening its cooldown;
- Beagle Buddy improves health, active duration, attack strength, and recovery.

The rank count is stored in a packed Zinc dword so the menu can display
current/max ranks without adding a platform-sensitive ABI structure.

## Update entries

### 2026-11-26 — Enhanced ranked-upgrade foundation

- Added packed per-upgrade rank state to Enhanced mode.
- Added caps and threshold progression for repeatable stat upgrades.
- Kept utility upgrades one-time and visible as owned/disabled.
- Added this mode/difference log as the source of truth for future Classic vs
  Enhanced work.

### 2026-11-26 — H.OS gameplay corrections

- Restored the original red bullet asset in both directions.
- Corrected bullet collision to use the battlefield Y coordinate.
- Added Y-based foreground/background sprite ordering.
- Removed free Start-key upgrades; offers are score-threshold gated.

### 2026-11-26 — Enhanced enemy variation and pickups

- Added walker, feral, and heavy/tank stat profiles.
- Added rare type-aware mutations with altered life, speed, armor, and contact
  damage; mutation tinting makes them distinguishable on the battlefield.
- Added diseased/zombified farm animals: fast chickens, bruising pigs, sturdy
  sheep, mobile goats, cow tanks, uncommon llamas, and exceedingly rare
  unicorn elites. Sheep, goats, and llamas have separate silhouettes and stat
  profiles between the pig and cow tiers.
- Reworked every farm-animal sprite to read as undead at a glance, using
  sickly coloration, darkened faces, red eyes, and visible wounds. The original
  zombie sprite remains the visual basis for the humanoid enemy classes.
- Added mutation-driven poison damage-over-time and cripple movement effects,
  with status icons above the commando.

### 2026-11-26 — Enemy facing and render stability

- Added per-enemy facing so enemies entering from the left mirror the right-side
  spawn presentation and enemies turn toward the commando as they approach.
- Fixed the painter's-algorithm loop to reset after pickup rendering; the old
  loop skipped zombie slots 0–2 whenever those enemies were above the player,
  producing apparent blinking or disappearance.

### 2026-11-26 — Laser Aim visual guide

- Added a thin light-red laser line extending from the commando toward the
  current facing direction when Laser Aim is owned.
- Kept Laser Aim visual-only; projectile collision and damage rules are not
  changed by the guide.
- Added limited health and temporary damage-boost pickups.
- Added farm-themed 24×24 palette sprites: a barn health pickup and pitchfork
  damage pickup.
- Added temporary Speed Boost and Rapid Fire pickups aligned with the ranked
  Fast Walk and Fast Shoot upgrade paths. Pickup rarity is weighted to 40%
  health, 30% damage, 20% speed, and 10% rapid fire.
- Added capped, temporary cow-pie land-mines. Cows periodically leave a pie
  behind them; contact deals 2 damage and consumes the hazard.
- Added rare zombie crows with a two-frame wing animation and irregular
  vertical weaving. Crows are weak nuisance enemies that create a moving area
  hazard without becoming a full tank threat.
- Added a three-rank Beagle Buddy Enhanced upgrade. Each rank increases the
  dog's health, active time, attack strength, and recovery cadence; damage can
  send it retreating to the broken-fence position until it recovers.
- Added the three-pig rescue event: piglets flee hostile enemies strongly and
  the commando lightly, can be carried one at a time to the barn, and display
  a saved counter. The Big Bad Wolf arrives after a search delay, prioritizes
  piglets, remains invulnerable until all three are saved, and can then be
  defeated by gunfire.

### 2026-11-27 — Enhanced combat, pacing, and support systems

- Laser Aim now stops at the first aligned target, modestly narrows shot spread,
  and adds a small damage bonus; ordinary fire still misses occasionally.
- Added rifle ammunition, ammo-crate replenishment, a reload/cooldown between
  shots, and a weak single-shot pistol fallback when the rifle is empty.
- Corrected bullet selection to damage the nearest intersected target rather
  than whichever enemy happened to occupy the first array slot.
- Replaced doubling level thresholds with an unbounded escalating threshold;
  elite, llama, unicorn, and crow frequency increases with level.
- Start now pauses/resumes gameplay; Select+Start remains the emergency exit.
- Beagle Buddy protects the nearest threats to the commando, retreats behind
  the broken fence at zero health, uses a subdued health bar, and returns after
  its rank-scaled recovery period.
- The pig rescue sequence now re-arms after the current wolf leaves, allowing
  the “the piglets got loose again” event to recur.
- Added visible Wood Chipper and Lawn Mower support upgrades. The mower clears
  ground enemies; the chipper periodically attacks airborne crows.
- Converted every upgrade entry to a five-rank progression with a measurable
  effect per rank; Time Stop now periodically freezes hostile zombies and the
  wolf instead of being an inert menu choice.

### 2026-11-27 — Neutral tree squirrel and HUD readability

- Added a recurring neutral squirrel that emerges beside the tree hole, tosses
  cracked bad nuts, and retreats after a short visit. Nuts lightly affect
  nearby player, enemies, dog, wolf, and rescue piglets; the squirrel itself
  is never a target and cannot be damaged.
- Enlarged the primary HUD text, gave it dedicated spacing, and added compact
  sun/day and crescent-moon/night indicators to prevent text collisions.

### 2026-11-27 — Piglet delivery and firearm spread correction

- Widened and lowered the barn entrance hand-off zone so carried piglets can be
  delivered without pixel-perfect positioning at the far-left edge.
- Tightened normal rifle grouping from a broad ±12-pixel spread to a small
  ±4-pixel grouping, while retaining a low-probability wide miss.
- Tightened Laser Aim grouping further and raised its accuracy without making
  every shot guaranteed to hit.

### 2026-11-27 — Zinc main menu and settings shell

- Added an in-game Zinc launcher with Play Classic Game, Play Enhanced Game,
  Settings, How To Play, and Exit Game entries.
- Added persisted sound, music, and global volume settings under the port's
  existing data directory; music is stored for future stream playback because
  the current H.OS host produces generated effects only.
- Kept Select+Start as a global emergency exit from the launcher and every
  gameplay/menu state.
- Wired the mode choice into the baseline rules: Classic restricts enemy
  spawning to the original three humanoid classes, restores straight unlimited
  shots, suppresses enhanced support/event systems, and caps upgrades at one
  purchase; Enhanced retains the expanded systems and five-rank progression.

### 2026-11-27 — Menu contrast and Enhanced ammunition balance

- Replaced the bright yellow menu selection fill with a dark neutral highlight
  so the existing white text remains legible.
- Increased Enhanced starting rifle ammunition from 60 to 100 rounds.
- Increased the ammo-crate weighting to 40%, increased crate replenishment to
  30 rounds, expanded the active pickup slots, and shortened pickup intervals
  to reduce first-level ammunition starvation.

### 2026-11-27 — Numeric font glyph correction

- Replaced malformed number glyphs and corrected the affected letter glyph
  lengths in the embedded 5×7 font table. Every glyph now has a complete,
  bounds-safe 35-pixel pattern, preventing digits from borrowing neighboring
  glyph data and rendering as letters or corrupted shapes.

### 2026-11-27 — HUD, help text, and piglet event readability

- Reduced the in-game HUD typography to the compact scale while retaining the
  larger dedicated menu/help typography.
- Reflowed the How To Play page so its instructional lines use the larger scale
  without running outside the 640×480 viewport.
- Corrected the settings digit glyph rows with verified 5×7 patterns.
- Increased piglet pickup tolerance and rescue durability so all three targets
  remain practically deliverable one at a time instead of being lost while the
  player reaches the next piglet.

### 2026-11-27 — Menu typography pass

- Enlarged launcher, settings, upgrade, pause, and game-over menu text while
  retaining panel bounds and readable contrast.
- Kept the How To Play page on its larger, reflowed layout.

### 2026-11-27 — Two-column control guide

- Reworked How To Play into a two-column control/action table for D-Pad, A, B,
  Start, and Select+Start.
- Added a dedicated plus-sign glyph so the emergency-exit control is rendered
  correctly by the embedded Zinc host font.
- Added a dedicated hyphen glyph and corrected the control label to `D-PAD`.

### 2026-11-27 — Throttled Enhanced commando dialogue

- Added two-line comic-style dialogue bubbles above the commando with dark
  text on a pale speech panel and a small tail.
- Added varied farm-pun, zombie-action, and 1980s-action-spoof lines for each
  enemy family, piglet pickup/delivery, wolf sighting/defeat, crow and unicorn
  sightings, support equipment, Beagle Buddy activation, pickups, and the
  squirrel.
- Added a 2.8-second speech duration and active-bubble suppression so lines
  remain occasional rather than becoming combat spam.
- Added dedicated apostrophe and period glyphs for punctuation such as the cow
  variation `DON'T HAVE A COW! WAIT...`.
- Added a rarer chicken variation: `STOP CHICKEN ME OUT... / IT'S CREEPY`.

### 2026-11-27 — Expanded anti-hero enemy banter

- Expanded every zombie family to at least three selectable kill lines, with
  four for sheep and unicorns; rare chicken and cow branches retain their
  dedicated farm puns.
- Reworked the delivery toward an original snarky action-hero voice: groan-
  level wordplay, cynical one-liners, and light fourth-wall flavor without
  making every kill produce a comment.

### 2026-11-27 — Occasional farm jokes

- Added a separate 24–42 second farm-joke cooldown with a small pool of
  standalone cow, bull, scarecrow, and fog puns. Jokes only appear when no
  combat caption is active, keeping the commando chatty without making him
  constant background noise.
- Added a second, event-gated joke pool with Red Riding Hood and fairy-tale
  references; those lines cannot appear until the piglet rescue has actually
  summoned the Big Bad Wolf.
- Added a rare chicken-kill punchline: `WHY DID THE CHICKEN CROSS THE FARM? /
  WE'LL NEVER KNOW NOW THAT YOU'RE DEAD!`.
- Added a rare goat-kill punchline: `TOTES MA'GOATS. / YOU'RE GOING TO MAKE A
  LOVELY TOTE!`.
- Added a rare llama-kill punchline: `OH GREAT... / LLAMA DRAMA...`.

### 2026-11-27 — Ambient commando story pool

- Added twelve three-part ambient monologues for Enhanced mode. They mix farm
  anecdotes, tactical observations, self-important war-hero taunts, and light
  fourth-wall humor.
- Stories are shown one bubble at a time, with a long cooldown and a separate
  anti-repeat mask so all twelve are used before any can recur. Ambient stories
  yield to enemy, event, pickup, and support-system dialogue.
- Added rare generic combat quips for the humanoid zombie classes: `TAKE THAT!
  / AND THAT!` and `EAT LEAD... / OR AT LEAST IT MIGHT BE LEAD`.
- Added restrained colloquial speech to selected ambient lines, including
  `SHOOTIN'`, `LOSIN'`, `MAKIN'`, `SOMETHIN'`, and `UNDERSTANDIN'`.

### 2026-11-27 — Damage reaction dialogue

- Added occasional Enhanced-mode reactions when the commando takes damage,
  including generic impact quips, a poison-specific complaint, and a farm-
  hazard reaction. Damage dialogue is throttled by the existing speech bubble
  suppression so repeated hits and poison ticks remain readable.
- Added four Beagle Buddy retreat lines. They trigger once when the buddy is
  exhausted or its active duty period ends, then it runs back to the fence for
  recovery.
- Expanded pickup reactions to four choices per category, adding obvious-but-
  cheeky lines for ammo, damage, health, speed, and rapid-fire pickups.
- Added a separate six-line pickup-denial taunt pool aimed at enemies who miss
  the item: `FINDERS KEEPERS`, `TOO SLOW, CORPSE`, and similar anti-hero
  remarks. These appear occasionally and do not immediately repeat.
- Added eight long-cooldown scenery comments covering the barn, tree, broken
  fence, crops, day/night cycle, and the generally questionable scenery. They
  are context-aware for day versus night and avoid immediate repeats.
- Added four rare soundtrack-awareness quips, including `A RADIO MIGHT HAVE
  BEEN NICE...`, to acknowledge the game's intentionally music-free presentation
  with light fourth-wall snark.
- Added a separate rare six-line “director commentary” pool. The commando can
  argue with the nonexistent director, demand more zombies, reject a stunt
  double, or complain about billing as if the farm were a film set.
- Added eight rarer transitional lines that blend the farm setting, tactical
  duty, movie framing, oversized bravado, and the possibility that this is the
  commando's last day. These bridge the other dialogue pools into a more
  coherent character voice instead of making each joke feel isolated.
