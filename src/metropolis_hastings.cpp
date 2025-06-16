#include "metropolis_hastings.hpp"

#include <cmath>
#include <iostream>
#include <random>

#include "utils.hpp"

std::map<char, char> metropolis_hastings_annealing(
    const std::map<char, char> &initial_state,
    std::function<std::map<char, char>(const std::map<char, char> &,
                                       const std::vector<char> &)>
        proposal_function,
    std::function<double(const std::string &, const std::map<char, char> &)>
        log_density,
    const std::vector<char> &chars, const std::string &text, int iters,
    int print_every, double initial_temp, double final_temp) {
  std::map<char, char> current_state = initial_state;
  double p1 = log_density(text, current_state);

  std::random_device rd;
  std::mt19937 g(rd());
  std::uniform_real_distribution<> dis(0.0, 1.0);

  // Annealing schedule: exponential decay
  double temp_factor = std::pow(final_temp / initial_temp, 1.0 / (iters - 1));
  double temp = initial_temp;

  for (int i = 0; i < iters; ++i) {
    std::map<char, char> proposed_state =
        proposal_function(current_state, chars);
    double p2 = log_density(text, proposed_state);

    // Standard Metropolis-Hastings acceptance criterion, but with
    // temperature
    if ((p2 - p1) / temp > std::log(dis(g))) {
      current_state = proposed_state;
      p1 = p2;
    }

    if (i % print_every == 0) {
      std::cout << "Iteration " << i << " (Temp: " << temp
                << "): " << apply_permutation(text.substr(0, 70), current_state)
                << "..." << std::endl;
    }

    // Cool the temperature for the next iteration
    temp *= temp_factor;
  }

  return current_state;
}