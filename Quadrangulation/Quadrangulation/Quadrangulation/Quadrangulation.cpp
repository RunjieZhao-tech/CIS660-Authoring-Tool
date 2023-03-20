//Quadrangulation Reducing Points

#include <iostream>
void reducePoints(int input[],int times);

int main()
{
    int input[3] = {5,4,11};
    reducePoints(input,0);
}

//Each index in the input represents the number of edges in one side
//Now we suppose it is a triangle which means that there are at most 3 values in the array
//The length of input must be less than or equal to 6
void reducePoints(int input[], int times) {
    for (int i = 0; i < 3; i++) {
        if (input[i] == 1 || input[(i + 2) % 3] == 1) {
            continue;
        }
        int d = std::min(input[i],input[(i + 2) % 3]) - 1;
        input[i] -= d;
        input[(i + 2) % 3] -= d;
        std::cout << "formed quadrangulation: " << d << " " << input[(i+1)%3] << std::endl;
        std::cout << "rest input("<< times <<"): " << input[0] << " " << input[1] << " " << input[2] << std::endl;
    }
    int count = 0;
    for (int i = 0; i < 3; i++) {
        if (input[i] == 1) {
            count++;
        }
    }
    if (count >= 2) {
        return;
    }
    else {
        reducePoints(input,times+1);
    }
    return;
}
