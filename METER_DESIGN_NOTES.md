# Meter design notes — Phase 4 GUI redesign

Reference for the metering redesign during Phase 4. Captures the chosen visualization concept, rationale, implementation sketch, and design decisions already made.

---

## Chosen concept: stereo-position level curve with pre/post layered ghost

Replaces the current two clusters of three VU bars (input L/M/R + output L/M/R) with a single horizontal visualizer showing **level as a curve across the stereo field**, with input and output drawn as overlapping translucent shapes.

### Visual language

- Horizontal axis: stereo position. **L at left edge, C at middle, R at right edge.**
- Vertical axis: level in dB (log-scaled, range TBD — likely ~-36 to +3 dB).
- Two curves rendered simultaneously:
  - **Input curve** (pre-compression) — black / neutral color, drawn first.
  - **Output curve** (post-compression) — red / accent color, drawn on top.
- Each curve is closed to the bottom and filled with translucent color → the "ghost" effect emerges naturally where the two overlap.
- Characteristic visual: a dip in the red curve at center, while the edges align with the black curve. That dip *is* the GR on the mid, drawn.

### What this communicates at a glance

- **The plugin's effect:** the dip at center is the entire identity of Center Space, made visual.
- **Stereo balance:** asymmetry in L vs R reads immediately.
- **In/out gain matching:** if the two curves meet at the L and R edges, gain staging is balanced. If red sits consistently below black across the whole curve, output gain is low.
- **Side-heavy material:** a U-shaped curve (low center, taller edges) tells the user "this is already wide content — the comp has less center to grab."
- **GR depth:** the gap between black and red at center is the GR, readable without a separate meter (though we're keeping the dedicated GR meter anyway).

---

## Implementation sketch (JUCE)

Straightforward `juce::Path` work driven off the existing 60 Hz editor timer.

Per frame:

1. **Sample three points** for input: L level (`inLevelChan0`), C level (`inMidLevel`, already `M·0.5`), R level (`inLevelChan1`).
2. **Sample three points** for output: same three corresponding atomics.
3. **Convert to dB**, map to vertical pixel position via log scale.
4. **Build the input path:** `startNewSubPath` at left edge, `quadraticTo` or Catmull-Rom through the C point to the right edge, `lineTo` down to bottom corners, `closeSubPath`.
5. **Build the output path:** same construction with output values.
6. **Fill input path** with translucent neutral color.
7. **Fill output path** with translucent accent color (drawn on top).
8. **Stroke** the top edge of each path with the solid version of its color for definition.

Optional refinements:

- Use 5 sample points instead of 3 — interpolate intermediate "left-mid" and "right-mid" levels as weighted blends of L and C, R and C. Gives the curve more shape definition without adding new audio-side state.
- Small dots at each true sample point to show where the data actually is.
- Subtle drop shadow under each curve for depth.

### Smoothing / ballistics

The curve sample points need the same rise/fall decay logic as the current VU bars or the visualization will look jittery. Reuse `HeightMultiplier`-style ballistics (one rise/fall state per sampled point — 3 or 5 total).

---

## Design decisions already made

These are locked in based on the brainstorm conversation — don't relitigate during Phase 4 unless something concrete changes.

| Topic | Decision | Reasoning |
|---|---|---|
| Center value source | `inMidLevel` (= M·0.5) and `outMidLevel` | Matches existing meter taps; encodes "mid" perceptually at center position |
| L/R value source | `inLevelChan0`/`outLevelChan0` and chan1 equivalents | Same taps as current L/R bars |
| Alternate "M for center, S for L/R" framing | Rejected | Side is one value — would draw flat across L and R, losing stereo-position meaning |
| Number of sample points | Start with 3, optionally extend to 5 for curve definition | 3 is enough for the visual story; more is polish |
| dB axis | Log (proper dB), range TBD (~-36 to +3 likely) | Sketch was illustrative only |
| Side-heavy signal handling | Accept as-is; user reads "wide source, little center to grab" from shape | Honest representation of what's happening |
| Mono signal handling | Accept as-is; flat line dipping uniformly | Document in user manual that mono defeats the plugin's purpose |
| SC level meter | Keep separate | Distinct from stereo-field info; user needs it |
| GR meter | Keep separate | Distinct from stereo-field info; user needs it (curve gap shows GR too, but dedicated meter gives precise read) |

---

## Open questions for Phase 4

Things to figure out during the design pass, not now:

1. **Exact dB range** for the vertical axis. -36 to +3? -48 to +3? Tune for visual readability of typical material.
2. **Smoothing constants** for rise/fall. Should match the perceived speed of the current VU bars or be slightly different for the curve aesthetic.
3. **Whether to add a visual GR delta highlight** between the input and output curves at center — e.g., a brighter shaded zone, or a small numeric "GR: X.X dB" overlay. Helps when center is already quiet pre-comp.
4. **Color palette and translucency levels** for the two curves. Needs to be readable on both the current dark theme and whatever Phase 4 lands on.
5. **Side meter and SC meter placement** around the new central visualizer. Probably small bars flanking it.
6. **Behavior when input/output type = M/S.** In that mode, "L" and "R" are actually M and S. Curve becomes "M level at center, S level at edges" — which is a different visual meaning. Decide: relabel axis, switch to a different visualization, or accept the meaning shift.

---

## Why this is the right call

Most comps with stereo metering use vectorscopes or correlation meters — both informative but generic, used the same on every comp regardless of what the plugin does. A "level across stereo position" curve is uncommon and directly visualizes what Center Space uniquely does: hold the sides constant while pulling the middle down. The visualization *is* the product description.
