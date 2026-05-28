# Step 11 — DAW verification notes

Working scratchpad for Phase 3 Step 11 (end-to-end DAW verification). Once all items are PASS, Phase 3 is done.

---

## Status

| # | Item | Status | Host(s) | Notes |
|---|---|---|---|---|
| 1 | Plugin scans & instantiates | PASS | Ableton Live | |
| 2 | GUI opens at correct size, all controls visible | PASS | Ableton Live | |
| 3 | All 22 params in automation panel | — | | not checked yet |
| 4 | State survives session save/reopen | — | | not checked yet |
| 5 | uiMode switch (Vibe↔Tweak) toggles component set | PASS | Ableton Live | |
| 6 | Vibe ↔ Tweak param independence | — | | not checked yet |
| 7 | Style switch hides/shows Knee + Peak/RMS | — | | not checked yet |
| 8 | Audio behavior — Modern VCA + Peak, + RMS, Opto | — | | needs volume up — deferred |
| 9 | **Bypass via host: clean toggle, no clicks, no PDC drift** | DEFERRED | | needs volume up — re-test tomorrow |
| 10 | **I/O type selectors — M/S in + M/S out passthrough** | DEFERRED | | needs volume up — re-test tomorrow |
| 11 | **Vibe-mode audio behavior (compress, react, feel, focus)** | DEFERRED | | needs volume up — re-test tomorrow |
| 11a | Feel of Compress/React linear sweeps + skews | DEFERRED | | needs volume up — re-test tomorrow |
| 12 | SC HPF/LPF audibly affect detection | — | | needs volume up — deferred |
| 13 | Lookahead reports latency (4 ms) | — | | not checked yet |
| 14 | No SC routed: no compression, gain still applies | — | | not checked yet |
| 15 | Style switch under heavy GR — no NaN/clicks | — | | not checked yet |
| 16 | Load old v1 patches — new params default | — | | not checked yet |

---

## Known issues found during Step 11

### 1. VUMeter output Mid doesn't visibly reduce under compression (FLAGGED FOR PHASE 4)

**Symptom:** With aggressive comp (threshold = -35 dB, ratio = 20:1, SC +12 dB) the GR meter shows immense reduction but the output Mid bar barely drops. Only at threshold = -100 dB is the drop obvious.

**Investigation:**
- Signal flow verified correct (mid is compressed at PluginProcessor.cpp:434, outMidBuffer captures the compressed mid at :436, outMidLevel reads it at :462/:475).
- 0.5× scale fix applied to `inSideLevel` for consistency with `inMidLevel`.
- VUMeter dB floor tightened from -100 dB to -60 dB (VUMeter.cpp:63).
- New `TestSignalChainGainReduction` in TestMain.cpp proves 19 dB of GR → 32% bar drop. Math is correct.

**Resolution:** Purely a display-scaling issue. Defer to Phase 4 GUI redesign — tighten the meter floor further (-48 or -36 dB) and align with the new visual language. CLAUDE.md Phase 4 note updated.

---

## To-do tomorrow

Re-test items 8, 9, 10, 11, 11a, 12 with volume up. Focus on the *feel* of the Vibe-mode macro sweeps (compress, react, focus) to validate the skews/ranges in CLAUDE.md's "Vibe-mode macro derivations" section feel musical, not just mathematically correct.
