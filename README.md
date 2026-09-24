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

## Why it is designed to leave room in a patch

Stock Zoom amp models can reserve a very large share of a patch's DSP budget:
for example, their descriptors list **74.75%** for `FD COMBO` and **95.26%**
for `DELUXE-R`. SilverSt takes a deliberately compact route while retaining
an amp and cabinet in one block:

- small IIR filter sections model the voice and cabinet rather than a loaded
  IR or convolution engine;
- no delay lines, reverb, oversampling, model/cab selector, or alternative
  output paths;
- **256 B** of persistent working state, with no sample-buffer queue or
  dynamic allocation;
- true-stereo processing throughout, with complementary mic/cab responses
  creating Width without a Haas delay.

The current audio loop measures approximately **212 static cycles per sample**.
That makes SilverSt lighter by design than a feature-matrix amp model and is
intended to leave practical room for modulation, delay or reverb after it.
The final DSP admission percentage is firmware-dependent and has not yet been
measured on a physical pedal, so this is not presented as a guaranteed loading
claim.

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
