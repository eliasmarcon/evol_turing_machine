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

// if DEBUG is defined then no debug is done
//#define DEBUG
#define PRINT_TAPE

using binary = unsigned char;

int const SIZE_B = sizeof(binary) * 8;
int const SIZE_M = 4;
int const max_1s[] = {1, 4, 6, 13, 4098};


char to_char(char write, char left, char next_state);
std::vector<char> create_states(int num_states);
char do_mutation(char mutate, char mutation_bit);


class Node {
public:
	Node* left = nullptr;
	Node* right = nullptr;
	binary data = 0;

	Node(Node* left, binary data, Node* right) {
		this->left = left;
		this->right = right;
		this->data = data;
	}
};


/*
The tape is a double linked list with. The Nodes hold a single value of type "binary".
for exmaple: binary = char. Every bit of this "binary" data type represents a value 
on the tape. So with binary = char each node represents 8 values on the tape.
Nodes are added to the left and right, as needed.
*/
class Tape {
public:
	Node* start = new Node(nullptr, 0, nullptr);
	Node* end = start;
	int length = 0;


	void print_tape() {
		Node* n = start;
		std::cout << get_bitset(n->data) << "|";
		while (n != end) {
			n = n->right;
			std::cout << get_bitset(n->data) << "|";
		}
		std::cout << std::endl;
	}


private:
	std::bitset<SIZE_B> get_bitset(binary b) {
		std::bitset<SIZE_B> x(b);
		return x;
	}

public:
	void add_right() {
		end->right = new Node(end, 0, nullptr);
		end = end->right;
		length++;
	}

	void add_left() {
		start->left = new Node(nullptr, 0, start);
		start = start->left;
		length++;
	}

	void reset() {
		Node* start = new Node(nullptr, 0, nullptr);
		Node* end = start;
		length = 0;
	}
};


/*
The head class holds a pointer to a tape and a pointer to a node on the tape,
which represents the node where the head is currently located in the tape.
It also has a position member which represents the position of the head in
the binary datatype, that is saved in the node. So the member "position" equals
the position in the binary type in bits or the number of bits you needed to 
skip in the current node to get to the bit where the head is currently located.
*/
class Head {
public:
	char position = 0;
	Node* current;
	Tape* tape;

	Head(Tape* tape) {
		this->tape = tape;
		current = tape->start;
	}

	void write(char binary) {
		if (binary) {
			current->data |= 1 << (7-position);
		}
		else {
			current->data &= ~(1 << (7-position));
		}
	}

	char read() {
		return (current->data >> (7-position)) & 1;
	}

	void move_right() {
		position++;
		if (position > 7) {
			if (current == tape->end) {
				tape->add_right();
			}
			current = current->right;
			position = 0;
		}
	}

	void move_left() {
		if (position == 0) {
			if (current == tape->start) {
				tape->add_left();
			}
			current = current->left;
			position = 7;
		}
		else {
			position--;
		}
	}

	void reset() {
		char position = 0;
		Node* current;
		tape->reset();
		current = tape->start;
	}
};



class TuringMachine {
public:
	std::vector<char> states;
	int number_of_states;
	Tape* tape;
	Head* head;
	char init_state;
	char current_state;
	int count_1 = 0;
	int count_step = 0;

	char read;
	char write;
	char left;
	char next_state;
	
	TuringMachine(Tape* tape, Head* head) {
		this->head = head;
		this->tape = tape;
	}

	void reset() {
		current_state = init_state;
		head->reset();
		count_1 = 0;
		count_step = 0;
	}

	void set_states(std::vector<char> states, char init_state) {
		this->states = states;
		this->number_of_states = states.size() / 2;
		this->init_state = init_state;
	}

	void get_current_state() {
		read = head->read();
		write = (states[2 * current_state + read] >> 0) & 1;
		left = (states[2 * current_state + read] >> 1) & 1;
		next_state = states[2 * current_state + read] >> 2;
	}

	int run() {
		current_state = init_state;
#ifndef DEBUG
		std::cout << "enter run with current_state = " << current_state << "\n";
		std::cout << "enter run with count_1 = " << count_1 << "\n";
		std::cout << "enter run with number_of_states = " << number_of_states << "\n";
		std::cout << "enter run with max_1s = " << max_1s[number_of_states - 1] + 1 << "\n";
#endif
		while (current_state < number_of_states
			&& count_1 < 5 * max_1s[number_of_states - 1] + 1
			&& tape->length < 10 * max_1s[number_of_states - 1]) {

			get_current_state();
#ifndef DEBUG
			std::cout << "r w l c n h" << "\n";
			std::cout << (int)read << " ";
			std::cout << (int)write << " ";
			std::cout << (int)left << " ";
			std::cout << (int)current_state << " ";
			std::cout << (int)next_state << " ";
			std::cout << (int)head->position << std::endl;
			std::cout << "count_1 = " << count_1 << std::endl;
#endif
			if (read != write) {
				head->write(write);
				if (write == 1) {
					count_1++;
				}
				else {
					count_1--;
				}
			}

#ifndef PRINT_TAPE
			tape->print_tape();
#endif
			if (left) {
				head->move_left();
			}
			else {
				head->move_right();
			}
			current_state = next_state;
			count_step++;
		}
#ifndef DEBUG
		std::cout << "steps = " << count_step << "\n";
#endif
		if (count_1 > 5 * max_1s[number_of_states - 1] + 1
			&& tape->length > 10 * max_1s[number_of_states - 1]) {
			return -1;
			} 
		else {
			return count_1;
		}
	}
};


/*
"translates" write, left and next_state into a single char
first bit: write 0 or 1
second bit: go left or right
rest of the bits: number of next state
*/
char to_char(char write, char left, char next_state) {
	return write + 2 * left + 4 * next_state;
}

/*
Flips a bit in a char and returns the char with the flipped bit
*/
char do_mutation(char mutate, char mutation_bit) {
	mutate ^= (1 << mutation_bit);
	return mutate;
}

/*
A state is represented by two chars. The first one applies if a 1 is read
and the second one if a 0 is read. The first bit of the char decides if a
0 or a 1 gets written. The second bit of the char decides if you make a 
step to the left or to the right. All the rest of the bits represent the
next state.
*/
std::vector<char> create_states(int num_states, int seed) {
	std::vector<char> states;
	states.reserve(2*num_states);
	char write;
	char read;
	char next;
	for (int j = 0; j < 2 * num_states; ++j) {
		srand(seed);
		write = rand() % 1;
		srand(seed*2);
		read = rand() % 1;
		srand(seed*3);
		next = rand() % (num_states+1);

		states.emplace_back(to_char(write, read, next));
	}
	return states;
}



int main() {

	int size_population = 10;
	int num_states = 2;
	int num_generations = 10;
	int seed = 0;

	std::vector<std::vector<char>> population;
	population.reserve(size_population);

	std::vector<int> scores;
	scores.reserve(size_population);

	for (int i = 0; i < size_population; ++i) {
		population.emplace_back(create_states(num_states, time(0)*i));
	}

	for (int g = 0; g < num_generations; ++g) {

		std::vector<int> scores;
		scores.reserve(size_population);
		// evaluate fitness of every member of the population
		for (int i = 0; i < size_population; ++i) {
			Tape* tape = new Tape();
			Head* head = new Head(tape);
			char init_state = 0;
			TuringMachine machine(tape, head);

			machine.set_states(population[i], init_state);
			scores.emplace_back(machine.run());
		}

		// get the indices of that part of the population that gets mutated
		std::vector<int> mutation_candidates;
		mutation_candidates.reserve(size_population);
		std::vector<int> sorted_scores = scores;
		sorted_scores.reserve(size_population);
		std::sort(sorted_scores.begin(), sorted_scores.end());
		int threshold = sorted_scores[size_population * 80 / 100];
		for (int i = 0; i < size_population; ++i) {
			if (scores[i] <= threshold) {
				mutation_candidates.emplace_back(i);
			}
			if (mutation_candidates.size() > 0.9 * size_population) {
				break;
			}
		}

		// mutate the population
		for (int i = 0; i < mutation_candidates.size(); ++i) {
			srand(time(0));
			int rand_state = rand() % (num_states * 2);
			// rand_bit = 0 --> mutate write
			// rand_bit = 1 --> mutate left
			// rand_bit = 2 --> mutate next state
			int rand_bit = rand() % 3;
			if (rand_bit == 2 && num_states > 1) {
				rand_bit = 2 + rand() % (num_states / 2 + 1);
			}
			population[mutation_candidates[i]][rand_state] = do_mutation(population[mutation_candidates[i]][rand_state], rand_bit);
		}
	}
}
/*
	// busy beaver for 4 states
	std::vector<char> states_bb_4;
	states_bb_4.emplace_back(to_char(1, 0, 1));
	states_bb_4.emplace_back(to_char(1, 1, 1));
	states_bb_4.emplace_back(to_char(1, 1, 0));
	states_bb_4.emplace_back(to_char(0, 1, 2));
	states_bb_4.emplace_back(to_char(1, 0, 4));
	states_bb_4.emplace_back(to_char(1, 1, 3));
	states_bb_4.emplace_back(to_char(1, 0, 3));
	states_bb_4.emplace_back(to_char(0, 0, 0));

	// busy beaver candidate for 5 states
	std::vector<char> states_bb_5;
	states_bb_5.emplace_back(to_char(1, 1, 1));
	states_bb_5.emplace_back(to_char(1, 1, 0));
	states_bb_5.emplace_back(to_char(1, 0, 2));
	states_bb_5.emplace_back(to_char(1, 0, 1));
	states_bb_5.emplace_back(to_char(1, 1, 0));
	states_bb_5.emplace_back(to_char(1, 0, 3));
	states_bb_5.emplace_back(to_char(1, 1, 0));
	states_bb_5.emplace_back(to_char(1, 0, 4));
	states_bb_5.emplace_back(to_char(1, 0, 5));
	states_bb_5.emplace_back(to_char(0, 0, 2));

	// another busy beaver candidate for 5 states
	std::vector<char> states_bb_5_2;
	states_bb_5_2.emplace_back(to_char(1, 0, 1));
	states_bb_5_2.emplace_back(to_char(1, 1, 2));
	states_bb_5_2.emplace_back(to_char(1, 0, 2));
	states_bb_5_2.emplace_back(to_char(1, 0, 1));
	states_bb_5_2.emplace_back(to_char(1, 0, 3));
	states_bb_5_2.emplace_back(to_char(0, 1, 4));
	states_bb_5_2.emplace_back(to_char(1, 1, 0));
	states_bb_5_2.emplace_back(to_char(1, 1, 3));
	states_bb_5_2.emplace_back(to_char(1, 0, 5));
	states_bb_5_2.emplace_back(to_char(0, 1, 0));

	*/

/*
auto start = std::chrono::high_resolution_clock::now();
machine.run();

auto end = std::chrono::high_resolution_clock::now();
auto time = end - start;

std::chrono::duration<long double, std::milli> milli(time);
std::cout << "milliseconds = " << milli.count() << "\n";
*/
/*
std::chrono::duration<long double, std::micro> micro(time);
std::cout << "microseconds = " << micro.count() << "\n";
*/



