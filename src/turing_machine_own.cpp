#include <list>
#include <vector>
#include <string>
#include <cmath>
#include <bitset>
#include <iostream>
#include <chrono>
#include <random>
#include <time.h>
#include <algorithm>
#include <map>

int const max_1s[] = {1, 4, 6, 13, 4098};
int const NUM_STATES = 2;

// Function to convert integer to character (a, b, c, ...)
char intToChar(int value) {
    return static_cast<char>('a' + value);
}

int createStateTable(){

    bool hasH = false;

    // Create a vector of vectors of strings
    std::vector<std::vector<std::string>> stateTableTuringMachine;

    // Initialize the vector with keys 'a0', 'a1', 'b0', 'b1', ..., and empty strings as values
    for (char key = 'a'; key < 'a' + NUM_STATES; ++key) {
        std::vector<std::string> stateVector;

        for (int zero_one = 0; zero_one < 2; ++zero_one) {
            std::random_device rd;
            std::mt19937 gen(rd());

            // random 0 or 1 for overwriting the current value
            int randomNumber = std::uniform_int_distribution<>(0, 1)(gen);

            // random l or r for moving left or right
            char randomDirection = std::uniform_int_distribution<>(0, 1)(gen) == 0 ? 'l' : 'r';

            // random state to go to with a halt state
            int randomState = std::uniform_int_distribution<>(0, NUM_STATES)(gen);
            char randomStateChar = intToChar(randomState);

            // convert number to character state
            if (randomState == NUM_STATES) {
                randomStateChar = 'h';
                hasH = true;
            }

            stateVector.push_back(std::string(1, key) + std::to_string(zero_one) + std::to_string(randomNumber) + randomDirection + randomStateChar);
        }

        stateTableTuringMachine.push_back(stateVector);
    }

    // Print the initialized vector of vectors of strings
	std::cout << "Initialized vector of vectors of strings: " << std::endl;
    for (const auto& vector : stateTableTuringMachine) {
        for (const auto& str : vector) {
            std::cout << str << std::endl;
        }
    }
    

	// If 'h' is not present, replace a random pair's last value with 'h'
	if (!hasH) {
		std::random_device rd;
		std::mt19937 gen(rd());

		// Generate a random index
		std::uniform_int_distribution<> dist(0, stateTableTuringMachine.size() - 1);
		int randomIndex = dist(gen);

		// Access the random vector
		std::vector<std::string>& randomVector = stateTableTuringMachine[randomIndex];

		// Replace the last value with 'h'
		if (!randomVector.empty()) {
			randomVector.back().back() = 'h';
		}

        // Print the updated stateTableTuringMachine
        std::cout << "Updated stateTableTuringMachine: " << std::endl;
        for (const auto& vector : stateTableTuringMachine) {
            for (const auto& str : vector) {
                std::cout << str << ' ';
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
	}

    return 1;
}



// create a Class Turing Machine
class TuringMachine {

    private:

        char tape [30] = "00000000000000000000000000000";
        std::map<std::string, std::string> statesTable;


    public: 

        // Constructor
        TuringMachine() {
            // Initialize your Turing Machine here
        }



};


int main() {
    // Create a Turing Machine
    TuringMachine tm;

    int statesTable = createStateTable();


    return 0;
}