#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

namespace{
    vector<vector<string>> tokens;
}

int main()
{   
    const int matrix_rows = 10; // matrix size
    const int matrix_cols = 10; // matrix size
    string arr[matrix_rows][matrix_cols];
    
    int ncols = 0; // total count inputted
    int nrows = 0; // total count inputted

    ifstream input("input.txt");
    string line;
   
    while (getline(input, line)){

        std::string buf;			// Have a buffer string
        std::stringstream ss(line); // Insert the string into a stream

        while (ss >> buf){
            arr[nrows][ncols] = buf;
            ncols ++;
            if(buf == "0" || buf == "cnf") {nrows++; ncols = 0;}
        }
    }

    cout << endl;
    for( int i=0; i < nrows; i++){
        
        for(int j=0; j < matrix_rows; j++){
            if(arr[i][j] == "")j++;
            else cout << arr[i][j] << " ";
            }
            cout<<endl;
        }

    return 0;
}