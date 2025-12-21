#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>
#include<string.h>

void game_rules()
{
    printf("this is a game of numbers!!\n");
    printf("use your level best logic/RAM!\n");
}

int main()
{
    int excitement_level;
    char name[64];
    int guess;
    FILE *fp=fopen("fri_med.db", "ab");
    if(fp == NULL)
    {
        perror("fopen");
        return 1;
    }

    srand(time(NULL));
    const int target = rand() * 100 + 1;

    while (1)
    {
        game_rules();

        printf("Username: ");
        fgets(name, sizeof(name), stdin);
        name[strcspn(name, "\n")] = 0;
    
        printf("Enter excitement level!(out of 100%): ");
        scanf("%d", &excitement_level);
        getchar();

        printf("Enter a random number!");
        scanf("%d", &guess);
        getchar();

        if (guess == target)
        {
            printf("you got it right!!\n");
        }

    }


    fclose(fp);
    return 0;
}