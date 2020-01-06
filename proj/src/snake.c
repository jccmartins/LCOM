#include "snake.h"

#include "graphics.h"

struct Snake *snake;
uint16_t pixels_per_unit = 25;

struct Apple *apple = NULL;

void(create_snake)()
{

  int snake_size = 4;

  snake = (struct Snake *)malloc(sizeof(struct Snake));
  snake->direction = RIGHT;
  snake->score = 0;

  struct Node *head = (struct Node *)malloc(sizeof(struct Node));

  snake->head = head;

  head->x = 4;
  head->y = 7;
  head->next = NULL;

  struct Node *last = head;

  for (int i = 2; i <= snake_size; i++)
  {
    struct Node *next = (struct Node *)malloc(sizeof(struct Node));
    next->x = last->x - 1;
    next->y = last->y;
    last->next = next;
    if (i == snake_size)
    {
      next->next = NULL;
    }
    else
    {
      last = next;
    }
  }
}

bool(is_snake_at_end_of_map)()
{

  uint16_t xRes = get_xRes();
  uint16_t yRes = get_yRes();

  switch (snake->direction)
  {
  case UP:
    if ((snake->head->y * pixels_per_unit) <= 0)
    {
      return true;
    }
    break;
  case DOWN:
    if (((snake->head->y + 1) * pixels_per_unit) >= yRes)
    {
      return true;
    }
    break;
  case LEFT:
    if ((snake->head->x * pixels_per_unit) <= 0)
    {
      return true;
    }
    break;
  case RIGHT:
    if (((snake->head->x + 1) * pixels_per_unit) >= xRes)
    {
      return true;
    }
    break;
  default:
    break;
  }

  return false;
}

bool(snake_apple_collision)() {
  struct Node *snake_piece = snake->head;
  if (apple != NULL) {
    while (snake_piece != NULL) {
      if ((snake_piece->x == apple->x) && (snake_piece->y == apple->y)) {
        return true;
      }
      snake_piece = snake_piece->next;
    }
  }
  return false;
}

bool(snake_snake_collision)()
{
  struct Node *tail = snake->head->next;

  while (tail != NULL)
  {
    if ((snake->head->x == tail->x) && (snake->head->y == tail->y))
    {
      return true;
    }
    tail = tail->next;
  }

  return false;
}

bool(move_snake)()
{

  if (snake_snake_collision())
  {
    return false;
  }

  struct Node *newHead = (struct Node *)malloc(sizeof(struct Node));

  switch (snake->direction)
  {
  case UP:
    if (is_snake_at_end_of_map())
    {
      newHead->x = snake->head->x;
      newHead->y = (get_yRes() - 1) / pixels_per_unit;
    }
    else
    {
      newHead->x = snake->head->x;
      newHead->y = snake->head->y - 1;
    }
    break;
  case DOWN:
    if (is_snake_at_end_of_map())
    {
      newHead->x = snake->head->x;
      newHead->y = 0;
    }
    else
    {
      newHead->x = snake->head->x;
      newHead->y = snake->head->y + 1;
    }
    break;
  case LEFT:
    if (is_snake_at_end_of_map())
    {
      newHead->x = (get_xRes() - 1) / pixels_per_unit;
      newHead->y = snake->head->y;
    }
    else
    {
      newHead->x = snake->head->x - 1;
      newHead->y = snake->head->y;
    }
    break;
  case RIGHT:
    if (is_snake_at_end_of_map())
    {
      newHead->x = 0;
      newHead->y = snake->head->y;
    }
    else
    {
      newHead->x = snake->head->x + 1;
      newHead->y = snake->head->y;
    }
    break;
  default:
    break;
  }

  newHead->next = snake->head;
  snake->head = newHead;

  if (snake_apple_collision())
  {
    apple = NULL;
    snake->score += 1;
  }
  else
  {
    struct Node *last = snake->head;
    struct Node *aux;

    while (last->next != NULL)
    {
      aux = last;
      last = last->next;
    }

    aux->next = NULL;
  }

  return true;
}

bool(change_snake_direction)(uint8_t byte)
{

  switch (byte)
  {
  case UP_ARROW_MAKE:
    if (snake->direction != DOWN)
    {
      snake->direction = UP;
      return true;
    }
    break;
  case DOWN_ARROW_MAKE:
    if (snake->direction != UP)
    {
      snake->direction = DOWN;
      return true;
    }
    break;
  case LEFT_ARROW_MAKE:
    if (snake->direction != RIGHT)
    {
      snake->direction = LEFT;
      return true;
    }
    break;
  case RIGHT_ARROW_MAKE:
    if (snake->direction != LEFT)
    {
      snake->direction = RIGHT;
      return true;
    }
    break;
  default:
    break;
  }

  return false;
}
