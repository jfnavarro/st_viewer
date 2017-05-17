#include "STData.h"

STData::STData()
{
}

STData::STData(const STData &other)
{
    m_counts_matrix = other.m_counts_matrix;
}

STData::~STData()
{
}

STData &STData::operator=(const STData &other)
{
    m_counts_matrix = other.m_counts_matrix;
    return (*this);
}

bool STData::operator==(const STData &other) const
{
    Q_UNUSED(other);
    //TODO implement
    return true;
}

void STData::parse_matrix(QByteArray filename) {
    // TODO the values type can be templated
    std::vector<std::string> row_names;
    std::vector<std::string> col_names;
    std::vector<std::vector<float>> values;

    // Open file
    std::ifstream f(filename.data());

    // Process the rest of the lines (row names and counts)
    unsigned row_number = 0;
    unsigned col_number = 0;
    char sep = '\t';
    for (std::string line; std::getline(f, line);) {
        std::istringstream iss(line);
        std::string token;
        std::vector<float> values_row;
        col_number = 0;
        while(std::getline(iss, token, sep)) {
            if (row_number == 0) {
                col_names.push_back(token);
            } else if (col_number == 0) {
                row_names.push_back(token);
            } else {
                values_row.push_back(std::stod(token));
            }
            ++col_number;
        }
        if (row_number > 0) {
            values.push_back(values_row);
        }
        ++row_number;
    }
    // Close file
    f.close();

    if (row_names.empty() || col_names.empty()) {
        throw std::runtime_error("The file does not contain a valid matrix");
    }

    if (col_names.size() != col_number - 1) {
        throw std::runtime_error("The header is missing or has no elements");
    }

    // Create an armadillo matrix
    Mat<float> counts_matrix(row_number - 1, col_number - 1);
    for (size_t i = 0; i < row_number - 1; ++i) {
        for (size_t j = 0; j < col_number - 1; ++j) {
            counts_matrix(i, j) = values[i][j];
        }
    }

    m_counts_matrix = counts_matrix;
}
