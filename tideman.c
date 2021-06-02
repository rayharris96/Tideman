#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

typedef struct
{
    int win;
    int loss;
}
score;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);
bool iscycle(int source, int loser);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    //Look through the candidates name and check for a match
    for (int i = 0; i < candidate_count; i++)
    {
        //Update ranks array to indicate candidate as rank preference
        if (strcmp(name, candidates[i]) == 0)
        {
            ranks[rank] = i;
            return true;
        }
    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            //Increment preferences score if the first index is lower than the next, indicating higher rank of i over j
            if (i < j)
            {
                preferences[ranks[i]][ranks[j]]++;
            }
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            //Look through all the prefences index and only increment pair count if there is a winner loser pair
            if (preferences[i][j] > preferences[j][i])
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
                pair_count++;
            }
        }
    }
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    //Creating an array for strength of win
    int strength[pair_count];
    for (int i = 0; i < pair_count; i++)
    {
        strength[i] = 0;
    }

    //Populating strength data into array
    for (int i = 0; i < pair_count; i++)
    {
        strength[i] = preferences[pairs[i].winner][pairs[i].loser] - preferences[pairs[i].loser][pairs[i].winner];
    }

    //Creating temporary array of size ONE
    pair temp_pair[1];

    //Sorting strength and pair array using bubble sort
    int swap_counter = -1;
    int n = pair_count;
    while (swap_counter != 0)
    {
        swap_counter = 0;
        for (int i = 0; i < n - 1; i++)
        {
            if (strength[i] < strength[i + 1])
            {
                int temp = strength[i + 1];
                strength[i + 1] = strength[i];
                strength[i] = temp;

                temp_pair[0] = pairs[i + 1];
                pairs[i + 1] = pairs[i];
                pairs[i] = temp_pair[0];

                swap_counter++;
            }
        }
        n--;
    }
    return;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    for (int i = 0; i < pair_count; i++)
    {
        if (!iscycle(pairs[i].winner, pairs[i].loser))
        {
            locked[pairs[i].winner][pairs[i].loser] = true;
        }
    }
    return;
}

// Print the winner of the election
void print_winner(void)
{
    score scores[pair_count];
    for (int i = 0; i < pair_count; i++)
    {
        scores[i].win = 0;
        scores[i].loss = 0;
    }

    for (int i = 0; i < pair_count; i++)
    {
        for (int j = 0; j < pair_count; j++)
        {
            if (locked[i][j] == true)
            {
                scores[i].win++;
                scores[j].loss++;
            }
        }
    }

    for (int i = 0; i < pair_count; i++)
    {
        if (scores[i].win >= 1 && scores[i].loss == 0)
        {
            printf("%s\n", candidates[i]);
        }
    }
    return;
}

bool iscycle(int source, int loser)
{
    if (source == loser)
    {
        return true;
    }

    for (int i = 0; i < candidate_count; i++)
    {
        if (locked[loser][i] == true)
        {
            if (iscycle(source, i) == true)
            {
                return true;
            }
        }
    }
    //If no more losers of loser
    return false;
}
