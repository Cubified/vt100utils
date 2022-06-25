# vt100utils

A single-header C library for encoding, decoding, and doing useful things with [ANSI graphics escape sequences](https://vt100.net/docs/vt510-rm/SGR.html).

This library is specifically targeted at graphics sequences because they are particularly useful for building terminal user interfaces, such as in my library [`tuibox`](https://github.com/Cubified/tuibox).

By separating plain (visible) text from its formatting, previously-challenging tasks such as text wrapping, formatting changes, and truncation become relatively straightforward.

## Demos

Content-aware text wrapping ([overflow.c](https://github.com/Cubified/vt100utils/blob/main/demos/overflow.c)):

![overflow.gif](https://github.com/Cubified/vt100utils/blob/main/gifs/overflow.gif)

Text truncation with animation ([hover.c](https://github.com/Cubified/vt100utils/blob/main/demos/hover.c)):

![hover.gif](https://github.com/Cubified/vt100utils/blob/main/gifs/hover.gif)

Per-word interactivity ([words.c](https://github.com/Cubified/vt100utils/blob/main/demos/words.c)):

![words.gif](https://github.com/Cubified/vt100utils/blob/main/gifs/words.gif)

## Features

- Decoder capable of parsing any arbitrary string and producing a list of text nodes
- Encoder capable of producing a terminal-compatible string from a list of text nodes
- Supports 8-color and 256-color palettes, as well as truecolor (16 million color) foreground and background
- Supports special formatting such as bold, underline, italic, and blinking text
- Fault-tolerant: Ignores unrecognized escape sequences (instead of aborting)

## Basic Usage

A sample program is as follows:

```c
#include "vt100utils.h"

int
main(void)
{
  struct vt100_node_t *head = vt100_decode("\x1b[32mHello world!\x1b[45;4mGoodbye."),
                      *tmp = head;

  while (tmp != NULL) {
    printf(
      "Text: %s\n  Foreground: %i\n  Background: %i\n  Mode: %i\n\n",
      tmp->str,
      tmp->fg.value,
      tmp->bg.value,
      tmp->mode
    );

    tmp = tmp->next;
  }

  vt100_free(head);

  return 0;
}
```

This code produces the following output:

```
Text:
  Foreground: 7
  Background: 0
  Mode: 0

Text: Hello world!
  Foreground: 2
  Background: 0
  Mode: 0

Text: Goodbye.
  Foreground: 2
  Background: 5
  Mode: 8
```

## The `vt100_node_t` and `vt100_color_t` Structs

These two structs encode information about a given text node, and are defined as follows:

```c
struct vt100_color_t {
  enum {
    palette_8,
    palette_8_bright,
    palette_256,
    truecolor
  } type;
  uint32_t value;
};

struct vt100_node_t {
  char    *str;
  int      len;
  struct vt100_color_t fg;
  struct vt100_color_t bg;
  uint8_t  mode;
  struct vt100_node_t *next;
};
```

When `vt100_decode` is called, a linked list consisting of `vt100_node_t` structs is built.  Each of these contain the following information:

- Their plain text content as a null-terminated string (as well as the length of the content)
- The text's foreground and background colors as `vt100_color_t` structs:
   - The type:  8-color palette, 8-color bright palette, 256-color palette, or truecolor
   - The value:  For palette-based color schemes, the index within the palette (e.g. 0 for color #30 in the 8-color standard palette).  For truecolor, an RGB-encoded `uint32_t`.
- The text's special formatting (its "mode"): Each bit represents one formatting type (e.g. bold, underline, italic, etc.).
   - To check for any single formatting type, AND this value with `1 << (i - 1)` where `i` is the ANSI code for enabling it (e.g. 1 for bold, 3 for italic, etc.)
- The pointer to the next text node in the chain

As is standard for ANSI escape sequences, nodes inherit the formatting of prior nodes unless overwritten.

Along with this, calling `vt100_encode` does not produce an identical string to the one passed to `vt100_decode`.  Instead, it should produce a string that _looks_ identical when rendered in a terminal.

## See Also

- [reflow](https://github.com/muesli/reflow):  An ANSI-sequence aware text reflow library written in Go
- [ANSI escape code reference](https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797)
