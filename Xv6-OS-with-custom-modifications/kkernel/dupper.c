// "hello" device driver"

#include "defs.h"
#include "file.h" // for struct devsw
#include "sleeplock.h"
#include "spinlock.h"
#include "types.h"

struct upper_t {
  struct spinlock lock;
  int front;
  int rear;
};

static struct upper_t s_upper;

// Circular queue from
// https://www.geeksforgeeks.org/c-program-to-implement-circular-queue/ Define
// the maximum size of the queue
#define MAX_SIZE 512

// Declare the queue array and s_upper.front, s_upper.rear variables
static int s_queue[MAX_SIZE];

// Function to check if the queue is full
static int s_isFull() {
  // If the next position is the s_upper.front, the queue is full
  return (s_upper.rear + 1) % MAX_SIZE == s_upper.front;
}

// Function to check if the queue is empty
static int s_isEmpty() {
  // If the s_upper.front hasn't been set, the queue is empty
  return s_upper.front == -1;
}

// Function to enqueue (insert) an element
static void s_enqueue(int data) {
  // If the queue is full, print an error message and
  // return
  if (s_isFull()) {
    // cprintf("Queue overflow\n");
    return;
  }
  // If the queue is empty, set the s_upper.front to the first
  // position
  if (s_upper.front == -1) {
    s_upper.front = 0;
  }
  // Add the data to the queue and move the s_upper.rear pointer
  s_upper.rear = (s_upper.rear + 1) % MAX_SIZE;
  s_queue[s_upper.rear] = data;
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
  // Get the data from the s_upper.front of the queue
  int data = s_queue[s_upper.front];
  // If the s_upper.front and s_upper.rear pointers are at the same
  // position, reset them
  if (s_upper.front == s_upper.rear) {
    s_upper.front = s_upper.rear = -1;
  } else {
    // Otherwise, move the s_upper.front pointer to the next
    // position
    s_upper.front = (s_upper.front + 1) % MAX_SIZE;
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
  iunlock(ip);

  acquire(&s_upper.lock);
  while (s_isEmpty()) {
    wakeup(&s_upper.rear);
    sleep(&s_upper.front, &s_upper.lock);
  }
  dst[0] = (char)s_dequeue();
  wakeup(&s_upper.rear);
  release(&s_upper.lock);

  ilock(ip);
  return 1;
}

int upperwrite(struct inode *ip, char *buf, int n) {
  iunlock(ip);

  acquire(&s_upper.lock);
  for (int i = 0; i < n; ++i) {
    while (s_isFull()) {
      wakeup(&s_upper.front);
      sleep(&s_upper.rear, &s_upper.lock);
    }
    s_enqueue(s_toUpper(buf[i]));
  }
  wakeup(&s_upper.front);
  release(&s_upper.lock);

  ilock(ip);
  return n;
}

void upperinit(void) {
  devsw[UPPER].write = upperwrite;
  devsw[UPPER].read = upperread;
  s_upper.front = -1;
  s_upper.rear = -1;
}
