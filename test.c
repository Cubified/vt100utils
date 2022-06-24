/*
 * test.c: A simple test for vt100utils
 */
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
