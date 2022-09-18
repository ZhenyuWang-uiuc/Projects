/**
 * shell
 * CS 241 - Spring 2022
 */

/*
 * Two things can be optimized (TODO!!! Maybe later?)
 *      1. chdir can put in change_dir function rather than in the child process handler
 *         benefit: I don't need to pass 'path' to the data_redirection_processing
 *      2. In process_status, I can use fscanf to read each process's information rather than use sstring and vector
 *         benefit: can reduce the number of runtime instructions (I assume) because I think fscanf can run faster
 *                  than the user defined types, i.e., sstring and vector
*/


#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <dirent.h>

#include "format.h"
#include "shell.h"
#include "vector.h"
#include "sstring.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define INT_MAX_BITS 11
#define SIZE_T_MAX_BITS 20
#define TIME_IN_24H_FORMAT 5

/*
 * check_opt_args_validity: It checks whether users properly use -h and -f
 *      hfd: history file descriptor
 *      sfd: script file descriptor
 *      argc: argument counter
 *      argv: arguments
 *      return value: 0 represents success, 1 represents failure
*/
int check_opt_args_validity(int argc, char *argv[], FILE **hfd, FILE **sfd)
{
    int isFile = 0;
    int isHistory = 0;

    for (int i = 1; i < argc; i++) {
        if (isHistory) {
            *hfd = fopen(argv[i], "a+");
            isHistory = 0;
        } else if (isFile) {
            *sfd = fopen(argv[i], "r");
            isFile = 0;
        } else if (strcmp(argv[i], "-h") == 0) {
            isHistory = 1;
        } else if (strcmp(argv[i], "-f") == 0) {
            isFile = 1;
        }
    }

    if (isHistory || isFile) print_usage();
    if (!*sfd) print_script_file_error();
    if (!*hfd) print_history_file_error();
    return isHistory || isFile || !*sfd || !*hfd;
}

/*
 * close_files: It will close the history file descriptor and script file descriptor
 *      hfd: history file descriptor
 *      sfd: script file descriptor
*/
void close_files(FILE *hfd, FILE *sfd)
{
    if (hfd != (void*)-1 && hfd != NULL) {
        fclose(hfd);
    }
    if (sfd != (void*)-1 && sfd != NULL) {
        fclose(sfd);
    }
}

/*
 * change_dir: It will change the current directory.
 *      path: the current directory
 *      argument: the target directory, the argument has the following pattern "../../abc/def"
 *      exit_status: if it successfully changes the directory, set exit_status to 0; 
 *                  otherwise, set exit_status to -1.
*/
void change_dir(char *path, char *argument, int *exit_status)
{
    if (argument == NULL) {
        memset(path, 0, PATH_MAX);
        path[0] = '/';
        *exit_status = 0;
        return;
    }

    bool fullPath = *argument == '/';
    char buffer[PATH_MAX];
    DIR *dir;

    if (fullPath) {
        dir = opendir(argument);
    } else {
        sstring *s = cstr_to_sstring(argument);
        vector *args = sstring_split(s, '/');
        strcpy(buffer, path);
        size_t size = vector_size(args);
        for (size_t i = 0; i < size; i++) {
            char *arg = vector_get(args, i);
            if (strcmp(arg, "..") == 0 && strlen(path) != 1) { // move to the parent dir
                for (size_t i = strlen(path) - 1; i >= 1; i--) {
                    if (path[i] == '/') {
                        path[i] = 0;
                        break;
                    }
                    path[i] = 0;
                }
            } else if (strcmp(arg, "..") == 0) {
                // nop - have this case just for looking more clearly
            } else if (strcmp(arg, ".") && strlen(arg) != 0) { // add child dir
                if (path[strlen(path) - 1] == '/') {
                    strcat(path, arg);
                } else {
                    strcat(path, "/");
                    strcat(path, arg);
                }
            }
        }
        dir = opendir(path);
        vector_destroy(args);
        sstring_destroy(s);
    }
    if (dir && fullPath) {
        memset(path, 0, PATH_MAX);
        strcpy(path, argument);
        closedir(dir);
        *exit_status = 0;
    } else if (dir) {
        closedir(dir);
        *exit_status = 0;
    } else {
        if (!fullPath) {
            memset(path, 0, PATH_MAX);
            strcpy(path, buffer);
        }
        *exit_status = -1;
        print_no_directory(argument);
    }
}

/*
 * update_command_vector: !<prefix> and #<index>'s helper function
 *      cmd: the command vector
 *      str: one of the history records
 *      status: the flag variable indicates which mode I will choose (-1 for nop, 0 for re, 1 for logical operators)
 *      return value: If the input string contains logical operators, return this string for later processing.
 *                    If the input string contains redirection commands, return this string for later processing.
 *                    If the input string doesn't contain logical operators, return NULL
*/
char *update_command_vector(vector *cmd, char *str)
{
    // print the command
    print_command(str);

    // if the input command contains logical operators or redirection commands, let other function processes it
    if (strstr(str, " &&") || strstr(str, " ||") || strstr(str, ";") ||
        strstr(str, " >") || strstr(str, " <") || strstr(str, " >>")) {
        return str;
    }

    sstring *s = cstr_to_sstring(str);
    vector *tmp = sstring_split(s, ' ');
    vector_clear(cmd);
    VECTOR_FOR_EACH(tmp, arg, { vector_push_back(cmd, arg); });
    sstring_destroy(s);
    vector_destroy(tmp);
    return NULL;
}

/*
 * signal_commands_helper: The implementation of the following three signals: 'kill', 'stop', 'cont'
 *      start: the iterator which points to the head of the cmd vector
 *      end: the iterator which points to the end of the cmd vector
 *      signum: the signal that user wants to pass
 *      exit_status: If the program successfully sneds the signal to the target process, set to 0. Otherwise, exit_status sets to -1
 *      return value: if the program succeeds sends the signal, return this process's id, otherwise, return 0
*/
int signal_commands_helper(void **start, void **end, int signum, int *exit_status)
{
    if (start + 1 == end) {
        print_invalid_command(*start);
        *exit_status = -1;
        return -1;
    }
    pid_t id = atoi(*++start);
    int ret = kill(id, signum);
    *exit_status = ret;
    if (ret == -1) { print_no_process_found(id); return -1; }
    return id;
}

/*
 * get_background_command: use id as a key to find the corresponding command
 *      id: one of the background processes's id
 *      processes: a vector contains each background processes
 *      bg_cmds: a vector contains corresponding commands
 *      return value: the corresponding command
*/
char *get_background_command(pid_t id, vector *processes, vector *bg_cmds)
{
    size_t vs = vector_size(processes);
    for (size_t i = 0; i < vs; i++) {
        if (*(int*)vector_get(processes, i) == id) {
            return vector_get(bg_cmds, i);
        }
    }
    return NULL;
}

/*
 * process_status: the implementation of ps
 *      id: one of the background process's id
 *      cmd one of the command is running at the background
 *      return value: 0 indicates the program successfully prints out the process's info
 *                    1 indicates the program fails to print out the process's info
*/
int process_status(pid_t id, char *cmd)
{
    // convert id to c string
    char id_in_str[INT_MAX_BITS];
    memset(id_in_str, 0, INT_MAX_BITS);
    sprintf(id_in_str, "%d", id);

    // convert state path to c string
    sstring *s = cstr_to_sstring("/proc/$$/stat");
    sstring_substitute(s, 0, "$$", id_in_str);
    char *state_path = sstring_to_cstr(s);
    sstring_destroy(s);

    FILE *f1 = fopen(state_path, "r");
    FILE *f2 = fopen("/proc/stat", "r");
    if (f1 == NULL) { fclose(f2); free(state_path); return 1; }

    char *buffer = NULL;
    size_t buffer_size = 0;

    long unsigned t_execution;       // in clock ticks
    long long unsigned t_start = 0;  // in clock ticks
    long long unsigned btime = 0;
    time_t total_time = 0;

    long unsigned sec;
    long unsigned min;

    char start_time[TIME_IN_24H_FORMAT + 1];
    char execution_time[SIZE_T_MAX_BITS * 2 + 2];
    process_info info;

    // update info
    info.pid = id;
    info.command = cmd;

    while (getline(&buffer, &buffer_size, f1) != EOF) {
        buffer[strlen(buffer) - 1] = 0;
        sstring *s = cstr_to_sstring(buffer);
        vector *v = sstring_split(s, ' ');
        info.state = *(char*)vector_get(v, 2);
        info.nthreads = strtol(vector_get(v, 19), NULL, 10);
        info.vsize = strtol(vector_get(v, 22), NULL, 10) / 1024;
        t_execution = strtol(vector_get(v, 13), NULL, 10);
        t_execution += strtol(vector_get(v, 14), NULL, 10);
        t_start = strtoll(vector_get(v, 21), NULL, 10);
        sstring_destroy(s);
        vector_destroy(v);
    }

    while (getline(&buffer, &buffer_size, f2) != EOF) {
        buffer[strlen(buffer) - 1] = 0;
        sstring *s = cstr_to_sstring(buffer);
        vector *v = sstring_split(s, ' ');
        if (strcmp(vector_get(v, 0), "btime") == 0) {
            btime = strtoll(vector_get(v, 1), NULL, 10);
            sstring_destroy(s);
            vector_destroy(v);
            break;
        }
        sstring_destroy(s);
        vector_destroy(v);
    }

    // calculate time
    sec = (t_execution / sysconf(_SC_CLK_TCK)) % 60;
    min = ((t_execution / sysconf(_SC_CLK_TCK)) - sec) / 60;
    total_time = btime + t_start / sysconf(_SC_CLK_TCK);

    // convert time to the c string
    time_struct_to_string(start_time, TIME_IN_24H_FORMAT + 1, localtime(&total_time));
    execution_time_to_string(execution_time, SIZE_T_MAX_BITS * 2 + 2, min, sec);
    info.start_str = start_time;
    info.time_str = execution_time;

    // print info
    print_process_info(&info);

    // release any allocated memory
    free(buffer);
    free(state_path);
    fclose(f1);
    fclose(f2);
    return 0;
}

/*
 * builtin_commands: The implementation of built-in commands.
 * Builtin commands include: 'cd', '!history', '#<index>', '!<index>', 'kill <pid_t>', 'stop <pid_t>', 'cont <pid_t>', 'ps'
 *      cmd: the command vector contains the command name and its arguments
 *      history: the history vector stores any execution commands
 *      processes: the processes vector contain every background processes' id and command
 *      path: the current directory
 *      argument: a string contains the command name and its arguments
 *      tmp: double pointer points to a history record
 *      exit_status: see the change_dir for details
 *      return value: 1 represents for executing built in commands succesfully, 0 represents for executing built in commands failingly
 *                    If the return value is 0, the command is considered as external commands
*/
int builtin_commands(vector *cmd, vector *history, vector *processes, vector *bg_cmds,
    char *path, char *argument, char **tmp, int *exit_status)
{
    void **start = vector_begin(cmd);
    void **end = vector_end(cmd);

    // use iterator to check
    if (strcmp((char*) *start, "cd") == 0) {
        if (start + 1 == end) {
            change_dir(path, NULL, exit_status);
        } else {
            change_dir(path, *++start, exit_status);
        }
        return 1;
    } else if (strcmp((char*) *start, "kill") == 0) {
        pid_t id = signal_commands_helper(start, end, SIGKILL, exit_status);
        if (id != -1) print_killed_process(id, get_background_command(id, processes, bg_cmds));
        return 1;
    } else if (strcmp((char*) *start, "stop") == 0) {
        pid_t id = signal_commands_helper(start, end, SIGSTOP, exit_status);
        if (id != -1) print_stopped_process(id, get_background_command(id, processes, bg_cmds));
        return 1;
    } else if (strcmp((char*) *start, "cont") == 0) {
        pid_t id = signal_commands_helper(start, end, SIGCONT, exit_status);
        if (id != -1) print_continued_process(id, get_background_command(id, processes, bg_cmds));
        return 1;
    }

    // use string to check
    if (strcmp(argument, "!history") == 0) {
        size_t size = vector_size(history);
        for (size_t i = 0; i < size; i++)
            print_history_line(i, vector_get(history, i));
        if (size == 0)
            print_no_history_match();
        return 1;
    } else if (*argument == '#') {
        size_t index = atoi(argument + 1);
        if (*(argument + 1) == 0 || index >= vector_size(history)) {
            print_invalid_index();
            return 1;
        } else {
            *tmp = update_command_vector(cmd, vector_get(history, index));
        }
    } else if (*argument == '!') {
        char *s = NULL;
        if (*(argument + 1) == 0) {
            s = *vector_back(history);
        } else {
            size_t lenpre = strlen(argument + 1);
            for (int i = (int) vector_size(history) - 1; i >= 0; i--) {
                char *tmp = vector_get(history, i);
                size_t lenstr = strlen(tmp);
                if (lenstr >= lenpre && memcmp(argument + 1, tmp, lenpre) == 0) {
                    s = tmp;
                    break;
                }
            }
        }
        if (s == NULL){
            print_no_history_match();
            return 1;
        } else {
            *tmp = update_command_vector(cmd, s);
        }
    } else if (strcmp(argument, "ps") == 0) {
        print_process_info_header();
        // print the parent state's first
        process_status(getpid(), "./shell");
        size_t vs = vector_size(processes);
        for (size_t i = 0; i < vs; i++) {
            if (process_status(*(int*)vector_get(processes, i), vector_get(bg_cmds, i))) {
                vector_erase(processes, i);
                vector_erase(bg_cmds, i);
            }
        }
        return 1;
    }
    return 0;
}

/*
 * parse_logical_operators: It will track whether the input command has '&&', '||' or ';' operators.
 * If the input command has one of the logical operators mentioned above, extract the command from the left side of the operator,
 * and jump over that logical operator.
 *      status: a flag variable that uses to track which logical operator will execute
 *              -1 for nop, 0 for the end, 1 for 'and', 2 for 'or', 3 for 'separator'
 *      argument: double pointer points to input command.
 *                If there exists a logical operator, update the argument and let it points to the next command after this logical operator
 *      return value: the first command that argument points to
*/
char *parse_logical_operators(int *status, char **argument)
{
    char *and = strstr(*argument, " &&");
    char *or = strstr(*argument, " ||");
    char *sep = strstr(*argument, ";");
    char *logical_cmd;
    char *ptr = *argument;

    if (and == NULL && or == NULL && sep == NULL) {
        logical_cmd = NULL;
    } else {
        if (and == NULL) and = (void*) -1;
        if (or == NULL) or = (void*) -1;
        if (sep == NULL) sep = (void*) -1;
        logical_cmd = MIN(MIN(and, or), sep);
    }

    if (logical_cmd && logical_cmd == and) { *and = 0; *status = 1; *argument = and + 4; }
    else if (logical_cmd && logical_cmd == or) { *or = 0; *status = 2; *argument = or + 4; }
    else if (logical_cmd && logical_cmd == sep) { *sep = 0; *status = 3; *argument = sep + 2; }
    else if (*status != -1) { *status = 0; }
    else { *status = -1; }
    return ptr;
}

/*
 * parse_redirection_commands: It will track whether the input command has '>', '<' or '>>' commands.
 * If the input command has one of the redirection commands mentioned above, extract the command from the left side of the command,
 * and extract the file name from the right side of the command.
 *      status: a flag variable that uses to track which redirection command will execute
 *              -1 for nop, 1 for 'ouput redirection', 2 for 'input redirection', 3 for 'append redirection'
 *      argument: the input command
 *      return value: the file name
*/
char *parse_redirection_commands(int *status, char *argument)
{
    char *re_output = strstr(argument, " >");
    char *re_input = strstr(argument, " <");
    char *re_append = strstr(argument, " >>");

    if (re_append) { *re_append = 0; *status = 3; return re_append + 4; }
    else if (re_input) { *re_input = 0; *status = 2; return re_input + 3; }
    else if (re_output) { *re_output = 0; *status = 1; return re_output + 3; }
    else { *status = -1; return NULL; }
}

/*
 * data_redirection_processing: This function only will be called in the child process.
 *      status: the flag variable controls which redirection mode I will use
 *              1 for '>', 2 for '<', 3 for '>>'
 *      filename: the string that contains the file name
 *      path: the current directory
 *      command_name: the name of a specific command 
 *      return value: If the mode is '>' or '>>', return NULL
 *                    If the mode is '<', return a vector that contains the command name and its arguments
*/
FILE *data_redirection_processing(int *status, char *filename, char *path)
{
    FILE * f = NULL;
    char *path_w_filename = calloc(strlen(filename) + strlen(path) + 2, 1);

    // create the path
    strcat(path_w_filename, path);
    strcat(path_w_filename, "/");
    strcat(path_w_filename, filename);

    // output redirection
    if (*status != 2) {  // output and append
        if (*status == 1) f = fopen(path_w_filename, "w");  // output
        else if (*status == 3) f = fopen(path_w_filename, "a+");  // append

        // check the existence of the file
        if (f == NULL) { print_redirection_file_error(); free(path_w_filename); exit(1); }
        dup2(fileno(f), fileno(stdout));
        free(path_w_filename);
        return f;
    }

    // input redirection
    // open the input redirection file and add the command name to the vector
    f = fopen(path_w_filename, "r");

    // check the existence of the file
    if (f == NULL) { print_redirection_file_error(); free(path_w_filename); exit(1); }
    dup2(fileno(f), fileno(stdin));
    free(path_w_filename);
    return f;
}

/*
 * memory_releaser: The helper function will release any allocated memory. The helper function only call in the child process
 *      tmp_3, tmp_4, ..., bg_cmds are the addresses point to heap memory
 *      hfd, sfd, redirection_f are FILE pointers
*/
void memory_releaser(void *tmp_3, void *tmp_4, void *path, void *buffer, 
    void *s, void *cmd, void *history, void *processes, void *bg_cmds, void *hfd, void *sfd, void *redirection_f)
{
    // any allocated variables
    if (tmp_4) free(tmp_4); free(path); free(tmp_3); free(buffer);

    // sstring
    sstring_destroy(s);

    // vector
    vector_destroy(cmd); vector_destroy(history); vector_destroy(processes); vector_destroy(bg_cmds);

    // files
    close_files(hfd, sfd); if (redirection_f) fclose(redirection_f);
}

/*
 * the signal handler
*/
void handler(int signum)
{
    (void)signum;
}

int shell(int argc, char *argv[])
{
    // open files and check the validity
    FILE *hfd = (void *)-1, *sfd = (void *)-1;
    int status = check_opt_args_validity(argc, argv, &hfd, &sfd);
    if (status) exit(1);

    // set signal handler
    signal(SIGINT, handler);

    // variables list
    FILE *input = (sfd == (void *)-1) ? stdin : sfd;
    vector *history = string_vector_create();
    vector *processes = int_vector_create();
    vector *bg_cmds = string_vector_create();
    char *buffer = NULL;
    char *parsed_cmd = NULL;
    char *path = calloc(PATH_MAX, 1);
    size_t buffer_size = 0;
    int eof = 0;
    int exit_status;      // command exit status
    int l_operator = -1;  // -1 for nop, 0 for the end, 1 for and, 2 for or, 3 for sep
    int re_command = -1;  // -1 for nop, 1 for output redirection, 2 for input redirection, 3 for append
    int background = 0;   // flag variables indicates whether the process is a background process
    int current_process_id = getpid();
    int ret = 0;          // return value from the builtin_commands function

    // get the current directory
    realpath(".", path);

    // print the prompt
    print_prompt(path, current_process_id);

    //
    while (getline(&buffer, &buffer_size, input) != EOF) {
        // parse the current command and temporarily copy the command
        buffer[strlen(buffer) - 1] = 0;
        char *tmp = buffer;
        char *tmp_2 = NULL;  // points to a logical operator command
        char *tmp_3 = strdup(buffer);
        char *tmp_4 = NULL;  // copy of tmp_2 (on the heap)
        char *filename;

        // check whether the next process is a background process
        background = (strlen(buffer) == 0) ? 0 : buffer[strlen(buffer) - 1] == '&';

        // if the next executing process is a background process, remove '&'
        if (background) {
            buffer[strlen(buffer) - 1] = 0;
            buffer[strlen(buffer) - 1] = 0;
        }

        // parse the next command
        NEXT_CMD:
        parsed_cmd = parse_logical_operators(&l_operator, &tmp);
        filename = parse_redirection_commands(&re_command, parsed_cmd);
        sstring *s = cstr_to_sstring(parsed_cmd);
        vector *cmd = sstring_split(s, ' ');

        // regularly check to see if children need reaping
        if (vector_size(processes)) {
            pid_t bg_id = waitpid(-1, NULL, WNOHANG);

            // if return id is -1, clear the processes vector and the bg_cmds vector
            if (bg_id == -1) {
                vector_clear(processes);
                vector_clear(bg_cmds);
            }

            // find that process id and erase it from the processes vector
            if (bg_id != 0 && bg_id == -1) {
                size_t vs = vector_size(processes);
                for (size_t i = 0; i < vs; i++) {
                    if (*(int*)vector_get(processes, i) == bg_id) {
                        vector_erase(processes, i);
                        vector_erase(bg_cmds, i);
                        break;
                    }
                }
            }
        }

        // if input is not stdin, print command to the terminal
        if (input != stdin) print_command(buffer);

        // if it is the exit command 
        if (strcmp(parsed_cmd, "exit") == 0) {
            // release memory
            free(tmp_3);
            sstring_destroy(s);
            vector_destroy(cmd);

            // check whether some background processes are still running, killing them
            size_t vs = vector_size(processes);
            for (size_t i = 0; i < vs; i++) {
                kill(*(int*)vector_get(processes, i), SIGKILL);
            }
            break;
        }

        // built-in commands
        BUILT_IN_CMD:
        ret = builtin_commands(cmd, history, processes, bg_cmds, path, parsed_cmd, &tmp_2 ,&exit_status);

        if (tmp_2) {  // if '#<n>' and !<prefix> execute a command with logical operators
            // make two copies of tmp_2: one for history and another for future processing
            tmp_4 = strdup(tmp_2);
            tmp_2 = NULL;
            tmp = tmp_4;
            tmp_3 = realloc(tmp_3, strlen(tmp_4) + 1);
            strcpy(tmp_3, tmp_4);

            // release memory
            sstring_destroy(s);
            vector_destroy(cmd);
            goto NEXT_CMD;
        } else if (!tmp_2 && !ret) {  // if '#<n>' and !<prefix> execute a built-in command
            char *next = (char*)*vector_front(cmd);
            if (strcmp(next, "cd") == 0 || strcmp(next, "kill") == 0 || strcmp(next, "stop") == 0 ||
                strcmp(next, "cont") == 0 || strcmp(next, "ps") == 0)
                goto BUILT_IN_CMD;
        }

        if (!ret && strlen(parsed_cmd)) {
            // external commands
            pid_t id = fork();
            if (id == -1) {
                print_fork_failed();
            } else if (id == 0) {
                FILE *redirection_f = NULL;
                print_command_executed(getpid());

                // setpgid
                if (background) {
                    if (setpgid(0, 0)) print_setpgid_failed();
                }

                // input or output redirection
                if (re_command != -1) redirection_f = data_redirection_processing(&re_command, filename, path);

                // TODO: change PATH won't affect the output of ls, how to solve this?
                // SOLVED: use chdir()
                chdir(path);
                execvp(vector_get(cmd, 0), (char *const *) vector_at(cmd, 0));
                print_exec_failed(vector_get(cmd, 0));

                // release memory and exit
                memory_releaser(tmp_3, tmp_4, path, buffer, s, cmd, history, processes, bg_cmds, hfd, sfd, redirection_f);
                exit(1);
            } else {
                if (background) {
                    if (!waitpid(id, &exit_status, WNOHANG)){
                        vector_push_back(processes, &id);
                        vector_push_back(bg_cmds, tmp_3);
                    }
                } else {
                    waitpid(id, &exit_status, 0);
                }
            }
        }

        // add the current command to history
        if (l_operator != -1) {
            if (l_operator > 0 && tmp_3) vector_push_back(history, tmp_3);
            if (!l_operator) l_operator = -1;
        } else if (!(strcmp(parsed_cmd, "!history") == 0 || *parsed_cmd == '!' || *parsed_cmd == '#') && strlen(parsed_cmd)) {
            vector_push_back(history, tmp_3);
        } else if ((*parsed_cmd == '!' && strcmp(parsed_cmd, "!history") != 0 && *(char*)*vector_front(cmd) != '!') ||
            (*parsed_cmd == '#' && *(char*)*vector_front(cmd) != '#')) {
            size_t size = vector_size(cmd);
            sstring *_cmd = cstr_to_sstring(*vector_front(cmd));
            sstring *space = cstr_to_sstring(" ");
            for (size_t i = 1; i < size; i++) {
                sstring *arg = cstr_to_sstring(vector_get(cmd, i));
                sstring_append(_cmd, space);
                sstring_append(_cmd, arg);
                sstring_destroy(arg);
            }
            char *char_cmd = sstring_to_cstr(_cmd);
            vector_push_back(history, char_cmd);
            free(char_cmd);
            sstring_destroy(_cmd);
            sstring_destroy(space);
        }
        
        // release memory and ready to execute the next command
        if (tmp_3) { free(tmp_3); tmp_3 = NULL; }  // I need this condition because I may use goto later
        sstring_destroy(s);
        vector_destroy(cmd);

        // (logical operators) special case: check the exit status to decide whether I need to execute the next command
        if ((WEXITSTATUS(exit_status) == 0 && l_operator == 1) || (WEXITSTATUS(exit_status) != 0 && l_operator == 2) ||
            l_operator == 3) {
            goto NEXT_CMD;
        } else if ((WEXITSTATUS(exit_status) != 0 && l_operator == 1)) {
            for (;;) {
                parse_logical_operators(&l_operator, &tmp);
                if (l_operator == 0) break;
                if (l_operator != 1) goto NEXT_CMD;
            }
        } else if ((WEXITSTATUS(exit_status) == 0 && l_operator == 2)) {
            for (;;) {
                parse_logical_operators(&l_operator, &tmp);
                if (l_operator == 0) break;
                if (l_operator != 2) goto NEXT_CMD;
            }
        }

        // use fseek to detect whether the next one is EOF (need a better solution)
        if (input != stdin) {
            eof = getline(&buffer, &buffer_size, input);
            if (eof != -1)
                fseek(input, -strlen(buffer), SEEK_CUR);
        }

        // print the prompt;
        if (eof != -1) print_prompt(path, current_process_id);

        // special case: tmp_4 is the copy of logical operator command
        if (tmp_4) free(tmp_4);
    }

    if (hfd != NULL && hfd != (void*)-1) {
        size_t size = vector_size(history);
        for (size_t i = 0; i < size; i++) {
            fprintf(hfd, "%s\n", vector_get(history, i));
        }
    }

    // if receive EOF (ctrl+D)
    size_t vs = vector_size(processes);
    for (size_t i = 0; i < vs; i++) {
        kill(*(int*)vector_get(processes, i), SIGKILL);
    }

    // prevent creating any zombies, i.e., releasing any status memory
    while (waitpid(-1, NULL, WNOHANG) != -1);

    // releasing any allocated memory and closing any open files
    free(path);
    free(buffer);
    vector_destroy(history);
    vector_destroy(processes);
    vector_destroy(bg_cmds);
    close_files(hfd, sfd);
    return 0;
}
