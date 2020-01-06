#ifndef SNAKE_H
#define SNAKE_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

/** @defgroup snake snake
 * @{
 * Snake functions and structs used.
 * 
 * \author João Carlos Carreira Martins
 * \author João Francisco de Pinho Brandão
 */

/** @brief Create snake struct with size 4, score 0 and direction RIGHT */
void(create_snake)();
/** @brief Checks if snake is at the end of the map */
bool(is_snake_at_end_of_map)();
/** @brief Detects snake/apple collision, true if there is a collision, false if not */
bool(snake_apple_collision)();
/** @brief Detects snake/snake collisions
 * \n Returns true if there is a collision and false if not
 */
bool(snake_snake_collision)();
/** @brief Move snake, true if snake can move, false if not (when snake collides with itself)
 * \n If snake collides with apple creates a new head for the snake and returns true
 * \n If snake collides with itself returns false
 * \n If there are no collisions creates a new head for the snake and deletes the last node
 */
bool(move_snake)();
/** @brief Change snake direction, using keyboard arrows
 * \n Returns true if it can change direction, false otherwise (if new direction is the opposite of the current one)
 */
bool(change_snake_direction)(uint8_t byte);

/** @brief Stores x and y coordinates and a struct node pointer */
struct Node
{
  int x;
  int y;
  struct Node *next;
};

/** @brief Stores snake information 
 * @param head - Stores position of the head of the snake and a struct node pointer to the next piece
 * @param score - Score of the snake, equals to the number of apples eaten
 * @param direction - Snake current direction (Left, Up, Right or Bottom)
 */
struct Snake
{
  struct Node *head;
  uint16_t score;
  uint16_t direction;
};

/** @brief Stores apple position (x and y) */
struct Apple
{
  int x;
  int y;
};

#define LEFT 0
#define UP 1
#define RIGHT 2
#define DOWN 3

#endif
