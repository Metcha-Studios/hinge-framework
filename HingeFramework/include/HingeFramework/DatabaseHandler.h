#pragma once
#pragma warning(disable:4996)
#ifndef DATABASE_HANDLER_H
#define DATABASE_HANDLER_H

#ifndef HINGE_API

#ifdef HINGE_EXPORTS
#define HINGE_API __declspec(dllexport)
#else
#define HINGE_API __declspec(dllimport)
#endif

#endif // HINGE_API

#include <string>
#include <vector>
#include <utf8.h>
#include <sqlite3.h>
#include <SQLiteCpp/SQLiteCpp.h>
#include <libxl/libxl.h>

namespace hinge_framework {
    struct ColumnInfo {
        std::string name;
        std::string type;
    };

    class HINGE_API DatabaseHandler {
    public:
        DatabaseHandler(const std::string& dbFilePath, const std::string& key);
        ~DatabaseHandler();

        bool tableExists(const std::string& table_name);
        bool clearTable(const std::string& table_name);
        bool importFromExcel(const char*& input_path);
        bool exportToExcel(const char*& output_path, const uint16_t color_value_header = 96, const uint16_t color_value_content_layer = 212, const uint16_t color_value_content_base = 255);

    private:
        std::string db_file_path_;
        std::string key_;
        SQLite::Database* database_;

        bool openDatabase();
        void closeDatabase();
        std::wstring utf8ToWide(const std::string& utf8Str);
        std::string wideToUtf8(const std::wstring& wide_str);
        std::vector<ColumnInfo> getColumnInfo(const std::string& table_name);
        void setRowHeightAndColumnWidth(libxl::Sheet* sheet, uint16_t row_height, const std::vector<hinge_framework::ColumnInfo>& column_info);
        bool createTable(const std::string& table_name, const std::vector<ColumnInfo>& columns);
        void insertData(const std::string& table_name, const std::vector<ColumnInfo>& columns, libxl::Sheet* sheet);
        bool columnExists(const std::string& table_name, const std::string& column_name);
    };
}

#endif // DATABASE_HANDLER_H
