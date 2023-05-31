#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "bloomfilter.h"
#include "model_to_score_an_email_file.h"
#define ll long long
#define MAX_DOMAIN_SIZE 100

int main(int argc, char const *argv[])
{
    // +++++++++++++++++++++++++++++EMAIL ID ANALYZER+++++++++++++++++++++++++++
    printf("\n----------ANALYZING THE SENDER'S EMAIL ID:---------------\n");
    matrixbf *mbf = NULL;
    mbf = trainfilter();
    FILE *emails;
    emails = fopen(argv[1], "r");
    if(!emails){
        perror("Couldn't find file in current repo...");
        return 1;
    }
    char mailID[MAX_DOMAIN_SIZE];
    fgets(mailID,MAX_DOMAIN_SIZE,emails);
    if(evaluate(mailID, mbf)){
        printf("The senders domain: %s seems suspicious.\n",mailID);
    }
    else{
        printf("The senders domain: %s seems ok.\n",mailID);
    }
    // +++++++++++++++++++++++++++++EMAIL CONTENTS ANALYZER+++++++++++++++++++++++++++
    printf("\n----------ANALYZING THE EMAIL CONTENTS:---------------\n");
    TrieMap *tm = NULL;
    tm = initTrieMap(tm);
    int corpus_size = 0;
    // Open the file in read mode
    FILE *file;
    file = fopen("word_to_freq.csv", "r");
    if (file == NULL)
    {
        printf("Unable to open the file.\n");
        return 0;
    }
    char ch;
    // Count the number of lines
    while ((ch = fgetc(file)) != EOF)
    {
        if (ch == '\n')
            corpus_size++;
    }
    // printf("Corpus Size:%d\n",corpus_size);
    file = fopen("word_to_freq.csv", "r");
    char line[50];
    char *word;
    int freq;
    while (fgets(line, 50, file))
    {
        word = strtok(line, ",");
        freq = atoi(strtok(NULL, ","));
        double weightage = (freq / (double)corpus_size);
        insertTrieMap(tm, word, weightage);
    }
    printf("successfully loaded trie\n");
    char email[1000000];
    int i = 1;
    while (fgets(email, 1000000, emails))
    {
        double score = 0.0;
        long long size = 1ll;
        char *token = strtok(email, " ");
        while (token)
        {
            if (token != " \n")
            {
                score += searchTrieMap(tm, token);
            }
            token = strtok(NULL, " \n");
            size++;
        }
        printf("Email score: %f\n", (score / size));
        if ((score / size) < QUARTILE_1)
        {
            printf("Email may not be spam...\n");
        }
        else if ((score / size) >= QUARTILE_1 && (score / size) <= QUARTILE_3)
        {
            printf("Email has moderate chances of being spam...\n");
        }
        else{
            printf("Email is mostly spam...\n");
        }
        
    }

    return 0;
}

