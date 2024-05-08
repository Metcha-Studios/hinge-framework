#include "pch.h"
#include "include/HingeFramework/DatabaseHandler.h"

#include <cmath>
#include <string>
#include <vector>
#include <variant>
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
        database_ = new SQLite::Database(db_file_path_.c_str(), SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
        database_->key(key_.c_str()); // Set encryption key
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