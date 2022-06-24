/*
 * vt100utils.h: An ANSI graphics escape sequence encoder/decoder
 */

#ifndef __VT100UTILS_H
#define __VT100UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "config.h"

/**
 * PREPROCESSOR
 */
#define MAX(a, b) (a > b ? a : b)

#define HAS_BOLD      0x1
#define HAS_UNDERLINE 0x4
#define HAS_BLINK     0x8

/**
 * STRUCTS and GLOBALS
 */
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

static struct vt100_color_t
  default_fg = { palette_8, 7 },
  default_bg = { palette_8, 0 };
static struct vt100_color_t
  global_fg = { palette_8, 7 },
  global_bg = { palette_8, 0 };
static uint8_t global_mode;

static char *empty_str = "";

/**
 * LIBRARY FUNCTIONS
 */

/*
 * vt100_sgr: Generate an escape sequence
 *   representing the given node's graphics
 *   data
 */
char *
vt100_sgr(struct vt100_node_t *node)
{
  char *buf = malloc(128);
  int len = sprintf(buf, "\x1b[");

  switch (node->fg.type) {
    case palette_8:
      len += sprintf(
        buf + len,
        "%i;",
        node->fg.value + 30
      );
      break;
    case palette_8_bright:
      len += sprintf(
        buf + len,
        "%i;",
        node->fg.value + 90
      );
      break;
    case palette_256:
      len += sprintf(
        buf + len,
        "38;5;%i;",
        node->fg.value
      );
      break;
    case truecolor:
      len += sprintf(
        buf + len,
        "38;2;%i;%i;%i;",
        (node->fg.value >> 16) & 0xff,
        (node->fg.value >> 8)  & 0xff,
        (node->fg.value >> 0)  & 0xff
      );
      break;
  }

  switch (node->bg.type) {
    case palette_8:
      len += sprintf(
        buf + len,
        "%i",
        node->bg.value + 40
      );
      break;
    case palette_8_bright:
      len += sprintf(
        buf + len,
        "%i",
        node->bg.value + 100
      );
      break;
    case palette_256:
      len += sprintf(
        buf + len,
        "48;5;%i",
        node->bg.value
      );
      break;
    case truecolor:
      len += sprintf(
        buf + len,
        "48;2;%i;%i;%i",
        (node->bg.value >> 16) & 0xff,
        (node->bg.value >> 8)  & 0xff,
        (node->bg.value >> 0)  & 0xff
      );
      break;
  }

  if (node->mode & HAS_BOLD)
    len += sprintf(buf + len, ";1");
  else
    len += sprintf(buf + len, ";21");

  if (node->mode & HAS_UNDERLINE)
    len += sprintf(buf + len, ";4");
  else
    len += sprintf(buf + len, ";24");

  if (node->mode & HAS_BLINK)
    len += sprintf(buf + len, ";5");
  else
    len += sprintf(buf + len, ";25");

  sprintf(buf + len, "m");

  return buf;
}

/*
 * vt100_encode: Encode a chain of nodes as
 *   a continuous string, for printing to
 *   the terminal
 */
char *
vt100_encode(struct vt100_node_t *node)
{
  int len = 0;
  int full_len = 0;
  int size;
  char *out = malloc((size = MAX(node->len, 32)));
  char *buf;

  struct vt100_node_t *tmp = node;

  while (tmp != NULL) {
    while (full_len + tmp->len + 30 > size) {
      out = realloc(out, (size *= 2));
    }

    buf = vt100_sgr(tmp);

    full_len += sprintf(
      out + full_len,
      "%s%s",
      buf,
      tmp->str
    );

    free(buf);

    tmp = tmp->next;
  }

  return out;
}

/*
 * vt100_parse: Parses a string beginning with
 *   "\x1b[" as a graphics/SGR escape sequence
 */
char *
vt100_parse(struct vt100_node_t *node, char *str)
{
  char *start = str + 2;
  char *end = start;
  char cpy[16];
  int args[256];
  int i = 0;
  int *to_modify1, *to_modify2;
  uint32_t tmp = 0;

  int j, state = 0;

  node->fg = global_fg;
  node->bg = global_bg;
  node->mode = global_mode;

  if (str[0] != '\x1b' || str[1] != '[')
    goto abort;

  for (;;) {
    switch (*end) {
    case 'm':
      args[i++] = atoi(start);
      for (j = 0; j < i; j++) {
        switch (state) {
        case 0:
          switch (args[j]) {
          case 0:
            node->fg = default_fg;
            node->bg = default_bg;
            node->mode = 0;
            break;
          /* GCC/Clang extension */
          case 30 ... 37:
            node->fg.type = palette_8;
            node->fg.value = args[j] - 30;
            break;
          case 40 ... 47:
            node->bg.type = palette_8;
            node->bg.value = args[j] - 40;
            break;
          case 90 ... 97:
            node->fg.type = palette_8_bright;
            node->fg.value = args[j] - 90;
            break;
          case 100 ... 107:
            node->bg.type = palette_8_bright;
            node->bg.value = args[j] - 100;
            break;
          case 1:
            node->mode |= HAS_BOLD;
            break;
          case 4:
            node->mode |= HAS_UNDERLINE;
            break;
          case 5:
            node->mode |= HAS_BLINK;
            break;
          default:
            state = args[j];
            break;
          }
          break;
        case 38:
          if (args[j] == 5) {
            if (j + 1 >= i || args[j + 1] < 0 || args[j + 1] > 255)
              goto abort;

            node->fg.type = palette_256;
            node->fg.value = args[++j];
          } else if (args[j] == 2) {
            if (j + 3 >= i)
              goto abort;

            node->fg.type = truecolor;

            node->fg.value = args[++j];
            node->fg.value <<= 8;
            node->fg.value |= args[++j];
            node->fg.value <<= 8;
            node->fg.value |= args[++j];
          } else {
            goto abort;
          }
          state = 0;
          break;
        case 48:
          if (args[j] == 5) {
            if (j + 1 >= i || args[j + 1] < 0 || args[j + 1] > 255)
              goto abort;

            node->bg.type = palette_256;
            node->bg.value = args[++j];
          } else if (args[j] == 2) {
            if (j + 3 >= i)
              goto abort;

            node->bg.type = truecolor;

            node->bg.value = args[++j];
            node->bg.value <<= 8;
            node->bg.value |= args[++j];
            node->bg.value <<= 8;
            node->bg.value |= args[++j];
          } else {
            goto abort;
          }
          state = 0;
          break;
        }
      }

      global_fg = node->fg;
      global_bg = node->bg;
      global_mode = node->mode;
      return end + 1;
    case ';':
      args[i++] = atoi(start);
      start = ++end;
      break;
    default:
      if (*end >= '0' && *end <= '9')
        end++;
      else
        goto abort;
      break;
    }
  }

abort:;
  node->fg = global_fg;
  node->bg = global_bg;
  node->mode = global_mode;
  return str + 1;
}

/*
 * vt100_decode: Decodes an input string
 *   into a chain of nodes
 */
struct vt100_node_t *
vt100_decode(char *str)
{
  struct vt100_node_t *head = malloc(sizeof(struct vt100_node_t)),
                      *cur  = head;
  char *start = str;
  char *end = str;

  cur->str = empty_str;
  cur->fg = global_fg;
  cur->bg = global_bg;

  for (;;) {
    switch (*end) {
    case '\0': /* Fall through */
    case '\x1b':
      if (end != start) {
        cur->str = malloc(end - start + 1);
        strncpy(cur->str, start, end - start);
        cur->str[end - start] = '\0';
        cur->len = end - start + 1;
      }

      if (*end == '\0')
        return head;

      cur->next = malloc(sizeof(struct vt100_node_t));
      cur = cur->next;
      cur->next = NULL;
      start = vt100_parse(cur, end);
      /* Fall through */
    default:
      end++;
      break;
    }
  }
}

void
vt100_free(struct vt100_node_t *head)
{
  struct vt100_node_t *tmp = head->next,
                      *prev = head;

  while (tmp != NULL) {
    if (prev->str != empty_str && prev->str != NULL)
      free(prev->str);
    free(prev);
    prev = tmp;
    tmp = tmp->next;
  }

  free(prev->str);
  free(prev);
}

#endif