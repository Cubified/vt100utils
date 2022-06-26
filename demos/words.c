/*
 * words.c: Per-word events
 */

#include "../vt100utils.h"
#include "tuibox.h"

#define MIN(a, b) (a < b ? a : b)

ui_t u;
struct vt100_node_t *head;

void
draw(ui_box_t *b, char *out)
{
  struct vt100_node_t *node = b->data1;
  char *sgr = vt100_sgr(node, NULL);

  sprintf(out, "%s%s", sgr, node->str);
  free(sgr);
}

void
click(ui_box_t *b, int x, int y)
{
  struct vt100_node_t *node = b->data1;
  node->fg.value += 10;
  if (node->fg.value > 255)
    node->fg.value = 10;
  ui_draw(&u);
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
  struct vt100_node_t *tmp;
  int x;
  int y;

  /* String manually generated with Javascript */
  head = vt100_decode("\x1B[38;5;100mClick\x1B[38;5;101many\x1B[38;5;102mword\x1B[38;5;103mto\x1B[38;5;104mchange\x1B[38;5;105mits\x1B[38;5;106mcolor!\x1B[38;5;107mThis\x1B[38;5;108mis\x1B[38;5;109ma\x1B[38;5;110mlong\x1B[38;5;111mparagraph\x1B[38;5;112mof\x1B[38;5;113mtext,\x1B[38;5;114mand\x1B[38;5;115mevery\x1B[38;5;116mword\x1B[38;5;117mcan\x1B[38;5;118mbe\x1B[38;5;119mclicked.\x1B[38;5;120mWhile\x1B[38;5;121mbehavior\x1B[38;5;122mlike\x1B[38;5;123mthis\x1B[38;5;124mis\x1B[38;5;125mpossible\x1B[38;5;126mwith\x1B[38;5;127mstandalone\x1B[38;5;128mtuibox\x1B[38;5;129m(or\x1B[38;5;130mother\x1B[38;5;131mlibraries),\x1B[38;5;132mit\x1B[38;5;133mwould\x1B[38;5;134mbe\x1B[38;5;135mincredibly\x1B[38;5;136mchallenging\x1B[38;5;137mand\x1B[38;5;138mcumbersome.\x1B[38;5;139mThis\x1B[38;5;140mdemo\x1B[38;5;141mis\x1B[38;5;142mwritten\x1B[38;5;143min\x1B[38;5;144mless\x1B[38;5;145mthan\x1B[38;5;146m100\x1B[38;5;147mlines\x1B[38;5;148mof\x1B[38;5;149mcode.");

  ui_new(0, &u);

  x = (u.ws.ws_col - 50) / 2;
  y = (u.ws.ws_row - 10) / 2;

  tmp = head->next;
  while (tmp != NULL) {
    ui_add(
      x, y,
      tmp->len, 1,
      0,
      NULL, 0,
      draw,
      click,
      NULL,
      tmp,
      NULL,
      &u
    );
    x += tmp->len;
    if (x > (u.ws.ws_col + 50) / 2) {
      x = (u.ws.ws_col - 50) / 2;
      y += 2;
    }
    tmp = tmp->next;
  }

  ui_key("q", stop, &u);

  ui_draw(&u);

  ui_loop(&u) {
    ui_update(&u);
  }

  return 0;
}
