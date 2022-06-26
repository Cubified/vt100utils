/*
 * hover.c: A simple truncation/hover animation
 */

#include "../vt100utils.h"
#include "tuibox.h"

#define MIN(a, b) (a < b ? a : b)

ui_t u;
struct vt100_node_t *head;
int w = 12;

void
draw(ui_box_t *b, char *out)
{
  struct vt100_node_t *tmp = head->next;
  char *sgr;
  int len = 0;
  int full_len = 0;

  while (tmp != NULL) {
    sgr = vt100_sgr(tmp, NULL);
    full_len += sprintf(
      out + full_len,
      "%s%.*s",
      sgr,
      MAX(0, w - len),
      tmp->str
    );
    len += tmp->len;
    free(sgr);
    tmp = tmp->next;
  }
  sprintf(
    out + full_len,
    "%s\n",
    w < 47 ? "..." : ""
  );
}

void
click(ui_box_t *b, int x, int y)
{
  while (w < 50) {
    w++;
    ui_draw(&u);
    usleep(10000);
  }
}

void
hover(ui_box_t *b, int x, int y, int down)
{
  if (down) {
    click(b, x, y);
  } else {
    while (w > 12) {
      w--;
      ui_draw(&u);
      usleep(10000);
    }
  }
}

void
stop()
{
  ui_free(&u);
  vt100_free(head);
  exit(0);
}

int
main(void)
{
  head = vt100_decode("\x1b[36mClick to see \x1b[4;38;5;125mt\x1b[38;5;127mh\x1b[38;5;130mi\x1b[38;5;135ms \x1b[38;5;140mt\x1b[38;5;145me\x1b[38;5;150mx\x1b[38;5;155mt\x1b[0;36m un-truncate!");

  ui_new(0, &u);

  ui_add(
    UI_CENTER_X, UI_CENTER_Y,
    35, 1,
    0,
    NULL, 0,
    draw,
    click,
    hover,
    NULL,
    NULL,
    &u
  );

  ui_key("q", stop, &u);

  ui_draw(&u);

  ui_loop(&u) {
    ui_update(&u);
  }

  return 0;
}
