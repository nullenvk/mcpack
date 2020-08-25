# mcpack - a Minecraft 1.12.2 protocol serialization library

## Usage:

TODO: Write usage

### Format specification

The format string used in `mc_unpack` and `mc_pack` functions uses single-char type specifiers,
described in the following table:

| Character | Parameters | Meaning |
|:---------:| ---------- | ------- |
| 1         | `char` | Fixed size 8-bit value |
| 2         | `short` | Fixed size 16-bit value |
| 4         | `int` | Fixed size 32-bit value |
| 8         | `long` | Fixed size 64-bit value |
| i         | `int` | Varint - variable size value (max 5 bytes)
| I         | `long` | Varlong - variable size value (max 10 bytes)
| s         | `char*` | NULL-terminated string (with prepended size in varint) |

