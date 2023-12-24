/*
Ijaz Ahmad ---------------------------------- i191873
Project-------------------------------------- Phase 1
*/

#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>
#include<unistd.h>

// fix max number of threads
#define MAX_THREADS 27
#define SUDOKU_SIZE 9
#define ROW_COL_SIZE 9

// to count the total wrong 
int countWithinRange = 0;
bool statusWithinRange = false;
int countWithoutRange = 0;
bool statusWithoutRange = false;

// as given in the pdf
typedef struct 
{
    /* data */
    int rows;
    int column;
} parameters;

// create a golobal "valid" 
int valid[MAX_THREADS] = {0}; // initilize with zero


/*
Now we create a board for the game.
*/

// this is our sukodu board...
int sudoku[SUDOKU_SIZE][SUDOKU_SIZE] = {

    {6, 2, 4, 5, 3, 9, 1, 8, 7},
    {5, 1, 9, 7, 2, 8, 6, 3, 4},
    {8, 3, 7, 6, 1, 4, 2, 9, 5},
    {1, 4, 3, 8, 6, 5, 7, 2, 9},
    {9, 5, 8, 2, 4, 7, 3, 6, 1},
    {7, 6, 2, 3, 9, 1, 4, 5, 8},
    {3, 7, 1, 9, 5, 6, 8, 4, 2},
    {4, 9, 6, 1, 8, 2, 5, 7, 3},
    {2, 8, 5, 4, 7, 3, 9, 1, 6}

};
/*
As given in the pdf
1. A thread to check that each column contains the digits 1
through 9
2. A thread to check that each row contains the digits 1
through 9
3. Nine threads to check that each of the 3 × 3 sub grids contains the digits 1
through 9 columns
*/

void* isColValid(void*);
void* isRowValid(void*);
void* isSquare3x3Valid(void*);


int main(){
    pthread_t tids[MAX_THREADS];
    int index = 0;
    
    // as suoku is 9x9
    int r , c;
    r = c = 9;

    // iterations
    for (int i = 0; i < r; i++){
        for(int j = 0; j<c; j++){
            // columns check
            if ( i == 0){
                parameters *colData = (parameters*)malloc(sizeof(parameters));
                colData->column = j;
                colData->rows = i;
                pthread_create(&tids[index++], NULL, isColValid, colData);
            }
            // row check
            if(j == 0){
                parameters *rowData = (parameters*)malloc(sizeof(parameters));
                rowData->column = j;
                rowData->rows = i;
                pthread_create(&tids[index++], NULL, isRowValid, rowData);
            }
            // check the squares
            if ( i % 3 == 0 && j%3 == 0){
                parameters* sqData = (parameters*)malloc(sizeof(parameters));
                sqData->rows = i;
                sqData->column = j;
                pthread_create(&tids[index++], NULL, isSquare3x3Valid, sqData);
            }
        }
    }


    // synchronize & join
    for (int i = 0; i < MAX_THREADS; i++){
        pthread_join(tids[i], NULL);
    }

    for (int i = 0; i < MAX_THREADS; i++){
        if(valid[i] == 0){
            printf("Sudoku solution is invalid!\n");
            pthread_exit(NULL);
        }
    }
    printf("Sudoku solution is valid!\n");
    pthread_exit(NULL);
}

/*
=========================================================
====================Thread Functions ====================
=========================================================
*/

void* isSquare3x3Valid(void* arg){
    parameters *ptr = (parameters*)arg;
    int row = ptr->rows;
    int col = ptr->column;

    // check the square validations
    if(row > 6 || col > 6 || row%3 != 0 || col%3 != 0){
        printf("In 3x3 Square: Inavlid rows and columns as their values are row = %d, and col = %d\n", row+1, col+1);
        // as these values are wrong 
        // thread should return
        pthread_exit(NULL);
    }

    int validityArray[ROW_COL_SIZE] = {0};
    for(int i = row; i < (row+3); i++){
        for (int j = col; j < (col+3); j++){
            int valid_Number = sudoku[i][j];

            if(valid_Number < 1 || valid_Number > 9){
                printf("In 3x3 Square: This number (%d) at row (%d) and col (%d) not in the range 1-9\n", valid_Number, i+1, j+1);
                pthread_exit(NULL); 
            }else if(validityArray[valid_Number - 1] == 1){
                printf("In 3x3 Square: This number (%d) at row (%d) and col (%d) is found more than once\n", valid_Number, i+1, j+1);
                pthread_exit(NULL); 
            }else{
                validityArray[valid_Number - 1] = 1;
            }
        }
    }
    valid[row + (col/3)] = 1;
    pthread_exit(NULL);
}

void* isRowValid(void* arg){
    parameters *ptr = (parameters*)arg;
    int r = ptr->rows;
    int c = ptr->column;

    if(c != 0 || r > 8){
        printf("In Row-Validation: Inavlid rows and columns as their values are row = %d, and col = %d\n", r+1, c+1);
        // as these values are wrong 
        // thread should return
        pthread_exit(NULL);
    }
    int validityArray[ROW_COL_SIZE] = {0};
    for(int i = 0; i < ROW_COL_SIZE; i++){
        int valid_Number = sudoku[r][i];
        if (valid_Number < 1 || valid_Number > 9){
            printf("In Row-Validation row = %d: This number (%d) at index (%d) not in the range 1-9\n", r+1, valid_Number, i+1);
            pthread_exit(NULL);   
        }else if(validityArray[valid_Number - 1] == 1){
            printf("In Row-Validation row = %d: This number (%d) at index (%d) is found more than once\n", r+1, valid_Number, i+1);
            pthread_exit(NULL); 
        }else{
            validityArray[valid_Number - 1] = 1;
        }
    }
    valid[r + 9] = 1;
    pthread_exit(NULL);
}


void* isColValid(void* arg){
    parameters* ptr = (parameters*)arg;
    int r = ptr->rows;
    int c = ptr->column;

    // first check the values of r and c
    if(r != 0 || c > 8){
        printf("In Col-Validation: Inavlid rows or columns as their values are row = %d, and col = %d\n", r+1, c+1);
        // as these values are wrong 
        // thread should return
        pthread_exit(NULL);
    } 
    int validityArray[ROW_COL_SIZE] = {0};
    for (int i = 0; i < ROW_COL_SIZE; i++){
        // here we check the numbers 1 to 9
        int valid_Number = sudoku[i][c];
        if (valid_Number < 1 || valid_Number > 9){
            printf("In Col-Validation col = %d: This number (%d) at index (%d) not in the range 1-9\n", c+1,valid_Number, i+1);
            pthread_exit(NULL);
        }else if(validityArray[valid_Number - 1] == 1){
            printf("In Col-Validation col = %d: This number (%d) at index (%d) is found more than once\n", c+1,valid_Number, i+1);
            pthread_exit(NULL); 
        }else{
            validityArray[valid_Number - 1] = 1; // this number is valid 
        }
    }
    valid[18 + c] = 1;
    pthread_exit(NULL);
}
