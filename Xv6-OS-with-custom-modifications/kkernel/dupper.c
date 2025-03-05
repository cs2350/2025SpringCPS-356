// "hello" device driver"

#include "defs.h"
#include "file.h" // for struct devsw
#include "sleeplock.h"
#include "spinlock.h"
#include "types.h"

static  int front = -1;
static  int rear = -1;

// Circular queue adapted from
// https://www.geeksforgeeks.org/c-program-to-implement-circular-queue/ Define
// the maximum size of the queue
#define MAX_SIZE 512

// Declare the queue array and front, rear variables
static int s_queue[MAX_SIZE];

// Function to check if the queue is full
static int s_isFull() {
  // If the next position is the front, the queue is full
  return (rear + 1) % MAX_SIZE == front;
}

// Function to check if the queue is empty
static int s_isEmpty() {
  // If the front hasn't been set, the queue is empty
  return front == -1;
}

// Function to enqueue (insert) an element
static void s_enqueue(int data) {
  // If the queue is full, print an error message and
  // return
  if (s_isFull()) {
    // cprintf("Queue overflow\n");
    return;
  }
  // If the queue is empty, set the front to the first
  // position
  if (front == -1) {
    front = 0;
  }
  // Add the data to the queue and move the rear pointer
  rear = (rear + 1) % MAX_SIZE;
  s_queue[rear] = data;
  // cprintf("Element %d inserted\n", data);
}

// Function to dequeue (remove) an element
static int s_dequeue() {
  // If the queue is empty, print an error message and
  // return -1
  if (s_isEmpty()) {
    // cprintf("Queue underflow\n");
    return -1;
  }
  // Get the data from the front of the queue
  int data = s_queue[front];
  // If the front and rear pointers are at the same
  // position, reset them
  if (front == rear) {
    front = rear = -1;
  } else {
    // Otherwise, move the front pointer to the next
    // position
    front = (front + 1) % MAX_SIZE;
  }
  // Return the dequeued data
  return data;
}

static char s_toUpper(char c) {
  static const char span = 'a' - 'A';
  if ('a' <= c && 'z' >= c) {
    c = c - span;
  }
  return c;
}

int upperread(struct inode *ip, char *dst, int n) {
  while (s_isEmpty()) {
  }
  dst[0] = (char)s_dequeue();

  return 1;
}

int upperwrite(struct inode *ip, char *buf, int n) {
  for (int i = 0; i < n; ++i) {
    while (s_isFull()) {
    }
    s_enqueue(s_toUpper(buf[i]));
  }

  return n;
}

void upperinit(void) {
  devsw[UPPER].write = upperwrite;
  devsw[UPPER].read = upperread;
}
