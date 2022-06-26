/*
 * overflow.c: A text box with correct wrapping
 */

#include "../vt100utils.h"
#include "tuibox.h"

#define MIN(a, b) (a < b ? a : b)

ui_t u;
struct vt100_node_t *head;
int w = 50;

void
draw(void)
{
  struct vt100_node_t *tmp;
  int x = 0;
  int off = 0;
  char *sgr;

  printf("\x1b[0;0H\x1b[2J\x1b[36m(Press \"q\" to exit)\n\x1b[32mColumn width: %i\x1b[0m\n\n", w);

  tmp = head->next;

  printf("        \x1b[35m┌");
  for (x = 1; x < w + 2; x++) {
    printf("─");
  }
  printf("┐\n        ");

  while (tmp != NULL) {
    x = 0;

    printf("│\x1b[0m ");
    while (x < w) {
      sgr = vt100_sgr(tmp, NULL);
      printf("%s%.*s", sgr, MIN(w - x - 1, tmp->len - off - 1), tmp->str + off);
      if (tmp->len - off - 1 > w - x - 1) {
        off += w - x - 1;
        x = w;
      } else {
        x += tmp->len - off - 1;
        off = 0;
        tmp = tmp->next;

        if (tmp == NULL) {
          x++;
          free(sgr);
          break;
        }
      }
      free(sgr);
    }

    for (; x < w; x++) {
      printf(" ");
    }
    printf(" \x1b[35m│\n        ");
  }

  printf("\x1b[35m└");
  for (x = 1; x < w + 2; x++) {
    printf("─");
  }
  printf("┘");

  printf("\n\n\x1b[0m");
}

void
shrink()
{
  if (w > 4)
    w--;
  draw();
}

void
grow()
{
  if (w < u.ws.ws_col - 3)
    w++;
  draw();
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
  head = vt100_decode("\x1b[31mLorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. \x1b[32mUt enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. \x1b[33mDuis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. \x1b[34mExcepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.");

  ui_new(0, &u);

  ui_key("\x1b[C", grow, &u);
  ui_key("\x1b[D", shrink, &u);
  ui_key("q", stop, &u);

  draw();

  ui_loop(&u) {
    ui_update(&u);
  }

  return 0;
}
