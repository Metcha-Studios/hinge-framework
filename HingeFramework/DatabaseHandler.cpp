#include "pch.h"
#include "include/HingeFramework/DatabaseHandler.h"

#include <cmath>
#include <string>
#include <sstream>
#include <vector>
#include <memory>
#include <filesystem>
#include <utf8.h>
#include <sqlite3.h>
#include <SQLiteCpp/SQLiteCpp.h>
#include <libxl/libxl.h>


hinge_framework::DatabaseHandler::DatabaseHandler(const std::string& db_file_path, const std::string& key)
    : db_file_path_(db_file_path), key_(key), database_(nullptr) {}

hinge_framework::DatabaseHandler::~DatabaseHandler() {
    closeDatabase();
}

bool hinge_framework::DatabaseHandler::tableExists(const std::string& table_name) {
    // Check if the table exists in the database
    std::string query = "SELECT name FROM sqlite_master WHERE type='table' AND name=?";
    SQLite::Statement query_stmt(*this->database_, query);
    query_stmt.bind(1, table_name);
    return query_stmt.executeStep();
}

bool hinge_framework::DatabaseHandler::clearTable(const std::string& table_name) {
    // Clear data from the table
    std::string query = "DELETE FROM " + table_name;
    SQLite::Statement query_stmt(*this->database_, query);
    return query_stmt.exec();
}

bool hinge_framework::DatabaseHandler::importFromExcel(const char*& input_path) {
    if (!openDatabase()) {
        return false;
    }

    libxl::Book* book = xlCreateXMLBook();
    book->setKey(L"libxl", L"windows-28232b0208c4ee0369ba6e68abv6v5i3");
    if (!book) {
        closeDatabase();
        return false;
    }

    std::string input_path_str = input_path;
    if (input_path_str.empty()) {
        closeDatabase();
        return false;
    }

    if (!book->load(utf8ToWide(input_path_str).c_str())) {
        book->release();
        closeDatabase();
        return false;
    }

    try {
        libxl::Sheet* sheet;
        // Iterate over sheets in the workbook
        for (size_t sheet_index = 0; sheet_index < book->sheetCount(); ++sheet_index) {
            sheet = book->getSheet(sheet_index);
            if (!sheet)
                continue;

            std::string table_name = wideToUtf8(sheet->name());

            // Clear existing data in the table
            std::string clear_table_query = "DELETE FROM " + table_name + ";";
            database_->exec(clear_table_query);

            // Check if the table already exists
            bool table_exists = database_->tableExists(table_name);

            // Get column names from the first row
            std::vector<std::string> column_names;
            for (size_t col = 0; col < sheet->lastCol(); ++col) {
                std::string column_name = wideToUtf8(sheet->readStr(0, col));
                // Check if the column name exists in the target table
                if (table_exists && !columnExists(table_name, column_name))
                    continue;
                column_names.push_back(column_name);
            }

            if (!table_exists) {
                // Create the table if it doesn't exist
                std::stringstream create_table_query;
                create_table_query << "CREATE TABLE " << table_name << " (";
                for (size_t col = 0; col < column_names.size(); ++col) {
                    if (col > 0)
                        create_table_query << ", ";
                    create_table_query << column_names[col] << " TEXT";
                }
                create_table_query << ");";
                database_->exec(create_table_query.str());
            }

            // Read data from the sheet and insert into the corresponding database table
            for (size_t row = 1; row < sheet->lastRow(); ++row) {
                std::stringstream insert_query;
                insert_query << "INSERT INTO " << table_name << " (";
                for (size_t col = 0; col < column_names.size(); ++col) {
                    if (col > 0)
                        insert_query << ", ";
                    insert_query << column_names[col];
                }
                insert_query << ") VALUES (";
                for (size_t col = 0; col < column_names.size(); ++col) {
                    std::string cell_data;
                    if (col < sheet->lastCol()) {
                        cell_data = wideToUtf8(sheet->readStr(row, col));
                    }
                    if (col > 0)
                        insert_query << ", ";
                    if (cell_data.empty()) {
                        insert_query << "NULL";
                    }
                    else {
                        insert_query << "'" << cell_data << "'";
                    }
                }
                insert_query << ");";
                database_->exec(insert_query.str());
            }
        }
        book->release();
        closeDatabase();
    }
    catch (const std::exception& e) {
        throw std::exception(e.what());
    }

    return true;
}

bool hinge_framework::DatabaseHandler::exportToExcel(const char*& output_path, const uint16_t color_value_header, const uint16_t color_value_content_layer, const uint16_t color_value_content_base) {
    if (!openDatabase()) {
        //throw std::runtime_error("Failed to open the database");
        return false;
    }

    libxl::Book* book = xlCreateXMLBook();
    book->setKey(L"libxl", L"windows-28232b0208c4ee0369ba6e68abv6v5i3");
    if (!book) {
        closeDatabase();
        return false;
    }

    std::string output_path_str = output_path;
    if (output_path_str.empty()) {
        // If no output path provided, use database name as default
        size_t last_slash = db_file_path_.find_last_of("/\\");
        std::string dbName = db_file_path_.substr(last_slash + 1);
        output_path_str += dbName + ".xlsx";
    }

    // Ensure the output directory exists, if not, create it
    std::filesystem::path output_dir = std::filesystem::path(output_path_str).parent_path();
    if (!std::filesystem::exists(output_dir))
        std::filesystem::create_directories(output_dir);

    // Create format
    libxl::Format* header_format = book->addFormat();
    libxl::Font* header_font = book->addFont();
    libxl::Format* content_format0 = book->addFormat();
    libxl::Font* content_font0 = book->addFont();

    header_font->setBold(true);
    header_font->setColor((color_value_header >= 128) ? libxl::COLOR_BLACK : libxl::COLOR_WHITE); // Set text color
    header_format->setFont(header_font);
    header_format->setAlignH(libxl::AlignH::ALIGNH_CENTER);
    header_format->setAlignV(libxl::AlignV::ALIGNV_CENTER);
    book->setRgbMode(true);
    libxl::Color header_color = book->colorPack(color_value_header, color_value_header, color_value_header);
    header_format->setFillPattern(libxl::FILLPATTERN_SOLID);
    header_format->setPatternForegroundColor(header_color);

    // Create format for content
    libxl::Color content_color0 = book->colorPack(color_value_content_base, color_value_content_base, color_value_content_base); // White color for odd rows
    libxl::Color content_color1 = book->colorPack(color_value_content_layer, color_value_content_layer, color_value_content_layer); // Light gray color for even rows

    content_font0->setBold(false);
    content_font0->setColor(libxl::COLOR_BLACK);
    content_format0->setFont(content_font0);
    content_format0->setAlignH(libxl::AlignH::ALIGNH_CENTER);
    content_format0->setAlignV(libxl::AlignV::ALIGNV_CENTER);
    content_format0->setFillPattern(libxl::FILLPATTERN_SOLID);
    content_format0->setPatternForegroundColor(content_color0);

    libxl::Format* content_format1 = book->addFormat();
    libxl::Font* content_font1 = book->addFont();
    content_font1->setBold(false);
    content_format1->setFont(content_font1);
    content_format1->setAlignH(libxl::AlignH::ALIGNH_CENTER);
    content_format1->setAlignV(libxl::AlignV::ALIGNV_CENTER);
    content_format1->setFillPattern(libxl::FILLPATTERN_SOLID);
    content_format1->setPatternForegroundColor(content_color1);

    // Iterate over tables in the database
    SQLite::Statement tables_query(*database_, "SELECT name FROM sqlite_master WHERE type='table'");
    while (tables_query.executeStep()) {
        std::string table_name = tables_query.getColumn(0).getText();

        libxl::Sheet* sheet = book->addSheet(utf8ToWide(table_name).c_str());
        if (!sheet) {
            book->release();
            closeDatabase();
            return false;
        }

        // Get column names and data types
        std::vector<hinge_framework::ColumnInfo> column_info = getColumnInfo(table_name);

        // Write column names as header with format
        for (size_t i = 0; i < column_info.size(); ++i) {
            sheet->writeStr(0, i, utf8ToWide(column_info[i].name).c_str(), header_format);
        }

        // Fetch and write data
        SQLite::Statement data_query(*database_, "SELECT * FROM " + table_name);
        uint64_t row = 1;
        while (data_query.executeStep()) {
            libxl::Format* current_format = (row % 2 == 0) ? content_format1 : content_format0; // Alternate row colors
            for (size_t i = 0; i < column_info.size(); ++i) {
                std::string data = data_query.getColumn(i).getText();
                if (column_info[i].type == "INTEGER") {
                    int64_t int_value = std::stoll(data);
                    sheet->writeNum(row, i, int_value, current_format);
                }
                else if (column_info[i].type == "REAL") {
                    double numeric_value = std::stod(data);
                    libxl::Format* real_format = book->addFormat();
                    real_format->setFont(current_format->font());
                    real_format->setAlignH(current_format->alignH());
                    real_format->setAlignV(current_format->alignV());
                    real_format->setFillPattern(current_format->fillPattern());
                    real_format->setPatternForegroundColor(current_format->patternForegroundColor());
                    real_format->setNumFormat(libxl::NUMFORMAT_NUMBER_SEP_D2);
                    sheet->writeNum(row, i, numeric_value, real_format);
                }
                else {
                    sheet->writeStr(row, i, utf8ToWide(data).c_str(), current_format);
                }
            }
            ++row;
        }

        // Set row height and column width
        setRowHeightAndColumnWidth(sheet, 24, column_info);
    }

    // Save workbook to file
    bool success = book->save(utf8ToWide(output_path_str).c_str());
    book->release();
    closeDatabase();
    return success;
}

bool hinge_framework::DatabaseHandler::openDatabase() {
    try {
        database_ = new SQLite::Database(this->db_file_path_.c_str(), SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
        database_->key(this->key_.c_str()); // Set encryption key
        return true;
    }
    catch (std::exception& e) {
        // Handle exception (e.g., file not found, encryption key incorrect)
        return false;
    }
}

void hinge_framework::DatabaseHandler::closeDatabase() {
    if (database_ != nullptr) {
        delete database_;
        database_ = nullptr;
    }
}

std::wstring hinge_framework::DatabaseHandler::utf8ToWide(const std::string& utf8_str) {
    std::wstring wide_str;
    utf8::utf8to16(utf8_str.begin(), utf8_str.end(), std::back_inserter(wide_str));
    return wide_str;
}

std::string hinge_framework::DatabaseHandler::wideToUtf8(const std::wstring& wide_str) {
    std::string utf8_str;
    utf8::utf16to8(wide_str.begin(), wide_str.end(), std::back_inserter(utf8_str));
    return utf8_str;
}

// Function to get column names and data types from database
std::vector<hinge_framework::ColumnInfo> hinge_framework::DatabaseHandler::getColumnInfo(const std::string& table_name) {
    std::vector<ColumnInfo> column_info;
    SQLite::Statement columns_query(*this->database_, "PRAGMA table_info(" + table_name + ")");
    while (columns_query.executeStep()) {
        ColumnInfo info;
        info.name = columns_query.getColumn(1).getText();
        info.type = columns_query.getColumn(2).getText();
        column_info.push_back(info);
    }
    return column_info;
}

void hinge_framework::DatabaseHandler::setRowHeightAndColumnWidth(libxl::Sheet* sheet, uint16_t row_height, const std::vector<hinge_framework::ColumnInfo>& column_info) {
    // Set row height
    for (size_t i = 0; i < sheet->lastRow(); ++i) {
        sheet->setRow(i, row_height);
    }

    // Set column width
    for (size_t i = 0; i < column_info.size(); ++i) {
        uint32_t width = 0; // Default width
        for (size_t j = 0; j < sheet->lastRow(); ++j) {
            std::wstring cell_data = sheet->readStr(j, i);
            if (cell_data.length() > width) {
                width = cell_data.length();
            }
        }
        sheet->setCol(i, i, (width + 8 < 20) ? 20 : width + 8);
    }
}

bool hinge_framework::DatabaseHandler::createTable(const std::string& table_name, const std::vector<ColumnInfo>& columns) {
    std::string query = "CREATE TABLE IF NOT EXISTS " + table_name + " (";
    for (size_t i = 0; i < columns.size(); ++i) {
        query += columns[i].name + " " + columns[i].type;
        if (i != columns.size() - 1) {
            query += ", ";
        }
    }
    query += ");";

    try {
        SQLite::Statement create_table(*database_, query);
        create_table.exec();
        return true;
    }
    catch (std::exception& e) {
        std::string error_message = "Exception caught in createTable(): ";
        error_message += e.what();
        throw std::runtime_error(error_message);
        return false;
    }
}

void hinge_framework::DatabaseHandler::insertData(const std::string& table_name, const std::vector<ColumnInfo>& columns, libxl::Sheet* sheet) {
    std::wstring query = L"INSERT INTO " + utf8ToWide(table_name) + L" (";
    for (size_t i = 0; i < columns.size(); ++i) {
        query += utf8ToWide(columns[i].name);
        if (i != columns.size() - 1) {
            query += L", ";
        }
    }
    query += L") VALUES (";

    for (int i = 1; i <= sheet->lastRow(); ++i) {
        std::wstring row_values;
        for (size_t j = 0; j < columns.size(); ++j) {
            std::wstring cell_value;
            if (j < sheet->lastCol()) {
                cell_value = sheet->readStr(i, j);
            }
            else {
                cell_value = L""; // If column index exceeds the number of columns in the sheet, insert empty string
            }
            row_values += L"'" + cell_value + L"'";
            if (j != columns.size() - 1) {
                row_values += L", ";
            }
        }
        std::wstring insert_query = query + row_values + L");";
        try {
            SQLite::Statement insert_data(*database_, wideToUtf8(insert_query));
            insert_data.exec();
        }
        catch (std::exception& e) {
            std::string error_message = "Exception caught in insertData(): ";
            error_message += e.what();
            throw std::runtime_error(error_message);
        }
    }
}

// Function to check if a column exists in a table
bool hinge_framework::DatabaseHandler::columnExists(const std::string& table_name, const std::string& column_name) {
    std::string query = "PRAGMA table_info(" + table_name + ");";

    SQLite::Statement query_result(*this->database_, query);

    while (query_result.executeStep()) {
        std::string existing_column_name = query_result.getColumn(1).getString();
        if (existing_column_name == column_name) {
            return true;
        }
    }

    return false;
}
