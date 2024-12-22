#ifndef _CSV_LOG_FILE_HPP_
#define _CSV_LOG_FILE_HPP_

#include "ilog_file.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <stdexcept>

namespace hako::logger {

/**
 * CsvLogFile
 * Implementation of ILogFile for managing CSV files.
 */
class CsvLogFile : public ILogFile {
private:
    std::string file_name_; // File name for resetting the file
    std::ofstream csv_file;
    std::ifstream csv_read_file;
    std::vector<std::vector<std::string>> data; // Stores all rows for reading
    size_t current_index = 0;                   // Current reading position

public:
    /**
     * Constructor for creating a new CSV log file.
     * @param file_name Name of the file to be created.
     * @param header Header information for the CSV file.
     */
    CsvLogFile(const std::string& file_name, const std::vector<LogHeaderType>& header)
        : ILogFile(file_name, header) {
        file_name_ = file_name;
        reset(); // Initialize the log file
        write_header(header);
    }

    /**
     * Constructor for loading an existing CSV log file.
     * @param file_name Name of the file to be loaded.
     */
    CsvLogFile(const std::string& file_name)
        : ILogFile(file_name) {
        file_name_ = file_name;
        csv_read_file.open(file_name, std::ios::in);
        if (!csv_read_file.is_open()) {
            throw std::runtime_error("Failed to open file for reading: " + file_name);
        }
        load_all_data();
    }

    /**
     * Destructor: Ensures files are properly closed.
     */
    ~CsvLogFile() {
        if (csv_file.is_open()) {
            csv_file.close();
        }
        if (csv_read_file.is_open()) {
            csv_read_file.close();
        }
    }

    /**
     * Writes a row of log data to the CSV file.
     * @param value The data to be written.
     */
    void write(const std::vector<LogDataType>& value) override {
        if (!csv_file.is_open()) {
            throw std::runtime_error("CSV file is not open for writing.");
        }
        for (size_t i = 0; i < value.size(); ++i) {
            csv_file << log_data_to_string(value[i]);
            if (i < value.size() - 1) {
                csv_file << ",";
            }
        }
        csv_file << "\n";
    }

    /**
     * Flushes the current data to the file.
     */
    void flush() override {
        if (csv_file.is_open()) {
            csv_file.flush();
        }
    }

    /**
     * Reads the next row of log data from the file.
     * Converts the string values into the appropriate types stored in LogDataType.
     * @param value Output parameter to store the read data as LogDataType.
     * @return True if a row was successfully read, false otherwise.
     */
    bool read(std::vector<LogDataType>& value) override {
        if (current_index < data.size()) {
            const auto& row = data[current_index++];
            value.resize(row.size());

            for (size_t i = 0; i < row.size(); ++i) {
                try {
                    value[i] = parse_cell(row[i], value[i]);
                } catch (const std::exception& e) {
                    throw std::runtime_error("Error parsing cell [" + row[i] + "] at index " + std::to_string(i) + ": " + e.what());
                }
            }
            return true;
        }
        return false;
    }

    /**
     * Removes the last logged row from the in-memory storage.
     */
    void remove_last() override {
        if (!data.empty()) {
            data.pop_back();
        }
    }
    /**
     * Resets the log file by clearing its contents.
     */
    void reset() override {
        if (csv_file.is_open()) {
            csv_file.close();
        }
        csv_file.open(file_name_, std::ios::out | std::ios::trunc);
        if (!csv_file.is_open()) {
            throw std::runtime_error("Failed to reset file: " + file_name_);
        }

        // Reset reading-related states
        data.clear();
        current_index = 0;
    }

private:
    /**
     * Writes the header row to the CSV file.
     * @param header Header information for the CSV file.
     */
    void write_header(const std::vector<LogHeaderType>& header) {
        for (size_t i = 0; i < header.size(); ++i) {
            csv_file << header[i].name;
            if (i < header.size() - 1) {
                csv_file << ",";
            }
        }
        csv_file << "\n";
    }

    /**
     * Loads all data from the CSV file into memory for reading.
     */
    void load_all_data() {
        std::string line;
        while (std::getline(csv_read_file, line)) {
            std::vector<std::string> row;
            std::string cell;
            std::istringstream lineStream(line);
            while (std::getline(lineStream, cell, ',')) {
                row.push_back(cell);
            }
            data.push_back(row);
        }
        csv_read_file.close();
    }

    /**
     * Converts a LogDataType value to its string representation.
     * @param value The LogDataType to be converted.
     * @return A string representation of the value.
     */
    std::string log_data_to_string(const LogDataType& value) const {
        if (std::holds_alternative<bool>(value)) return std::to_string(std::get<bool>(value));
        if (std::holds_alternative<int8_t>(value)) return std::to_string(std::get<int8_t>(value));
        if (std::holds_alternative<uint8_t>(value)) return std::to_string(std::get<uint8_t>(value));
        if (std::holds_alternative<int16_t>(value)) return std::to_string(std::get<int16_t>(value));
        if (std::holds_alternative<uint16_t>(value)) return std::to_string(std::get<uint16_t>(value));
        if (std::holds_alternative<int32_t>(value)) return std::to_string(std::get<int32_t>(value));
        if (std::holds_alternative<uint32_t>(value)) return std::to_string(std::get<uint32_t>(value));
        if (std::holds_alternative<int64_t>(value)) return std::to_string(std::get<int64_t>(value));
        if (std::holds_alternative<uint64_t>(value)) return std::to_string(std::get<uint64_t>(value));
        if (std::holds_alternative<float>(value)) return std::to_string(std::get<float>(value));
        if (std::holds_alternative<double>(value)) return std::to_string(std::get<double>(value));
        if (std::holds_alternative<std::string>(value)) return std::get<std::string>(value);
        return "";
    }

    /**
     * Parses a single cell from a string and converts it to the appropriate type in LogDataType.
     * @param cell The cell value as a string.
     * @param current The current LogDataType instance to determine the type.
     * @return The parsed LogDataType instance with the correct value.
     */
    LogDataType parse_cell(const std::string& cell, const LogDataType& current) const {
        try {
            if (std::holds_alternative<bool>(current)) {
                return std::stoi(cell) != 0;
            } else if (std::holds_alternative<int8_t>(current)) {
                return static_cast<int8_t>(std::stoi(cell));
            } else if (std::holds_alternative<uint8_t>(current)) {
                return static_cast<uint8_t>(std::stoi(cell));
            } else if (std::holds_alternative<int16_t>(current)) {
                return static_cast<int16_t>(std::stoi(cell));
            } else if (std::holds_alternative<uint16_t>(current)) {
                return static_cast<uint16_t>(std::stoi(cell));
            } else if (std::holds_alternative<int32_t>(current)) {
                return std::stoi(cell);
            } else if (std::holds_alternative<uint32_t>(current)) {
                return static_cast<uint32_t>(std::stoul(cell));
            } else if (std::holds_alternative<int64_t>(current)) {
                return std::stoll(cell);
            } else if (std::holds_alternative<uint64_t>(current)) {
                return std::stoull(cell);
            } else if (std::holds_alternative<float>(current)) {
                return std::stof(cell);
            } else if (std::holds_alternative<double>(current)) {
                return std::stod(cell);
            } else if (std::holds_alternative<std::string>(current)) {
                return cell;
            }
            throw std::runtime_error("Unsupported type in LogDataType");
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string("Failed to parse value: ") + e.what());
        }
    }    
};

} // namespace hako::logger

#endif /* _CSV_LOG_FILE_HPP_ */
