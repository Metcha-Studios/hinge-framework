#include "pch.h"
#include "include/HingeFramework/DatabaseHandler.h"

#include <string>
#include <vector>
#include <filesystem>
#include <utf8.h>
#include <sqlite3.h>
#include <SQLiteCpp/SQLiteCpp.h>
#include <libxl/libxl.h>



hinge_framework::DatabaseHandler::DatabaseHandler(const std::string& dbFilePath, const std::string& key)
    : dbFilePath_(dbFilePath), key_(key), database_(nullptr) {}

hinge_framework::DatabaseHandler::~DatabaseHandler() {
    closeDatabase();
}

std::wstring hinge_framework::DatabaseHandler::utf8ToWide(const std::string& utf8Str) {
    std::wstring wideStr;
    utf8::utf8to16(utf8Str.begin(), utf8Str.end(), std::back_inserter(wideStr));
    return wideStr;
}

bool hinge_framework::DatabaseHandler::exportToExcel(std::string& outputPath, const uint16_t color_value_header, const uint16_t color_value_content_layer, const uint16_t color_value_content_base) {
    if (!openDatabase()) {
        return false;
    }

    libxl::Book* book = xlCreateXMLBook();
    book->setKey(L"libxl", L"windows-28232b0208c4ee0369ba6e68abv6v5i3");
    if (!book) {
        closeDatabase();
        return false;
    }

    if (outputPath.empty()) {
        // If no output path provided, use database name as default
        size_t lastSlash = dbFilePath_.find_last_of("/\\");
        std::string dbName = dbFilePath_.substr(lastSlash + 1);
        outputPath += dbName + ".xlsx";
    }

    // Ensure the output directory exists, if not, create it
    std::filesystem::path outputDir = std::filesystem::path(outputPath).parent_path();
    if (!std::filesystem::exists(outputDir))
        std::filesystem::create_directories(outputDir);

    // Create format for header
    libxl::Format* headerFormat = book->addFormat();
    libxl::Font* headerFont = book->addFont();
    headerFont->setBold(true);
    headerFormat->setFont(headerFont);
    headerFormat->setAlignH(libxl::AlignH::ALIGNH_CENTER);
    headerFormat->setAlignV(libxl::AlignV::ALIGNV_CENTER);
    book->setRgbMode(true);
    libxl::Color headerColor = book->colorPack(color_value_header, color_value_header, color_value_header);
    headerFormat->setFillPattern(libxl::FILLPATTERN_SOLID);
    headerFormat->setPatternForegroundColor(headerColor);
    headerFormat->setBorder(libxl::BORDERSTYLE_THIN);

    // Create format for content
    libxl::Color content_color0 = book->colorPack(color_value_content_base, color_value_content_base, color_value_content_base); // White color for odd rows
    libxl::Color content_color1 = book->colorPack(color_value_content_layer, color_value_content_layer, color_value_content_layer); // Light gray color for even rows

    libxl::Format* content_format0 = book->addFormat();
    libxl::Font* content_font0 = book->addFont();
    content_font0->setBold(false);
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
    SQLite::Statement tablesQuery(*database_, "SELECT name FROM sqlite_master WHERE type='table'");
    while (tablesQuery.executeStep()) {
        std::string tableName = tablesQuery.getColumn(0).getText();

        libxl::Sheet* sheet = book->addSheet(utf8ToWide(tableName).c_str());
        if (!sheet) {
            book->release();
            closeDatabase();
            return false;
        }

        // Get column names and data types
        std::vector<std::pair<std::string, std::string>> columnInfo;
        SQLite::Statement columnsQuery(*database_, "PRAGMA table_info(" + tableName + ")");
        while (columnsQuery.executeStep()) {
            std::string columnName = columnsQuery.getColumn(1).getText();
            std::string dataType = columnsQuery.getColumn(2).getText();
            columnInfo.push_back({ columnName, dataType });
        }

        // Write column names as header with format
        for (size_t i = 0; i < columnInfo.size(); ++i) {
            sheet->writeStr(0, i, utf8ToWide(columnInfo[i].first).c_str(), headerFormat);
            sheet->setCol(i, i, 20); // Set column width
        }

        // Fetch and write data
        SQLite::Statement dataQuery(*database_, "SELECT * FROM " + tableName);
        uint64_t row = 1;
        libxl::Format* numFormat = book->addFormat();
        while (dataQuery.executeStep()) {
            libxl::Format* currentFormat = (row % 2 == 0) ? content_format1 : content_format0; // Alternate row colors
            for (size_t i = 0; i < columnInfo.size(); ++i) {
                std::string data = dataQuery.getColumn(i).getText();
                if (columnInfo[i].second == "INTEGER") {
                    int intValue = std::stoi(data);
                    sheet->writeNum(row, i, intValue, currentFormat);
                }
                else if (columnInfo[i].second == "REAL") {
                    double numericValue = std::stod(data);
                    numFormat->setNumFormat(libxl::NUMFORMAT_NUMBER_SEP_D2); // Set the format for numeric columns
                    sheet->setCol(i, i, 20, numFormat);
                    sheet->writeNum(row, i, numericValue, currentFormat);
                }
                else {
                    sheet->writeStr(row, i, utf8ToWide(data).c_str(), currentFormat);
                }
            }
            ++row;
        }
    }

    // Save workbook to file
    bool success = book->save(utf8ToWide(outputPath).c_str());
    book->release();
    closeDatabase();
    return success;
}


bool hinge_framework::DatabaseHandler::openDatabase() {
    try {
        database_ = new SQLite::Database(dbFilePath_.c_str(), SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
        database_->key(key_.c_str()); // Set encryption key
        return true;
    }
    catch (std::exception& e) {
        // Handle exception (e.g., file not found, encryption key incorrect)
        return false;
    }
}

void hinge_framework::DatabaseHandler::closeDatabase() {
    if (database_) {
        delete database_;
        database_ = nullptr;
    }
}
