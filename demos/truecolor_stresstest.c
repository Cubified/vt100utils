/*
 * truecolor_stresstest.c: Test terminal's truecolor printing ability
 *
 * Should produce a clean green-magenta gradient, with black in the top
 *   left and white in the bottom right
 */

#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <math.h>

#define VT100UTILS_SKIP_FORMATTING
#include "../vt100utils.h"

int main(){
  int x, y, len = 0;
  char *buf;
  struct winsize ws;
  struct vt100_node_t *head;

  ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
  buf = malloc(ws.ws_row * ws.ws_col * 20);

  for(y=0;y<ws.ws_row;y++){
    for(x=0;x<ws.ws_col;x++){
      len += sprintf(
        buf + len,
        "\x1b[48;2;%i;%i;%im ",
        (int)floor(((double)y/(double)ws.ws_row)*255.0f),
        (int)floor(((double)x/(double)ws.ws_col)*255.0f),
        (int)floor(((double)y/(double)ws.ws_row)*255.0f)
      );
    }
  }

  head = vt100_decode(buf);
  free(buf);

  printf("\x1b[?25l\n");

  buf = vt100_encode(head);
  puts(buf);
  free(buf);

  printf("\x1b[0m\x1b[?25h\x1b[0;0H\n");

  vt100_free(head);

  return 0;
}
