#ifndef TO_JSON_H
#define TO_JSON_H

#include <vector>
#include <string>

void to_json(const std::vector<int> &v_indices, const std::vector<int> &v_offsets, const std::vector<int> &v_valences, const std::vector<int> &v_index, const std::vector<int> &v_data,
             const std::vector<int> &e_indices, const std::vector<int> &e_data,
             const std::vector<int> &f_indices, const std::vector<int> &f_offsets, const std::vector<int> &f_valances, const std::vector<int> &f_data,
             int depth, const std::string &output_dir);

#endif // TO_JSON_H
