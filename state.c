#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t* state, unsigned int snum);
static char next_square(game_state_t* state, unsigned int snum);
static void update_tail(game_state_t* state, unsigned int snum);
static void update_head(game_state_t* state, unsigned int snum);

/* Task 1 */
game_state_t* create_default_state() {
  // TODO: Implement this function
  game_state_t *default_state = malloc(sizeof(game_state_t));
  default_state->num_rows = 18;
  default_state->num_snakes = 1;

  snake_t *s = malloc(sizeof(snake_t) * default_state->num_snakes);
  s->tail_row = 2;
  s->tail_col = 2;
  s->head_row = 2;
  s->head_col = 4;
  s->live = true;
  default_state->snakes = s;
  
  char **board_d = malloc(sizeof(char*) * default_state->num_rows);
  for (size_t i = 0; i < default_state->num_rows; i++) {
    board_d[i] = malloc(sizeof(char) * 21);
    if (i == 0 || i == 17) {
      strcpy(board_d[i], "####################");
      continue;
    }
    if (i == 2) {
      strcpy(board_d[i], "# d>D    *         #");
      continue;
    }
    strcpy(board_d[i], "#                  #");
  }
  default_state->board = board_d;

  return default_state;
}

/* Task 2 */
void free_state(game_state_t* state) {
  // TODO: Implement this function.
  free(state->snakes);
  state->snakes = NULL;
  for (size_t i = 0; i < state->num_rows; i++) {
    free(state->board[i]);
    state->board[i] = NULL;
  }
  free(state->board);
  state->board = NULL;
  free(state);
  state = NULL;
}

/* Task 3 */
void print_board(game_state_t* state, FILE* fp) {
  // TODO: Implement this function.
  for (size_t i = 0; i < state->num_rows; i++) {
    fprintf(fp, "%s\n", state->board[i]);
  }
  return;
}

/*
  Saves the current state into filename. Does not modify the state object.
  (already implemented for you).
*/
void save_board(game_state_t* state, char* filename) {
  FILE* f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_state_t* state, unsigned int row, unsigned int col) {
  return state->board[row][col];
}

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  // TODO: Implement this function.
  char tail[] = "wasd";
  for (size_t i = 0; i < strlen(tail); i++) {
    if (c == tail[i]) {
      return true;
    }
  }
  return false;
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  // TODO: Implement this function.
  char head[] = "WASDx";
  for (size_t i = 0; i < strlen(head); i++) {
    if (c == head[i]) {
      return true;
    }
  }
  return false;
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  // TODO: Implement this function.
  char snake[] = "wasd^<v>WASDx";
  for (size_t i = 0; i < strlen(snake); i++) {
    if (c == snake[i]) { 
      return true;
    }
  }
  return false;
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
  // TODO: Implement this function.
  if (c == '^') {
    return 'w';
  }
  if (c == '<') {
    return 'a';
  }
  if (c == 'v') {
    return 's';
  }
  else {
    return 'd';
  }
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
  // TODO: Implement this function.
  if (c == 'W') {
    return '^';
  }
  if (c == 'A') {
    return '<';
  }
  if (c == 'S') {
    return 'v';
  }
  else {
    return '>';
  }
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  // TODO: Implement this function.
  if (c == 'v' || c == 's' || c == 'S') {
    return cur_row + 1;
  }
  if (c == '^' || c == 'w' || c == 'W') {
    return cur_row - 1;
  }
  else {
    return cur_row;
  }
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  // TODO: Implement this function.
  if (c == '>' || c == 'd' || c == 'D') {
    return cur_col + 1;
  }
  if (c == '<' || c == 'a' || c == 'A') {
    return cur_col - 1;
  }
  else {
    return cur_col;
  }
}

/*
  Task 4.2

  Helper function for update_state. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  snake_t* snake = state->snakes;
  unsigned int snake_row = snake[snum].head_row;
  unsigned int snake_col = snake[snum].head_col;
  char head = get_board_at(state, snake_row, snake_col);
  unsigned int row = get_next_row(snake_row, head);
  unsigned int col = get_next_col(snake_col, head);
  char next = get_board_at(state, row, col);
  return next;
}

/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  snake_t* snake = state->snakes;
  unsigned int head_r = snake[snum].head_row;
  unsigned int head_c = snake[snum].head_col;
  char head = get_board_at(state, head_r, head_c);
  unsigned int row = get_next_row(head_r, head);
  unsigned int col = get_next_col(head_c, head);
  snake[snum].head_row = row;
  snake[snum].head_col = col;
  set_board_at(state, row, col, head);
  set_board_at(state, head_r, head_c, head_to_body(head));
  return;
}

/*
  Task 4.4

  Helper function for update_state. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  snake_t* snake = state->snakes;
  unsigned int tail_r = snake[snum].tail_row;
  unsigned int tail_c = snake[snum].tail_col;
  char tail = get_board_at(state, tail_r, tail_c);
  unsigned int row = get_next_row(tail_r, tail);
  unsigned int col = get_next_col(tail_c, tail);
  char body = get_board_at(state, row, col);
  char new_tail = body_to_tail(body);
  snake[snum].tail_row = row;
  snake[snum].tail_col = col;
  set_board_at(state, row, col, new_tail);
  set_board_at(state, tail_r, tail_c, ' ');
  return;
}

/* Task 4.5 */
void update_state(game_state_t* state, int (*add_food)(game_state_t* state)) {
  // TODO: Implement this function.
  snake_t* snakes = state->snakes;
  for (size_t i = 0; i < state->num_snakes; i++) {
    if (snakes[i].live == false) {
      continue;
    }
    unsigned int head_r = snakes[i].head_row;
    unsigned int head_c = snakes[i].head_col;
    char head = get_board_at(state, head_r, head_c);
    unsigned int next_r = get_next_row(head_r, head);
    unsigned int next_c = get_next_col(head_c, head);
    char next = get_board_at(state, next_r, next_c);
    if (is_snake(next) || next == '#') {
        snakes[i].live = false;
        set_board_at(state, head_r, head_c, 'x');
    } else if (next == '*') {
      update_head(state, i);
      add_food(state);
    } else {
    update_head(state, i);
    update_tail(state, i);
    }
  }
  return;
}

/* Task 5 */
game_state_t* load_board(char* filename) {
  // TODO: Implement this function.
  game_state_t* state = malloc(sizeof(game_state_t));
  state->board = malloc(sizeof(char*));
  FILE *file = fopen(filename, "r");
  char buffer[255];
  fgets(buffer, 255, file);
  buffer[strlen(buffer) - 1] = '\0';
  state->board[0] = malloc(sizeof(char) * (strlen(buffer)+1));
  strcpy(state->board[0], buffer);
  for (size_t i = 1; fgets(buffer, 255, file); i++) {
    state->board = realloc(state->board, sizeof(char*) * (i+1));
    state->board[i] = malloc(sizeof(char) * (strlen(buffer)+1));
    buffer[strlen(buffer) - 1] = '\0';
    strcpy(state->board[i], buffer);
    state->num_rows = i;
  }
  state->num_rows = state->num_rows + 1;
  fclose(file);
  return state;
}

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  snake_t* snake = state->snakes;
  char tail;
  unsigned int tail_r = 0, tail_c = 0, count = 0;
  bool notfound = true;
  for (size_t i = 0; i < state->num_rows && notfound; i++) {
    for (size_t j = 0; j < strlen(state->board[i]); j++) {
      tail = get_board_at(state, i, j);
      if (is_tail(tail)) {
        count++;
      }
      if (snum == count - 1) {
        tail_r = i;
        tail_c = j;
        notfound = false;
        break;
      }
    }
  }
  state->snakes[snum].tail_row = tail_r;
  state->snakes[snum].tail_col = tail_c;
  char next = tail;
  unsigned int next_r = tail_r;
  unsigned int next_c = tail_c;
  unsigned int head_r = 0, head_c = 0;
  char head = next;
  while (is_snake(next)) {
    head_r = next_r;
    head_c = next_c;
    head = next;
    next_r = get_next_row(next_r, next);
    next_c = get_next_col(next_c, next);
    next = get_board_at(state, next_r, next_c);
  }
  state->snakes[snum].head_row = head_r;
  state->snakes[snum].head_col = head_c;
  if (head == 'x') {
    state->snakes[snum].live = false;
  } else {
    state->snakes[snum].live = true;
  }
  return;
}

/* Task 6.2 */
game_state_t* initialize_snakes(game_state_t* state) {
  // TODO: Implement this function.
  char tail;
  int count = 0;
  for (size_t i = 0; i < state->num_rows; i++) {
    for (size_t j = 0; j < strlen(state->board[i]); j++) {
      tail = get_board_at(state, i, j);
      if (is_tail(tail)) {
        count++;
      }
    }
  }
  state->num_snakes = count;
  state->snakes = malloc(sizeof(snake_t) * count);
  for (size_t i = 0; i < count; i++) {
    find_head(state, i);
  }
  return state;
}
