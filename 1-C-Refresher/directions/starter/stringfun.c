/*
 * stringfun.c
 * 
 * This program performs various string manipulations based on the command-line arguments provided.
 * It supports the following functionalities:
 * - Print usage information
 * - Count words in a string
 * - Reverse a string
 * - Print words and their lengths
 * - Search and replace words in a string
 * 
 * Usage: ./stringfun [-h|c|r|w|x] "string" [other args]
 * 
 * Options:
 * -h: Print usage information
 * -c: Count words in the provided string
 * -r: Reverse the provided string
 * -w: Print words and their lengths in the provided string
 * -x: Search and replace words in the provided string
 * 
 * Author: Ali Ural
 * Date: 01/19/2025
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define BUFFER_SZ 50

// prototypes
void usage(char *);
void print_buff(char *, int);
int setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int count_words(char *, int, int);
void reverse_string(char *, int);
void print_words_and_lengths(char *, int);

// Function to process the user string into the buffer
int setup_buff(char *buff, char *user_str, int len) {
    //TODO: #4:  Implement the setup buff as per the directions
    // *buff is the buffer to store the processed string
    // *user_str is the user input string
    // len is the size of the buffer

    // Validate input arguments
    if (!buff || !user_str || len <= 0) {
        return -2;
    }

    const char *pstr = user_str;
    char *pbuff = buff;
    int processed_len = 0;
    int consecutive_spaces = 0;

    // Skip leading spaces and tabs
    while (*pstr == ' ' || *pstr == '\t') {
        pstr++;
    }

    // Find the last non-space character
    const char *end = pstr + strlen(pstr) - 1;
    while (end > pstr && (*end == ' ' || *end == '\t')) {
        end--;
    }

    // Process the trimmed user string
    while (pstr <= end) {
        if (processed_len >= len) {
            return -1;
        }

        if (*pstr == ' ' || *pstr == '\t') {
            if (!consecutive_spaces) {
                *pbuff = ' ';
                pbuff++;
                processed_len++;
                consecutive_spaces = 1;
            }
        } else {
            *pbuff = *pstr;
            pbuff++;
            processed_len++;
            consecutive_spaces = 0;
        }

        pstr++;
    }


    return processed_len;
}

// Add dots after reversal or word processing
void add_padding_dots(char *buff, int str_len, int len) {
    while (str_len < len) {
        buff[str_len++] = '.';
    }
}

// Function to count words in the buffer
int count_words(char *buff, int len, int str_len) {
    if (!buff || len <= 0 || str_len <= 0) {
        return -2;
    }

    int word_count = 0;
    int in_word = 0;

    for (int i = 0; i < str_len; i++) {
        if (*(buff + i) != ' ') { 
            if (!in_word) {       
                word_count++;
                in_word = 1;
            }
        } else {
            in_word = 0; 
        }
    }

    return word_count;
}

// Function to reverse the string in the buffer
void reverse_string(char *buff, int str_len) {
    char *start = buff;                
    char *end = buff + str_len - 1;    
    char temp;

    // Reverse only the portion of the buffer containing the string
    while (start < end) {
        temp = *start;
        *start = *end;
        *end = temp;

        start++;
        end--;
    }
}

// Function to print words and their lengths
void print_words_and_lengths(char *buff, int str_len) {
    int word_index = 1;  
    int word_len = 0;

    printf("Word Print\n----------\n");

    for (int i = 0; i <= str_len; i++) {
        if (i == str_len || *(buff + i) == ' ') {
            if (word_len > 0) {
                // Print the word and its length
                printf("%d. %.*s(%d)\n", word_index, word_len, buff + i - word_len, word_len);
                word_index++;  
                word_len = 0;  
            }
        } else {
            word_len++; 
        }
    }

    // Print the number of words returned
    printf("\nNumber of words returned: %d\n", word_index - 1);
}

int search_and_replace(char *buff, int buff_size, const char *search, const char *replace) {
    if (!buff || !search || !replace || buff_size <= 0) {
        return -2;
    }

    char *found = strstr(buff, search);
    if (!found) {
        return -3;
    }

    int search_len = strlen(search);
    int replace_len = strlen(replace);
    int buff_len = strlen(buff);

    // Check if replacement would cause buffer overflow
    if ((buff_len - search_len + replace_len) > buff_size) {
        // Truncate and replace as much as possible
        int max_copy_len = buff_size - (found - buff);
        strncpy(found, replace, max_copy_len);
        found[max_copy_len] = '\0'; 
        return -1; 
    }

    // Shift buffer contents to accommodate replacement
    if (replace_len != search_len) {
        memmove(found + replace_len, found + search_len, buff_len - (found - buff) - search_len + 1);
    }

    // Replace the search string with the replacement
    memcpy(found, replace, replace_len);

    return 0;
}

// Print the buffer
void print_buff(char *buff, int len) {
    printf("Buffer:  [%.*s]\n", len, buff);
}

// Print usage instructions
void usage(char *exename) {
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]", exename);
}


int main(int argc, char *argv[]) {
    char *buff;
    char *input_string;
    char opt;
    int rc;
    int user_str_len;

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    // Because the argc < 2 condition ensures that there are at least 
    // two arguments passed to the program: the program name (argv[0]) 
    // and the command option (argv[1]). If argv[1] doesn’t exist, it 
    // means the user didn’t provide enough arguments, so the program prints 
    // the usage instructions and exits with an error code of 1. By checking 
    // argc first, we avoid accessing an out of bounds memory location for argv[1], 
    // which could cause a segmentation fault.
    if ((argc < 2) || (*argv[1] != '-')) {
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1] + 1);

    if (opt == 'h') {
        usage(argv[0]);
        exit(0);
    }

    //TODO:  #2 Document the purpose of the if statement below
    // This if statement checks if the user has provided a string 
    // to process as the second argument (argv[2]). If the program 
    // is invoked with only the option flag (like -c or -r) but no 
    // string to process, the program doesn’t have the required input 
    // to perform its operations. In this case, it prints the usage 
    // instructions and exits with an error code of 1. This prevents 
    // the program from proceeding with invalid or incomplete input, 
    // ensuring that subsequent operations have the necessary data to 
    // work with.
    if (argc < 3) {
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2];

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    buff = malloc(BUFFER_SZ);
    if (!buff) {
        printf("Error: Memory allocation failed.\n");
        exit(99);
    }
    
    // Process the user string
    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);
    if (user_str_len < 0) {
        if (user_str_len == -1) {
            printf("Error: Input string is too long.\n");
        } else {
            printf("Error setting up buffer, error = %d\n", user_str_len);
        }
        free(buff);
        exit(3);
    }

    //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
    //       the case statement options
    // Process the user string based on the option
    switch (opt) {
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);
            if (rc < 0) {
                printf("Error counting words, rc = %d\n", rc);
                free(buff);
                exit(3);
            }
            printf("Word Count: %d\n", rc);
            add_padding_dots(buff, user_str_len, BUFFER_SZ);
            break;

        case 'r':
            reverse_string(buff, user_str_len);
            add_padding_dots(buff, user_str_len, BUFFER_SZ);
            break;

        case 'w':
            print_words_and_lengths(buff, user_str_len);
            add_padding_dots(buff, user_str_len, BUFFER_SZ);
            break;
        
        case 'x':
            if (argc < 5) {
                printf("Error: Insufficient arguments for -x flag.\n");
                free(buff);
                exit(1);
            }

            rc = search_and_replace(buff, BUFFER_SZ, argv[3], argv[4]);
            if (rc == -3) {
                printf("Error: Search string not found.\n");
                free(buff);
                exit(3);
            } else if (rc == -1) {
                printf("Replacement caused truncation.\n");
            } else if (rc < 0) {
                printf("Error replacing string, rc = %d\n", rc);
                free(buff);
                exit(3);
            }
            break;

        default:
            usage(argv[0]);
            free(buff);
            exit(1);
    }

    print_buff(buff, BUFFER_SZ);
    free(buff);
    exit(0);
}