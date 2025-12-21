#include <stdio.h>
#include <string.h>

int main()
{
    const int target = 10;
    int guess;
    char name[30];
    char line[256];
    char input[50];
    FILE *fp;

    fp = fopen("gamenight.txt", "a+");
    if (fp == NULL)
    {
        printf("File could not be opened!\n");
        return 1;
    }

    printf("Welcome to Game Night!\n");
    printf("Guess the secret number between 1 and 100.\n");

    printf("Enter your name: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = 0; // remove newline

    while (1)
    {
        printf("Enter your guess (or type 'quit' to exit): ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0; // remove newline

        if (strcmp(input, "quit") == 0)
        {
            printf("You quit the game.\n");
            break;
        }

        // Convert to number
        if (sscanf(input, "%d", &guess) != 1)
        {
            printf("Invalid input. Please enter a number.\n");
            continue;
        }

        if (guess == target)
        {
            printf("You got it, %s! 🎉 Enjoy your win!\n", name);
            fprintf(fp, "Name: %s  Number Guessed: %d\n", name, guess);
            printf("Data saved successfully!\n");
            break;
        }
        else
        {
            printf("Wrong guess! Try again.\n");
        }
    }

    fclose(fp);

    printf("\n--- Game Results So Far ---\n");
    fp = fopen("gamenight.txt", "r");
    if (fp != NULL)
    {
        while (fgets(line, sizeof(line), fp) != NULL)
        {
            printf("%s", line);
        }
        fclose(fp);
    }
    else
    {
        printf("No previous game data found.\n");
    }

    printf("\nGame ended. Thanks for playing!\n");
    return 0;
}
