# MCMC Substitution Cipher Solver: C++ High-Performance Edition

This project is a C++ implementation of a substitution cipher solver that uses a Metropolis-Hastings Markov Chain Monte Carlo (MCMC) algorithm enhanced with Simulated Annealing. It is a high-performance rewrite of an original Python implementation, designed to leverage modern hardware and advanced optimization techniques for significantly faster and more accurate decoding.

The core of the algorithm models the problem of deciphering a text as a search for the most likely permutation map (the cipher key). It uses a training text (e.g., "War and Peace") to learn the statistical properties of the English language, specifically the frequency of individual characters and the transition probabilities between character pairs (bigrams). It then uses this statistical model to evaluate the "likelihood" of a decoded text, guiding the MCMC simulation toward the correct solution.

## From Python to C++: A High-Performance Rewrite

The primary motivation for porting the project from Python to C++ was to achieve a massive leap in performance. While the Python version is excellent for prototyping, C++ offers several key advantages that have been leveraged in this implementation:

* **Raw Speed:** As a compiled language, C++ provides low-level control and execution speeds that are orders of magnitude faster than an interpreted language like Python.
* **True Parallelism:** Native support for multithreading allows the program to run multiple MCMC chains simultaneously, making full use of modern multi-core CPUs.
* **Fine-Grained Optimization:** C++ allows for detailed, low-level optimizations, from data structure selection to compiler-level enhancements.

## Core Improvements & New Features

This C++ version is not just a direct translation; it incorporates significant algorithmic improvements, new features, and enhanced tooling.

### 1. Massive Performance Gains & Algorithmic Enhancements

The most important improvements were made to the core MCMC algorithm to increase both speed and the quality of the results.

* **Optimized Log-Likelihood Calculation:** While the original Python library *contained* a faster `compute_log_probability_by_counts` function, the main simulation loop used a slower approach that rescanned the entire text in every iteration. The C++ version uses the optimized "by counts" strategy by default for its main simulation.
    * **Before the simulation:** It processes the secret message *once* to create a **transition counts matrix** (a bigram count).
    * **During the simulation:** It calculates the score using these pre-computed counts. This changes the complexity of each step from being proportional to the length of the text to being constant, resulting in a dramatic speedup.

* **Simulated Annealing with a Temperature Parameter:** This C++ implementation introduces **Simulated Annealing**, a significant enhancement over the standard Metropolis-Hastings algorithm.
    * A `temperature` parameter is added to the acceptance probability calculation. It starts high and is gradually "cooled" or lowered over the course of the simulation.
    * **Effect:** At the beginning (high temperature), the algorithm is more likely to accept "bad" moves (permutations that decrease the log-probability). This allows it to explore the solution space more broadly and avoid getting stuck in local optima. As the temperature cools, the algorithm becomes more "greedy," converging on the best solution it has found. This leads to more robust and accurate final results.

* **Pre-computed Logarithms:** The `std::log` function can be computationally expensive. The logarithms of the frequency and transition matrices (from the training data) are calculated just once before the MCMC chains begin, avoiding redundant calculations.

### 2. True Parallelism with Multithreading

To maximize the use of modern hardware, the deciphering process was parallelized.

* **Concurrent MCMC Chains:** The C++ implementation uses `std::thread` to launch multiple independent MCMC chains at once. Each thread explores the solution space from a different random starting point.
* **Dynamic Thread Management:** The number of parallel chains is not hardcoded. It is automatically set to the number of available hardware cores on the machine (`std::thread::hardware_concurrency()`), ensuring optimal resource utilization.
* **Thread-Safe Results:** A `std::mutex` is used to ensure that when each thread completes, it can safely compare its result to the current best solution and update it without causing race conditions.

### 3. Enhanced Tooling and Usability

The project has been structured with robust, professional tooling to make it easier to build, run, and use.

* **CMake Build System:** The project uses CMake to manage the build process. This provides a reliable and cross-platform way to compile the executables and libraries. It is configured to use the `-O3` compiler flag for maximum optimization.
* **Code Auto-FOrmatting:** The project is set up with `clang-format` for consistent code styling. A `.clang-format` configuration file is included in the root directory, and a `format` target is available in the CMake build.
* **Dedicated Encoding/Scrambling Tool:** The `scramble_text` executable is a new tool that allows you to encode your own messages. It:
    1.  Takes a plain-text file as input.
    2.  Generates a new, random substitution cipher key.
    3.  Applies the key to scramble the text.
    4.  Saves the resulting secret message to an output file.
    5.  **Crucially,** it **saves the generated key to a separate file**, which is required for decoding.

* **Flexible Command-Line Interface:** The `run_deciphering` program features a powerful and user-friendly command-line interface that allows for full control over the decoding process:
    * `-i <file>`: Specify the training text (e.g., `warpeace_input.txt`).
    * `-d <file>`: Specify the secret message file to decode.
    * `-o <file>`: **(New)** Optionally specify a file to save the final, clean deciphered text to.
    * `-iters <number>`: Tune the number of iterations for each MCMC chain (default is `500000`).
    * `-print_every <number>`: Tune how often progress is printed to the console (default is `100000`).

## How to Build and Run

You must have a C++ compiler (like g++) and CMake installed.

### 1. Build the Project

From the root directory of the project, run the following commands:

```bash
# Create a build directory
mkdir build

# Navigate into it
cd build

# Run CMake to configure the project
cmake ..

# Run make to compile the code
make
```

This will create two executables in the `build` directory: `scramble_text` and `run_deciphering`.

### 2. Usage Examples

All commands should be run from within the `build` directory.

#### Encoding a Message

To create your own secret message from a plain text file (e.g., `../data/shakespeare_input.txt`):

```bash
./scramble_text -i ../data/shakespeare_input.txt -o my_secret_message.txt -k my_key.txt
```

* This will create `my_secret_message.txt` with the encoded text.
* This will create `my_key.txt` with the substitution key used to encode it.

#### Deciphering a Message

To decipher a secret message (`secret_message.txt`) using a training file (`warpeace_input.txt`):

```bash
# Basic usage
./run_deciphering -i ../data/warpeace_input.txt -d secret_message.txt

# With more iterations and saving the output to a file
./run_deciphering -i ../data/shakespeare_input.txt -d secret_message.txt -iters 1000000 -o deciphered_text.txt
```

This command will launch multiple parallel MCMC chains and, after they complete, print the best-guess deciphered text to the console and save it to `deciphered_text.txt` if the `-o` flag is used.

It is worth noting that through my many runs of this, I have found that training on `shakespeare_input.txt` produces significantly better results. This is most likely because its much longer that `warpeace_input.txt`