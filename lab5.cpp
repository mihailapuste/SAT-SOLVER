#include <cstring>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stack>
#include <vector>
#include <omp.h>
#include <pthread.h>

using namespace std;

namespace {
    int n_cols = 0;
    int gloabl_backtrack = 0;
    pthread_mutex_t pthread_lock; // for testing
    pthread_mutex_t backtrack_lock; 
}

typedef struct{ // struct used to populate the matrix
    int value; // contains the value of the cell
    int cellrow; // the row of on which the cell resides
    bool visited; // boolean visited flag
} cell;

typedef struct{ // struct used in solution array
    int value;
    int row;
} sltn;


typedef struct { 
    vector<int> list;   
    int root_id;         
    int n_rows;
    int n_cols;
    int n_variables;
} thread_args; 


bool siblingExists(std::vector<sltn> &solutions, int cell){ // makes all inputs positive, then compares them
                if( cell < 0) cell = (-1)*cell; // if input is neg, make pos
                for(int i = 0; i < solutions.size(); i++){ // check cell against solutions
                    int x = solutions[i].value; 
                    if( x < 0) x = (-1)*x; // make sol pos if neg
                    if( cell == x ) return true; // if sol and cell match, return true
                }
                return false; //else false
}

void printSolution(std::vector<sltn> &solutions){ // Prints each element in solution array
                cout << "solutions : < ";
                for(int i = 0; i < solutions.size(); i++){
                    cout << solutions[i].value << " ";
                }
                cout << ">\n";

}

bool checkNumberValidity(int input, int n_variables){ // check to see if input number in exists in solution (+ or -)
     if( input < 0) input = (-1)*input;
     if( input > n_variables){
         return false; // number does not exist in solution
     }else{
         return true; // number exists in solution (+ or -)
     }
}



void*sat_solver(void * args){
    thread_args sat_args = (*(thread_args *) args); //file
    vector<int> list2 = sat_args.list;
    int n_rows = sat_args.n_rows;
    int n_cols = sat_args.n_cols;
    int n_variables = sat_args.n_variables;
    vector<sltn> solution_set;
    int n_backtracks = 0;
    bool solution_found = false;
    bool validation_run = false; // bool for final pass through

    n_cols = ((n_variables * 2) + 1);
    cell matrix[n_rows][n_cols];
    int row = 0;
    int col = 0;

    for (int i = 0; i < list2.size(); i++){
        // cout << list2.at(i) << " ";
        int val = list2.at(i);
        cell temp = {val, row, false};
        matrix[row][col] = temp;
        if (list2.at(i) == 0){
            // cout << endl;
            row++;
            col = 0;
        }
        else{
            col++;
        }
    }
   
    validation_run: // final run through after solution was thought to be found. Validation run will complete the solution if necessary, or check it one final time.

    for(int i = 0; i < n_rows; i++){ // rows

        for(int j = 0; j < n_cols; j++){ // cols
            if(i == 0){
                j = j + sat_args.root_id;
            }
        //   cout << solution_set.size() << " ";
               
                for(int k = 0; k < solution_set.size(); k++){ //check to see if cell exists in solution
                    if(matrix[i][j].value == solution_set[k].value){ // if cell exists in solution
                        if(i == (n_rows-1)){ // and is at last row, solution found -> exit
                            if(validation_run == true) {// potential solution found
                                // cout << "Found < " << matrix[i][j].value << " > in solution\n-- No more rows to search\n-- ";
                                // printSolution(solution_set);
                                // cout << endl << "___ Validation complete ___" << endl;
                                solution_found = true;
                                goto done_solving;
                            }
                            else{
                                validation_run = true;
                                // cout << endl << "___ Validation run ___" << endl;
                                goto validation_run;
                            }
                        }
                        else{ // otherwise, look to next row
                            // cout << "Found < " << matrix[i][j].value << " > in solution\n-- Moving to row: " << i+2 << "\n-- ";
                            // printSolution(solution_set);
                            goto next_row;
                        }
                    }
                }

            if(solution_set.size() < n_variables
                && matrix[i][j].visited == false 
                && matrix[i][j].value != 0 
                && !siblingExists(solution_set, matrix[i][j].value)){ // if cell is not root, doesn't exist in solution, and there is room left in solution, and cell is unvisited, place it in solution
                // cout << "Add solution\n-- push( "<< matrix[i][j].value <<" )\n-- Moving to row: " << i+2 << "\n-- ";
                solution_set.push_back({ matrix[i][j].value, matrix[i][j].cellrow });
                matrix[i][j].visited = true;
                // printSolution(solution_set);
                if(checkNumberValidity(matrix[i][j].value, n_variables) == false){
                    // cout << "\n\nERROR: Invalid variable read!\n";
                    // cout << "!!! < " << matrix[i][j].value <<" > IS INVALID !!!\n\n";
                    goto done_solving;
                }
                if(i == (n_rows-1)){
                    if(validation_run == true) {// potential solution found
                            // cout << endl << "___ Validation complete ___" << endl;
                            solution_found = true;
                            goto done_solving;
                        }
                    else{
                            validation_run = true;
                            // cout << endl << "___ Validation run ___" << endl;
                            goto validation_run;
                    }
                }
                else{
                    goto next_row;
                }
            }

            if(matrix[i][j].value == 0){
                for (int x = 0; x < n_cols; x++){
                    matrix[i][x].visited = false;
                }
                
                if((i-1) < 0){
                    // cout << "no solution found";
                    solution_found = false;
                    goto done_solving;
                }
                else{
                    // cout << "backtrack\n-- pop( " << solution_set.back().value << " )\n-- Moving to row: " << i << "\n-- ";
                    if(solution_set.size() > 0){
                        solution_set.pop_back();  
                    }
                    // printSolution(solution_set);
                    i = i - 2; // decrement 1 row up (for loops inc +1 every time, so (2-1 = 1))
                    pthread_mutex_lock(&backtrack_lock);
                        gloabl_backtrack ++;
                    pthread_mutex_unlock(&backtrack_lock);
                    goto next_row;
                }
            }

        } //cols
        // cout << endl; // nextline every row
        next_row: {}

    } // rows

    done_solving: {}

    pthread_mutex_lock(&pthread_lock);
    // gloabl_backtrack = gloabl_backtrack + n_backtracks;

    if (solution_found == true) {
        cout <<"\n\n=== Solution EXISTS ===\nThread "<< sat_args.root_id << " terminated with :\n";
        printSolution(solution_set);
        cout << "backtracks: " << gloabl_backtrack << "\n\n\n";
        exit(0);
        // pthread_mutex_unlock(&pthread_lock);
        }
     pthread_mutex_unlock(&pthread_lock);
    
     pthread_exit(NULL);
    // cout << "\n\nSAT SOLVER LAB5 - DONE\n\n";
}

int main()
{
    cout << "\n\nSAT SOLVER LAB5 - START\n\n";

    // Deque * solution_set = new Deque();
    vector<sltn> solution_set;
    vector<int> list;
    ifstream input("input.txt");
    string line;
    bool solution_found = false;
    bool validation_run = false; // bool for final pass through
    int n_variables = 0;
    int n_backtracks = 0;
    int n_rows = 0;
    // int n_cols = 0;

    while (getline(input, line)){
        std::string buf;            // Have a buffer string
        std::stringstream ss(line); // Insert the string into a stream

        while (ss >> buf){
            if (buf == "p"){
                continue;
            }
            if (buf == "cnf"){
                ss >> buf;
                n_variables = std::stoi(buf);
                ss >> buf;
                n_rows = std::stoi(buf);
                break;
            }
            int f = std::stoi(buf);
            list.push_back(f);
        }
    }


int num_root = 0;
for (int k = 0; k<list.size(); k++){
    if (list.at(k)==0){
        break;
    }
    num_root++;
}

thread_args arg_arr[num_root]; 

for(int i = 0; i < num_root; i++){
    arg_arr[i] = {list, i, n_rows, n_cols, n_variables};
}

pthread_t arra[num_root];

for (int i = 0; i < num_root; i++) {
    pthread_create(&arra[i], NULL, sat_solver,(void *) &(arg_arr[i]));
}

for (int i = 0; i < num_root; i++){
    pthread_join(arra[i], NULL);
}


cout << "\n\n=== NO SOLUTION ===\nTerminated with:\nTotal # of backtracks: " << gloabl_backtrack << "\n\n";



return 0;
}