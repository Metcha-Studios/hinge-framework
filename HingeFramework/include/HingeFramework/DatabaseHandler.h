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

        bool exportToExcel(const char*& output_path, const uint16_t color_value_header, const uint16_t color_value_content_layer, const uint16_t color_value_content_base = 255);

    private:
        std::string db_file_path_;
        std::string key_;
        SQLite::Database* database_;

        bool openDatabase();
        void closeDatabase();
        std::wstring utf8ToWide(const std::string& utf8Str);
        std::vector<ColumnInfo> getColumnInfo(const std::string& table_name);
        void setRowHeightAndColumnWidth(libxl::Sheet* sheet, uint16_t row_height, const std::vector<hinge_framework::ColumnInfo>& column_info);
    };
}

#endif // DATABASE_HANDLER_H
