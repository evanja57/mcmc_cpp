#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <mutex>
#include <thread>
#include <vector>

#include "deciphering_utils.hpp"
#include "metropolis_hastings.hpp"
#include "utils.hpp"

int main(int argc, char *argv[]) {
  std::string train_file;
  std::string decode_file;
  std::string output_file;
  int iters = 500000;       // Default number of iterations
  int print_every = 10000;  // Default print frequency

  // Loop through command-line arguments
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "-i" && i + 1 < argc) {
      train_file = argv[++i];
    } else if (arg == "-d" && i + 1 < argc) {
      decode_file = argv[++i];
    } else if (arg == "-o" && i + 1 < argc) {
      output_file = argv[++i];
    } else if ((arg == "-iters" || arg == "-e") && i + 1 < argc) {
      try {
        iters = std::stoi(argv[++i]);
      } catch (const std::exception &e) {
        std::cerr << "Invalid number for -iters: " << argv[i] << std::endl;
        return 1;
      }
    } else if ((arg == "-print_every" || arg == "-p") && i + 1 < argc) {
      try {
        print_every = std::stoi(argv[++i]);
      } catch (const std::exception &e) {
        std::cerr << "Invalid number for -print_every: " << argv[i]
                  << std::endl;
        return 1;
      }
    }
  }

  // Check if required arguments were provided
  if (train_file.empty() || decode_file.empty()) {
    std::cerr << "Usage: " << argv[0]
              << " -i <inputfile> -d <decodefile> [-iters <number>] "
                 "[-print_every <number>]"
              << std::endl;
    return 1;
  }

  std::string train_text, decode_text;
  read_file(train_file, train_text);
  read_file(decode_file, decode_text);

  std::vector<char> model_chars = get_chars_from_text(train_text);
  auto char_to_ix = get_char_to_ix(model_chars);
  auto freq_stats =
      get_frequency_statistics(train_text, model_chars, char_to_ix);
  auto trans_matrix =
      get_transition_matrix(train_text, model_chars, char_to_ix);
  std::vector<char> az_chars = az_list();

  // Pre-calculate logs of the statistics
  std::vector<double> log_freq_stats = freq_stats;
  for (double &val : log_freq_stats) {
    // Add a small epsilon to avoid log(0) which is -infinity
    val = std::log(val + 1e-10);
  }

  std::vector<std::vector<double> > log_trans_matrix = trans_matrix;
  for (auto &row : log_trans_matrix) {
    for (double &val : row) {
      val = std::log(val + 1e-10);
    }
  }

  // Pre-calculate the transition counts of the text to decode
  TransitionCounts decode_text_counts =
      compute_transition_counts(decode_text, char_to_ix);
  char first_char = decode_text.empty() ? ' ' : decode_text[0];

  // The log density function uses the pre-calculated counts
  auto log_density_func = [&](const std::string &text,
                              const std::map<char, char> &p_map) {
    // The text parameter is ignored, but kept to match the function
    // signature required by metropolis_hastings_annealing.
    return compute_log_probability_by_counts(decode_text_counts, first_char,
                                             p_map, char_to_ix, log_freq_stats,
                                             log_trans_matrix);
  };

  int n_chains = std::thread::hardware_concurrency();
  std::vector<std::thread> threads;
  std::map<char, char> best_permutation;
  double max_log_prob = -std::numeric_limits<double>::infinity();
  std::mutex mtx;

  std::cout << "Starting " << n_chains
            << " parallel MCMC chains with Simulated Annealing..." << std::endl;

  for (int i = 0; i < n_chains; ++i) {
    threads.emplace_back([&, i]() {
      std::cout << "--- Chain " << i + 1 << " starting ---" << std::endl;
      auto initial_permutation = generate_random_permutation_map(model_chars);

      auto final_chain_permutation = metropolis_hastings_annealing(
          initial_permutation, propose_move, log_density_func, az_chars,
          decode_text, iters, print_every);

      double final_log_prob =
          log_density_func(decode_text, final_chain_permutation);

      // Lock the mutex before accessing shared variables
      std::lock_guard<std::mutex> lock(mtx);
      std::cout << "\n--- Chain " << i + 1
                << " finished with log probability: " << final_log_prob
                << " ---\n"
                << std::endl;
      // std::cout << apply_permutation(decode_text, final_chain_permutation)
      // << std::endl;

      if (final_log_prob > max_log_prob) {
        max_log_prob = final_log_prob;
        best_permutation = final_chain_permutation;
      }
    });
  }

  for (auto &th : threads) {
    th.join();
  }

  std::cout << "\n*************************************************************"
               "*******************\n"
            << std::endl;
  std::cout << "Best Deciphered Text (log prob: " << max_log_prob << "):\n\n\n"
            << apply_permutation(decode_text, best_permutation) << "\n\n"
            << std::endl;
  std::cout << "***************************************************************"
               "*****************"
            << std::endl;

  // saving to optional output file
  if (!output_file.empty()) {
    std::cout << "\nSaving best deciphered text to " << output_file << "..."
              << std::endl;
    std::ofstream out_f(output_file);
    if (!out_f.is_open()) {
      std::cerr << "Error: Could not open file to save output: " << output_file
                << std::endl;
    } else {
      out_f << apply_permutation(decode_text, best_permutation);
      out_f.close();
      std::cout << "Successfully saved to " << output_file << std::endl;
    }
  }

  return 0;
}