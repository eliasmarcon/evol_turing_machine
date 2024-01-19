#!/bin/bash

if [ "$#" -eq 3 ]; then
    
    # set number of states
    num_states=$1

    # set population size
    population_size=$2

    # set number of generations
    num_generations=$3

    # check if number of states is valid (between 1 and 4)
    if [ "$num_states" -lt 1 ] || [ "$num_states" -gt 4 ]; then
        echo "The number of states has to be between 1 and 4"
        exit 1
    fi

    # check if population size is valid
    if [ "$population_size" -lt 10 ]; then
        echo "The population size has to be greater than 10"
        exit 1
    fi

    # check if number of generations is valid
    if [ "$num_generations" -lt 10 ]; then
        echo "The number of generations has to be greater than 10"
        exit 1
    fi

else
    echo "Usage: ./run_busy_beaver.sh <num_states> <population_size> <num_generations>"
    exit 1
fi

output_file="./busy_beaver_output.txt"
cpp_file="./src/main.cpp"
output_executable="./out/busy_beaver"

# Clear the output file
> "$output_file"

# Compile the C++ program
g++ -o "$output_executable" "$cpp_file" -lga -fpermissive -Ofast


# # Check if compilation was successful
# if [ $? -eq 0 ]; then
#     echo "Compilation of Busy Beaver Problem was successful. Running the program..."

#     # Run the compiled program and append the output to the file
#     "$output_executable" "$num_states" "$population_size" "$num_generations" >> "$output_file"
            
#     echo "Results saved into $output_file."
# else
#     echo "Compilation failed. Please check for errors in your C++ code."
# fi


# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation of Busy Beaver Problem was successful. Running the program..."

    for num_states in {3..4}; do
        
        # Loop through population size from 100 to 1000 in steps of 100
        for population_size in {10..1000..200}; do
            # Clear the output file
            > "$output_file"

            # Loop through num_generations from 250 to 2000 in steps of 250
            for num_generations in {10..1500..200}; do

                # Print the current configuration
                echo "Running Busy Beaver Problem with $num_states states, $population_size population size and $num_generations generations..."

                # Run the compiled program and append the output to the file
                "$output_executable" "$num_states" "$population_size" "$num_generations" >> "$output_file"

            done
        done
    done

    echo "Results saved into $output_file."
else
    echo "Compilation failed. Please check for errors in your C++ code."
fi