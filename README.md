# mcpack - a Minecraft 1.12.2 protocol serialization library

## About

This library is meant for serializing and deserializing data from Minecraft's networking
protocol. However, it doesn't implement anything related to packets, because that'd
require both use of dynamic memory allocation and external dependencies (which we'd like to
avoid). If you'd like to handle packet decoding/encoding/compression/encryption, you should
either write your own wrapper functions around this library or use another library implementing it.
I might make a separate library for that later on.

Here are some features of this library:

* Rather small (<1K LOC)
* Supports usage without dynamic memory allocation
* No external dependencies

## Usage

`mc_buffer_t` is the most basic structure in mcpack library, meant for describing the output/input
buffer for unpack/pack functions. Moreover, it allows you to specify whether you'd like to
dynamically allocate the data buffer or use a static one. In order to create a `mc_buffer_t`, you
have to use either the `mc_buffer_new` or the `mc_buffer_new_static` functions. After you're
finished with using a buffer, you can clean it up with `mc_buffer_free` (if it's a static buffer,
it won't clean up the data).
In order to deserialize data from a buffer, you have to first create a `mc_buffer_t` with use of
`mc_buffer_make`, which will encapsulate your data for this purpose. Then, you have to use
`mc_unpack` function on it with proper format data. If you'd like to use just one function
instead of two, you can also use `mc_unpack_raw`, which can use raw byte arrays.
In order to serialize data, you have to set up a new `mc_buffer_t`. Then, you have to use `mc_pack` on
it. Afterwards, you should be able to read the data by accessing structure's `data` field.
Output size will be available through the `size` field.

### Return codes

If any of these functions fails, it should return `-1`.

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
| s         | `char*` | NULL-terminated string (with prepended size in varint). Dynamically allocated when unpacked |
| B (TODO)  | `long long, unsigned char*` | Varint-prepended byte array. Dynamically allocated when unpacked|

