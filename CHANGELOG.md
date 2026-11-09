# Changelog

## 2.0.0

- Preserved the original 773 x 384 reference-panel layout, including the
  original hardware labels, photographic knobs, switches, and enclosure.
- Preserved the original reference image's photographed dial faces and added
  interactive rotation by turning the exact artwork with each control.
- Replaced the erroneous meter bar with a moving, peak-responsive analog
  needle, illuminated scale, and independent input peak light.
- Updated all three illuminated tape-head displays to show each head's actual
  delay in milliseconds, including live host-tempo changes.
- Set the default head selector and switches to the single active Head 1
  configuration shown in the supplied reference.
- Retained the original toggle artwork and made tempo sync, input send, wet
  solo, tape loop, and power indicators follow their real parameter states.
- Added synchronized 70%-150% interface sizing from the header and scaled the
  background art, controls, labels, and meter together.
- Added INFO options for Galaxy Tape, Loops, license notices, and the public
  source repository.
- Added thirteen production starting points for vocals, drums, bass, guitar,
  keys, dub, ambience, and tape-bus effects.
- Added **Visit Loops** as the checked post-install installer action.
- Updated the Windows installer and GitHub Release to publish the versioned
  `Galaxy-Tape-v2.0.0-Setup.exe`.
- Avoided unnecessary audio-thread reconfiguration of JUCE's reverb when the
  tape-stock mode has not changed.
- Extended the circular delay buffer to the longest supported tempo-synced
  note length at the plugin's lowest tempo, so long-note sync cannot silently
  truncate to two seconds.

## 1.0.0

- Initial 64-bit Windows VST3 tape echo and reverb.
- Added the supplied interface art, parameter controls, A/B state comparison,
  five starting presets, and the Windows installer workflow.
