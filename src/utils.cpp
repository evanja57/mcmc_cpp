#include "utils.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>

std::vector<char> az_list() {
  std::vector<char> cx;
  for (char c = 'a'; c <= 'z'; ++c) {
    cx.push_back(c);
  }
  for (char c = 'A'; c <= 'Z'; ++c) {
    cx.push_back(c);
  }
  return cx;
}
std::map<char, char> generate_random_permutation_map(
    const std::vector<char> &chars) {
  std::map<char, char> p_map = generate_identity_p_map(chars);
  std::vector<char> cx = az_list();
  std::vector<char> cx2 = az_list();
  std::random_device rd;
  std::mt19937 g(rd());
  std::shuffle(cx2.begin(), cx2.end(), g);

  for (size_t i = 0; i < cx.size(); ++i) {
    p_map[cx[i]] = cx2[i];
  }
  return p_map;
}

std::map<char, char> generate_identity_p_map(const std::vector<char> &chars) {
  std::map<char, char> p_map;
  for (char c : chars) {
    p_map[c] = c;
  }
  return p_map;
}

std::vector<char> get_chars_from_text(const std::string &text) {
  std::vector<char> chars;
  std::map<char, bool> char_map;
  for (char c : text) {
    if (char_map.find(c) == char_map.end()) {
      chars.push_back(c);
      char_map[c] = true;
    }
  }
  return chars;
}

std::map<char, int> get_char_to_ix(const std::vector<char> &chars) {
  std::map<char, int> char_to_ix;
  for (size_t i = 0; i < chars.size(); ++i) {
    char_to_ix[chars[i]] = i;
  }
  return char_to_ix;
}

std::vector<double> get_frequency_statistics(
    const std::string &text, const std::vector<char> &chars,
    const std::map<char, int> &char_to_ix) {
  std::vector<double> freq(chars.size(), 0.0);
  for (char c : text) {
    if (char_to_ix.count(c)) {
      freq[char_to_ix.at(c)]++;
    }
  }
  for (size_t i = 0; i < freq.size(); ++i) {
    freq[i] /= text.length();
  }
  return freq;
}

std::vector<std::vector<double> > get_transition_matrix(
    const std::string &text, const std::vector<char> &chars,
    const std::map<char, int> &char_to_ix) {
  int n_chars = chars.size();
  std::vector<std::vector<double> > trans_matrix(
      n_chars, std::vector<double>(n_chars, 1.0));  // Add-1 smoothing
  for (size_t i = 0; i < text.length() - 1; ++i) {
    if (char_to_ix.count(text[i]) && char_to_ix.count(text[i + 1])) {
      trans_matrix[char_to_ix.at(text[i])][char_to_ix.at(text[i + 1])]++;
    }
  }

  for (int i = 0; i < n_chars; ++i) {
    double total = 0;
    for (int j = 0; j < n_chars; ++j) {
      total += trans_matrix[i][j];
    }
    for (int j = 0; j < n_chars; ++j) {
      trans_matrix[i][j] /= total;
    }
  }
  return trans_matrix;
}

std::string apply_permutation(const std::string &text,
                              const std::map<char, char> &permutation_map) {
  std::string new_text = "";
  for (char c : text) {
    if (permutation_map.count(c)) {
      new_text += permutation_map.at(c);
    } else {
      new_text += c;
    }
  }
  return new_text;
}

void read_file(const std::string &filename, std::string &text) {
  std::ifstream file(filename);
  if (file.is_open()) {
    std::string line;
    // Correctly replace newlines with spaces, matching the Python
    // implementation
    while (std::getline(file, line)) {
      text += line + " ";
    }
    file.close();
    // Remove the final trailing space
    if (!text.empty()) {
      text.pop_back();
    }
  } else {
    std::cerr << "Unable to open file " << filename << std::endl;
  }
}

void print_permutation_map(const std::map<char, char> &p_map) {
  std::cout << "Permutation Map (Key):\n";
  for (const auto &pair : p_map) {
    std::cout << "  " << pair.first << " -> " << pair.second << "\n";
  }
  std::cout << std::endl;
}

/**
 * @brief Saves the permutation map to a file, one mapping per line.
 */
void save_permutation_map(const std::map<char, char> &p_map,
                          const std::string &filename) {
  std::ofstream file(filename);
  if (!file.is_open()) {
    std::cerr << "Could not open file to save key: " << filename << std::endl;
    return;
  }
  for (const auto &pair : p_map) {
    file << pair.first << " " << pair.second << "\n";
  }
  file.close();
  std::cout << "Successfully saved key to " << filename << std::endl;
}