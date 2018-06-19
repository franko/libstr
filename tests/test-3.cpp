#include "str_cpp.h"

int main() {
    str s = str::format("Hello %s!", "world");
    printf("%s\n", s.text());
    return 0;
}
