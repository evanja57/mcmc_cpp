#ifndef UTILS_HPP
#define UTILS_HPP

#include <map>
#include <string>
#include <vector>

// Function declarations
std::vector<char> az_list();
std::map<char, char> generate_random_permutation_map(
    const std::vector<char> &chars);
std::map<char, char> generate_identity_p_map(const std::vector<char> &chars);
std::vector<char> get_chars_from_text(const std::string &text);
std::map<char, int> get_char_to_ix(const std::vector<char> &chars);
std::vector<double> get_frequency_statistics(
    const std::string &text, const std::vector<char> &chars,
    const std::map<char, int> &char_to_ix);
std::vector<std::vector<double> > get_transition_matrix(
    const std::string &text, const std::vector<char> &chars,
    const std::map<char, int> &char_to_ix);
std::string apply_permutation(const std::string &text,
                              const std::map<char, char> &permutation_map);
void read_file(const std::string &filename, std::string &text);

void print_permutation_map(const std::map<char, char> &p_map);

/**
 * @brief Saves a permutation map (cipher key) to a file.
 * * @param p_map The permutation map to save.
 * @param filename The name of the file to save the key to.
 */
void save_permutation_map(const std::map<char, char> &p_map,
                          const std::string &filename);

#endif  // UTILS_HPP