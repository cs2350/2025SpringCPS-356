#include "stat.h"
#include "types.h"
#include "user.h"

int main(int argc, const char *argv[]) {
  const char* message = "Stuff\n";
  for (int i = 0; i < 1000; ++i) {
    for(int j = 0; j < 7; ++j) {
      write(1, &message[j], 1);
    }
  }
}