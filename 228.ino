#include <GyverMAX7219.h>
MAX7219 < 1, 1, 5 > mtrx; 

#define WIDTH 8
#define HEIGHT 8
#define START_X 1
#define START_Y 4

#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

struct point {
  int x;
  int y;
};

int direction = RIGHT; 
struct point snake[WIDTH*HEIGHT]; 
int snake_length = 2;
struct point food;

bool is_collision(int x, int y) {
  if (snake[0].x == x && snake[0].y == y)
    return true;
  for (int i = 1; i < snake_length; i++) {
    if (snake[i].x == x && snake[i].y == y)
      return true;
  }
  return false;
}

void draw_field(bool ate_food) {
  if (!ate_food) {
    mtrx.clear();
    for (int i = 0; i < snake_length; ++i) {
      mtrx.dot(snake[i].x, snake[i].y);
    }
    mtrx.dot(food.x, food.y);
    mtrx.update();
    delay(1000);
  }
}

void generate_food() {
  randomSeed(analogRead(A0)); 
  int x, y;
  do {
    x = random(0, WIDTH);
    y = random(0, HEIGHT);
  } while (is_collision(x, y) || (x == 0 && y == 0));
  food.x = x;
  food.y = y;
}

void initialize_snake() {
  snake[0].x = START_X + 1;
  snake[0].y = START_Y;
  snake[1].x = START_X;
  snake[1].y = START_Y;
}


void restart() {
  for (int i = 0; i < 8; i++) { 
    mtrx.clear();
    mtrx.line(0, i, 7, i);
    mtrx.update();
    delay(228);
  }
  mtrx.clear();
  initialize_snake();
  snake_length = 2; 
  direction = RIGHT;
  generate_food(); 
} 

void move_snake(bool &ate_food) {
  int i;
  for (i = snake_length-1; i > 0; i--) {
    snake[i].x = snake[i-1].x;
    snake[i].y = snake[i-1].y;
  }
  switch (direction) {
    case UP:
      snake[0].y--;
      break;
    case DOWN:
      snake[0].y++;
      break;
    case LEFT:
      snake[0].x--;
      break;
    case RIGHT:
      snake[0].x++;
      break;
  }
  if (snake[0].x == -1 || snake[0].x == WIDTH || snake[0].y == -1 || snake[0].y == HEIGHT) {
    restart();
  }
  for (i = 1; i < snake_length; i++) {
    if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
      restart();
    }
  }
  if (snake[0].x == food.x && snake[0].y == food.y) {
    snake_length++;
    generate_food();
    ate_food = true;
  } else {
    ate_food = false;
  }
}

void process_input() {
  if (direction != DOWN && !digitalRead(A4))
    direction = UP;
  else if (direction != UP && !digitalRead(A3)) 
    direction = DOWN;
  else if (direction != RIGHT && !digitalRead(A1))
    direction = RIGHT;
  else if (direction != LEFT && !digitalRead(A2))
    direction = LEFT;
}

void setup() {
  mtrx.begin();
  mtrx.setBright(5);
  pinMode(A1, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);
  pinMode(A3, INPUT_PULLUP);
  pinMode(A4, INPUT_PULLUP);
  restart();
}

void loop() {
  bool ate_food = false;
  process_input();
  move_snake(ate_food);
  draw_field(ate_food);
  delay(100);
}
