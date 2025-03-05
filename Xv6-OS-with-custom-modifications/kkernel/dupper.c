// "hello" device driver"

#include "defs.h"
#include "file.h" // for struct devsw
#include "sleeplock.h"
#include "spinlock.h"
#include "types.h"

/// @brief This type stores a spin lock and teh data protected by the spin lock
/// The front and read members are used by the enqueue() and dequeue() and
/// isFull() and isEmpty() functions.
struct upper_t {
  struct spinlock lock; ///<- the lock
  int front;  /// <- next index to read (also used as sleep lock)
  int rear; /// <- next index to write (also used as sleep lock)
};

static struct upper_t s_upper;

// Circular queue adapted from
// https://www.geeksforgeeks.org/c-program-to-implement-circular-queue/ Define
// the maximum size of the queue
#define MAX_SIZE 512

// Declare the queue array and front, s_upper.rear variables
static int s_queue[MAX_SIZE];

// Function to check if the queue is full
static int s_isFull() {
  // If the next position is the front, the queue is full
  return (s_upper.rear + 1) % MAX_SIZE == s_upper.front;
}

// Function to check if the queue is empty
static int s_isEmpty() {
  // If the s_upper.fronthasn't been set, the queue is empty
  return s_upper.front== -1;
}

// Function to enqueue (insert) an element
static void s_enqueue(int data) {
  // If the queue is full, print an error message and
  // return
  if (s_isFull()) {
    // cprintf("Queue overflow\n");
    return;
  }
  // If the queue is empty, set the s_upper.frontto the first
  // position
  if (s_upper.front== -1) {
    s_upper.front= 0;
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
  // Get the data from the s_upper.frontof the queue
  int data = s_queue[s_upper.front];
  // If the s_upper.frontand s_upper.rear pointers are at the same
  // position, reset them
  if (s_upper.front== s_upper.rear) {
    s_upper.front= s_upper.rear = -1;
  } else {
    // Otherwise, move the s_upper.frontpointer to the next
    // position
    s_upper.front= (s_upper.front+ 1) % MAX_SIZE;
  }
  // Return the dequeued data
  return data;
}

/// @brief Returns the ASCII uppercase version of c. Nonalphabetic
/// characters are returned unchanged.
/// @param c : character to convert
/// @return : upper case version of c
static char s_toUpper(char c) {
  static const char span = 'a' - 'A';
  if ('a' <= c && 'z' >= c) {
    c = c - span;
  }
  return c;
}

/// @brief This is the read function for the dupper device
/// @param ip : The inoe pointer for the idone representing the device
/// @param dst : Buffere to store up to n characters read
/// @param n : maximum number of caharcetrs to read
/// @return number of characters actually read and stored in dst
int upperread(struct inode *ip, char *dst, int n) {    
  iunlock(ip);
  acquire(&s_upper.lock);

  for(int i = 0; i < n; ++i) {
    while (s_isEmpty()) {
      wakeup(&s_upper.rear);
      sleep(&s_upper.front, &s_upper.lock);
    }
    dst[i] = (char)s_dequeue();
  }
  wakeup(&s_upper.rear);
  release(&s_upper.lock);
  ilock(ip);

  return n;
}

/// @brief This is the write function of teh dupper device
/// @param ip : inode pointer for teh device
/// @param buf : The buffer containng chatacyers to write
/// @param n : The number of characters written 
/// @return returns n (any other value is an error)
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

/// @brief THis function initailizes the dupper device
/// @param  
void upperinit(void) {
  devsw[UPPER].write = upperwrite;
  devsw[UPPER].read = upperread;
  s_upper.front= -1;
  s_upper.rear = -1;
}
