#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

namespace
{
vector<string> tokens;
}

typedef struct
{
    string value;
    bool visited;
} cell;

int main()
{

    ifstream input("input.txt");
    string line;
    int Num_variables;
    int Num_clauses;
    int x;
    vector<string> list;

    while (getline(input, line))
    {
        std::string buf;            // Have a buffer string
        std::stringstream ss(line); // Insert the string into a stream

        while (ss >> buf)
        {

            if (buf == "p")
            {
                continue;
            }

            if (buf == "cnf")
            {
                ss >> buf;
                Num_variables = std::stoi(buf);
                cout << Num_variables << endl;
                ss >> buf;
                Num_clauses = std::stoi(buf);
                cout << Num_clauses << endl
                     << endl;
                break;
            }
            list.push_back(buf);
        }
        // while (ss >> buf)
        // {
        //     cout << x << endl;
        // }
    }
    cell matrix[Num_clauses][(Num_variables * 2) + 1];

    int row = 0; // keeps track of l
    int col = 0;
    for (int i = 0; i < list.size(); i++)
    {
        cout << list.at(i) << " ";
        string val = list.at(i);
        cell temp = {val, false};
        matrix[row][col] = temp;
        if (list.at(i) == "0")
        {
            cout << endl;
            row++;
        }
        col++;
    }

    return 0;
}

// const int matrix_rows = 10; // matrix size
// const int matrix_cols = 10; // matrix size
// string arr[matrix_rows][matrix_cols];

// int ncols = 0; // total count inputted
// int nrows = 0; // total count inputted

// cout << endl;
// for (int i = 0; i < nrows; i++)
// {

//     for (int j = 0; j < matrix_rows; j++)
//     {
//         if (arr[i][j] == "")
//             j++;
//         else
//             cout << arr[i][j] << " ";
//     }
//     cout << endl;
// }
