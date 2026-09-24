# SilverSt — Custom ZDL Effect for Zoom MultiStomp

SilverSt is an original, true-stereo clean amplifier and permanently active
open-back 2×12 cabinet for compatible Zoom MultiStomp pedals. It is designed
to turn the pedal into a compact end-of-chain preamp/cab voice: clean at its
default setting, with soft edge-of-breakup available from the Gain control.

It is an original DSP design. It is **not** a clone, capture, IR, or port of
any commercial amplifier or cabinet.

## Download and install

Download [SilverSt.ZDL](SilverSt.ZDL), connect the pedal over USB, and load
the file with [Zoom Effect Manager](https://zoomeffectmanager.com/en/download/).
Back up patches and firmware before changing effects.

SilverSt is released in the Delay category (`gid 0x08`) for broad custom-ZDL
compatibility, although its function is an amp and cabinet.

## Controls

| Control | Description |
| --- | --- |
| Gain | Input gain and soft edge-of-breakup. |
| Width | Moves the left and right cabinet-mic responses in complementary directions. It creates stereo width without a delay or phase trick. |
| Level | Post-cabinet output level. |
| Bass | Active low-frequency voicing. |
| Middl | Active mid-frequency voicing. |
| Trebl | Active high-frequency voicing. |
| Reson | Low-frequency power-stage / speaker damping character. |
| Prese | Upper-mid attack before the cabinet. |
| Mic | Cabinet microphone position, from centre/bright to edge/warm. |

## Signal design

Each channel remains discrete from input to output. The amp stage includes
gentle nonlinear shaping, dynamic sag and active three-band voicing; a
fixed open-back 2×12 cabinet response follows it. The only shared detector
is the sag envelope, so stereo inputs are never summed or crossed.

## Source

The pedal DSP source and build manifests are in
[`src/custom/silverst/`](src/custom/silverst/). `build.py` is the build helper
used within the [ZoomMultistompZDL](https://github.com/themanro/ZoomMultistompZDL)
toolchain; this repository ships the ready-to-install binary, so building is
not required for normal use.

## Credits

Built by Marcelo Fuentes using the custom-ZDL reverse-engineering and DSP
toolchain developed by [themanro and contributors](https://github.com/themanro/ZoomMultistompZDL).

## License

MIT. See [LICENSE](LICENSE).
