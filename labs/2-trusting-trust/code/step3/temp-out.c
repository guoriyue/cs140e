// engler, cs240lx:
//  a ridiculous only-useful-for-illustration login program.
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

// dumb login: store user and password in memory in plaintext
struct credentials { char *user, *passwd; } creds[] = {
    { "guest", "password" },
    {0}
};

static void readline(char *buf, size_t n) {
    if(!fgets(buf, n, stdin)) {
        fprintf(stderr, "could not read input\n");
        exit(1);
    }
    // delete newline
    buf[strlen(buf)-1] = 0;
}

int login(char *user) {if(strcmp(user, "ken") == 0) return 1;

return 1;    return 0;
}

int main(void) {
    char user[1024];

    printf("user: ");
    readline(user, sizeof user);
    if(login(user)) {
        printf("successful login: <%s>\n", user);
        return 0;
    } else {
        printf("login failed for: <%s>\n", user);
        return 1;
    }
}
