// DONE BY MOHAMMAD AMMOURAH , YAZEED ABUKHALIL AND MOHAMMAD SWAIELM
#include <iostream>
#include <bits/stdc++.h>
#include <pthread.h>
using namespace std;
pthread_mutex_t Lock;
pthread_mutex_t Pixels;
pthread_mutex_t brightLock;
pthread_mutex_t darkLock;
pthread_mutex_t normalLock;
 
int numOfBright, numOfDark, numOfNormal;
int **Oimage, **Fimage;
struct Th_Range
{
	int SizeOfImage;
	int StartRow;
	int EndRow;
	int ThreadId;
	bool flag = 0;
};
//   NOTE IF UNSAFE VERSION IS NEEDED ONLY COMMENT/REMOVE THE LOCKS
 
void *Median(void *par)
{
	struct Th_Range *Num = (struct Th_Range *)par;
	int StartRow = Num->StartRow;
	bool flag = Num->flag;
	int EndRow = Num->EndRow;
	int Size = Num->SizeOfImage;
	int Neighbour[9] = {0};
	int dark = 0, bright = 0, normal = 0;
 
	pthread_mutex_lock(&Lock); // HERE I HAD TO PUT LOCKS OR USE PRINTF(C lang) TO PROTECT FROM INTERRUPTING BY TERMINAL THREAD
	cout << "ThreadID= " << Num->ThreadId << ", startRow= " << StartRow << ", endRow= " << EndRow  << endl;
	pthread_mutex_unlock(&Lock);
 
	if (EndRow == StartRow)
		EndRow += 2;
	else
		EndRow++;
	for (int i = StartRow + 1; i < EndRow; i++)
	{
		for (int j = 1; j < Size - 1; j++)
		{
			Neighbour[0] = Oimage[i - 1][j - 1];
			Neighbour[1] = Oimage[i - 1][j];
			Neighbour[2] = Oimage[i - 1][j + 1];
			Neighbour[3] = Oimage[i][j - 1];
			Neighbour[4] = Oimage[i][j];
			Neighbour[5] = Oimage[i][j + 1];
			Neighbour[6] = Oimage[i + 1][j - 1];
			Neighbour[7] = Oimage[i + 1][j];
			Neighbour[8] = Oimage[i + 1][j + 1];
			sort(Neighbour, Neighbour + 9);
			Fimage[i][j] = Neighbour[4];
			if (Neighbour[4] > 200)               // NOW THE FOLLOWING LOCKS ARE USED TO PROTECT THE SHARED VARIABLES BETWEEN THE THREADS                           //
			{                                    // THE SHARED VARIABLES HERE ARE THE GLOBAL VARIABLES numOfBright,numOfDark AND numOfNormal                        //
				bright++;						 // THE Oimage AND Fimage ARE ALSO SHARED BUT NO RACE CONDITION WILL OCCUR BECAUSE EACH THREAD HAS IT'S OWN i and j //
				pthread_mutex_lock(&brightLock); // IF UNSAFE VERSION IS NEEDED ONLY PUT A COMMENT HERE AND AT LINE 61
				numOfBright++;
				pthread_mutex_unlock(&brightLock);
			}
			else if (Neighbour[4] < 50)
			{
				dark++;
				pthread_mutex_lock(&darkLock); // IF UNSAFE VERSION IS NEEDED ONLY PUT A COMMENT HERE AND AT LINE 68
				numOfDark++;
				pthread_mutex_unlock(&darkLock);
			}
			else
			{
				normal++;
				pthread_mutex_lock(&normalLock); // IF UNSAFE VERSION IS NEEDED ONLY PUT A COMMENT HERE AND AT LINE 75
				numOfNormal++;
				pthread_mutex_unlock(&normalLock);
			}
		}
	}
	pthread_mutex_lock(&Lock); // HERE I HAD TO PUT LOCKS OR USE PRINTF(C lang) TO PROTECT FROM INTERRUPTING BY TERMINAL THREAD
	cout << "ThreadID " << Num->ThreadId << " numOfBright " << bright << " numOfDark " << dark << " numOfNormal " << normal << endl;
	pthread_mutex_unlock(&Lock); 
	pthread_exit(NULL);
}
int main(int argc, char *argv[])
{
	int num_threads, n, r, c;
	numOfNormal = numOfDark = numOfBright = 0;
	if (argc >= 2)
		num_threads = atoi(argv[1]);
	else
		exit(-1);
	ifstream cin("input.txt");
	cin >> n;
	r = c = n + 2;    // THE ORIGINAL MATRIX NOW HAS 2 ROWS AND 2 COLUMNS IN ADDITION TO ITS ORIGINAL SIZE WE WILL INITIALIZE THOSE ROWS AND COLUMNS TO 0'S
 
	Oimage = new int *[r]();
	Fimage = new int *[r]();
 
	for (int i = 0; i < c; i++) // INITIALIZING THE 2D MATRICES WITH ZERO'S
	{
		Oimage[i] = new int[c]();
		Fimage[i] = new int[c]();
	}
	for (int i = 1; i < r - 1; i++) // THE ORIGINAL MATRIX IS BOUNDED BY 0'S SO WE WILL START THE INPUT FROM R = 1 AND C = 1 TO R-1 AND C-1 
		for (int j = 1; j < c - 1; j++)
			cin >> Oimage[i][j];
	// Creating Threads
	pthread_t *threads = new pthread_t[num_threads];
	if (n == num_threads)
	{ 
		for (int i = 0; i < num_threads; i++)
		{
			struct Th_Range *RANGE = (struct Th_Range *)malloc(sizeof(struct Th_Range));
			RANGE->SizeOfImage = r;
			RANGE->ThreadId = i;
			RANGE->StartRow = i;
			RANGE->EndRow = i + 1; 
			pthread_create(&threads[i], NULL, Median, RANGE);
		}
		for (int i = 0; i < num_threads; i++)
			pthread_join(threads[i], NULL);
	}
	else
	{
		if (n % num_threads == 0)
		{
			int remaining = n / num_threads, save_end[num_threads];
			save_end[0] = 0;
			for (int i = 0; i < num_threads; i++)
			{
				struct Th_Range *RANGE = (struct Th_Range *)malloc(sizeof(struct Th_Range));
				RANGE->SizeOfImage = r;
				RANGE->ThreadId = i;
				if (i == 0)
					RANGE->StartRow = 0;
				else
					RANGE->StartRow = save_end[i - 1];
				save_end[i] = RANGE->EndRow = RANGE->StartRow + remaining;
				pthread_create(&threads[i], NULL, Median, RANGE);
			}
			for (int i = 0; i < num_threads; i++)
				pthread_join(threads[i], NULL);
		}
		else if (num_threads > n)
		{
			int i;
			for (i = 0; i < n; i++)
			{
				struct Th_Range *RANGE = (struct Th_Range *)malloc(sizeof(struct Th_Range));
				RANGE->SizeOfImage = r;
				RANGE->StartRow = i;
				RANGE->EndRow = i+1;
				RANGE->ThreadId = i;
				RANGE->flag = 1;
				pthread_create(&threads[i], NULL, Median, RANGE);
			}
			for (int i = 0; i < num_threads; i++)
				pthread_join(threads[i], NULL);
			cout << " Percentage of unneeded threads = " << 100 * (float(num_threads - n) / num_threads) << "%\n";
		}
		else
		{									 // num_threads < n && n%num_threads !=0
			int stripSize = n / num_threads; 
			int remainder = n % num_threads; 
			int save_end = 0;
			for (int i = 0; i < num_threads; i++)
			{
				struct Th_Range *RANGE = (struct Th_Range *)malloc(sizeof(struct Th_Range));
				RANGE->ThreadId = i;
				RANGE->SizeOfImage = r;
				RANGE->StartRow = save_end;
				if (remainder > 0)
				{
					RANGE->EndRow = RANGE->StartRow + stripSize + 1;  
					save_end = RANGE->EndRow;						 
					remainder--;
				}
				else
				{
					RANGE->EndRow = RANGE->StartRow + stripSize;
					save_end = RANGE->EndRow;
				}
				pthread_create(&threads[i], NULL, Median, RANGE);
			}
			for (int i = 0; i < num_threads; i++)
				pthread_join(threads[i], NULL);
		}
	}
	cout << "\n Main: numOfBright " << numOfBright << " numOfDark " << numOfDark << " numOfNormal " << numOfNormal << endl;
	ofstream cout("output.txt");
	for (int i = 1; i < r - 1; i++)
	{
		for (int j = 1; j < c - 1; j++)
			cout << Fimage[i][j] << " ";
		cout << endl;
	}
	pthread_exit(NULL);
	return 0;
}
