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
    class HINGE_API DatabaseHandler {
    public:
        DatabaseHandler(const std::string& dbFilePath, const std::string& key);
        ~DatabaseHandler();

        bool exportToExcel(std::string& outputPath, const uint16_t color_value_header, const uint16_t color_value_content_layer, const uint16_t color_value_content_base = 255);

    private:
        std::wstring utf8ToWide(const std::string& utf8Str);

        bool openDatabase();
        void closeDatabase();

        std::string dbFilePath_;
        std::string key_;
        SQLite::Database* database_;
    };
}

#endif // DATABASE_HANDLER_H
