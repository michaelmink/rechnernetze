#include <stdio.h>

int main(){

    char buffer[10];
    fgets(buffer, sizeof(buffer), stdin);
    printf("%s", buffer);
}
