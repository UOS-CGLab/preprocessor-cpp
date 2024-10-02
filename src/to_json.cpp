#include "to_json.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>  // for std::transform

// 내부적으로만 사용할 함수들은 이곳에 정의
namespace {
    void clear_json(const std::string &output_dir)
    {
        std::string filename = output_dir + "/topology.json";
        std::ofstream json_file(filename);
        if (json_file.is_open())
        {
            json_file << "[]";  // 빈 배열로 초기화
            json_file.close();
        }
        else
        {
            std::cerr << "Error: Cannot open file " << filename << " for writing" << std::endl;
        }
    }

    void append_to_json(const nlohmann::json &data, const std::string &output_dir)
    {
        std::string filename = output_dir + "/topology.json";
        std::ifstream json_file_in(filename);
        nlohmann::json json_data;

        if (json_file_in.is_open())
        {
            if (json_file_in.peek() != std::ifstream::traits_type::eof()) // 파일이 비어 있지 않으면
            {
                json_file_in >> json_data;
            }
            json_file_in.close();
        }

        std::ofstream json_file_out(filename);
        if (json_file_out.is_open())
        {
            if (!json_data.empty()) // 기존 데이터가 있으면 새 데이터를 추가
            {
                json_data.push_back(data);
            }
            else // 파일이 비어 있으면
            {
                json_data = nlohmann::json::array({data});
            }

            json_file_out << json_data.dump(4);  // 4 스페이스로 들여쓰기
            json_file_out.close();
        }
        else
        {
            std::cerr << "Error: Cannot open file " << filename << " for writing" << std::endl;
        }
    }

    // 배열에 offset을 추가하는 함수
    template <typename T>
    std::vector<T> add_offset(const std::vector<T> &array, T offset)
    {
        std::vector<T> result(array.size());
        std::transform(array.begin(), array.end(), result.begin(), [offset](T x) { return x + offset; });
        return result;
    }
} // unnamed namespace

void to_json(const std::vector<int> &v_indices, const std::vector<int> &v_offsets, const std::vector<int> &v_valances, const std::vector<int> &v_index, const std::vector<int> &v_data,
             const std::vector<int> &e_indices, const std::vector<int> &e_data,
             const std::vector<int> &f_indices, const std::vector<int> &f_offsets, const std::vector<int> &f_valances, const std::vector<int> &f_data,
             int depth, const std::string &output_dir)
{
    if (depth == 0)
    {
        clear_json(output_dir);  // depth가 0일 경우 JSON 파일 초기화
    }

    nlohmann::json data;
    data["depth"] = depth;

    data["data"] = nlohmann::json::object();

    data["data"]["v_indices"] = v_indices;
    data["data"]["v_offsets"] = v_offsets;
    data["data"]["v_valances"] = v_valances;
    data["data"]["v_index"] = v_index;
    data["data"]["v_data"] = v_data;

    data["data"]["e_indices"] = e_indices;
    data["data"]["e_data"] = e_data;

    data["data"]["f_indices"] = f_indices;
    data["data"]["f_offsets"] = f_offsets;
    data["data"]["f_valances"] = f_valances;
    data["data"]["f_data"] = f_data;

    // JSON 데이터 추가
    append_to_json(data, output_dir);
}

// to_json2: offset 적용 후 JSON에 저장하는 함수
void to_json(const std::vector<int> &v_indices, const std::vector<int> &v_offsets, const std::vector<int> &v_valances, const std::vector<int> &v_index, const std::vector<int> &v_data,
              const std::vector<int> &e_indices, const std::vector<int> &e_data,
              const std::vector<int> &f_indices, const std::vector<int> &f_offsets, const std::vector<int> &f_valances, const std::vector<int> &f_data,
              int depth, int offset, const std::string &output_dir)
{
    nlohmann::json data;
    data["depth"] = depth;

    data["data"] = nlohmann::json::object();

    data["data"]["v_indices"] = add_offset(v_indices, offset);
    data["data"]["v_offsets"] = add_offset(v_offsets, offset);
    data["data"]["v_valances"] = v_valances;
    data["data"]["v_index"] = add_offset(v_index, offset);
    data["data"]["v_data"] = add_offset(v_data, offset);

    data["data"]["e_indices"] = add_offset(e_indices, offset);
    data["data"]["e_data"] = add_offset(e_data, offset);

    data["data"]["f_indices"] = add_offset(f_indices, offset);
    data["data"]["f_offsets"] = add_offset(f_offsets, offset);
    data["data"]["f_valances"] = f_valances;
    data["data"]["f_data"] = add_offset(f_data, offset);

    // JSON 데이터 추가
    append_to_json(data, output_dir);
}
