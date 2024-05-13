// HingeFramework_DLLTEST.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <SQLiteCpp/SQLiteCpp.h>
#include <HingeFramework/Hash.h>
#include <HingeFramework/RsaCipher.h>
#include <HingeFramework/Aes256Cipher.h>
#include <HingeFramework/DatabaseHandler.h>

// 定义学生类
class Student {
private:
    int id;
    std::string name;
    double score;

public:
    Student(int id, const std::string& name, double score) : id(id), name(name), score(score) {}

    int getId() const { return this->id; }
    std::string getName() const { return this->name; }
    double getScore() const { return this->score; }
};

// 数据库操作类
class Database {
private:
    SQLite::Database* db;
    hinge_framework::DatabaseHandler* handler;

public:
    Database(const std::string& dbName, const std::string& encryptionKey) {
        db = new SQLite::Database(dbName.c_str(), SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
        db->key(encryptionKey.c_str());
        handler = new hinge_framework::DatabaseHandler(dbName, encryptionKey);
    }

    ~Database() {
        if (this->db) {
            delete this->db;
            this->db = nullptr;
        }
        if (this->handler) {
            delete this->handler;
            this->handler = nullptr;
        }
    }

    void createTable() {
        try {
            db->exec("CREATE TABLE IF NOT EXISTS students (id INTEGER PRIMARY KEY, name TEXT, score REAL)");
            db->exec("CREATE TABLE IF NOT EXISTS TESTING_TABLE (TESTING_HEADER0 INTEGER PRIMARY KEY, TESTING_HEADER1 TEXT, TESTING_HEADER2 REAL)");
        }
        catch (const std::exception e) {
            std::cerr << "捕获到异常: " << e.what() << std::endl;
        }
    }

    void insertStudent(const Student& student) {
        try {
            SQLite::Statement query(*db, "INSERT INTO students (id, name, score) VALUES (?, ?, ?)");
            query.bind(1, student.getId());
            query.bind(2, student.getName());
            query.bind(3, student.getScore());
            query.exec();
        }
        catch (const std::exception& e) {
            std::cerr << "捕获到异常: " << e.what() << std::endl;
        }
    }

    void deleteStudent(int id) {
        SQLite::Statement query(*db, "DELETE FROM students WHERE id = ?");
        query.bind(1, id);
        query.exec();
    }

    void updateStudent(const Student& student) {
        SQLite::Statement query(*db, "UPDATE students SET name = ?, score = ? WHERE id = ?");
        query.bind(1, student.getName());
        query.bind(2, student.getScore());
        query.bind(3, student.getId());
        query.exec();
    }

    Student getStudent(int id) {
        SQLite::Statement query(*db, "SELECT * FROM students WHERE id = ?");
        query.bind(1, id);
        if (query.executeStep()) {
            int studentId = query.getColumn(0).getInt();
            std::string name = query.getColumn(1).getString();
            double score = query.getColumn(2).getDouble();
            return Student(studentId, name, score);
        }
        throw std::runtime_error("Student not found");
    }

    std::vector<Student> getAllStudents() {
        std::vector<Student> allStudents;
        SQLite::Statement query(*db, "SELECT * FROM students");
        //SQLite::Statement query(*db, "SELECT * FROM TESTING_TABLE");

        while (query.executeStep()) {
            int id = query.getColumn(0).getInt();
            std::string name = query.getColumn(1).getString();
            double score = query.getColumn(2).getDouble();
            allStudents.push_back(Student(id, name, score));
        }

        return allStudents;
    }

    bool exportToExcel(const char*& output_path) {
        return handler->exportToExcel(output_path, 96, 212);
    }

    bool importFromExcel(const char*& input_path) {
        return handler->importFromExcel(input_path);
    }
};

void showMainMenu() {
    std::cout << "----- Main Menu -----" << std::endl;
    std::cout << "1. Add Student" << std::endl;
    std::cout << "2. Delete Student" << std::endl;
    std::cout << "3. Update Student" << std::endl;
    std::cout << "4. Search Student" << std::endl;
    std::cout << "5. Show All Students" << std::endl;
    std::cout << "6. Import from Excel" << std::endl;
    std::cout << "7. Export to Excel" << std::endl;
    std::cout << "8. Exit" << std::endl;
}

std::string centerAlign(const std::string& text, uint16_t width) {
    std::stringstream ss;
    int padding = width - text.length();
    if (padding > 0) {
        int leftPadding = padding / 2;
        int rightPadding = padding - leftPadding;
        ss << std::setw(leftPadding) << "" << text << std::setw(rightPadding) << "";
    }
    else {
        ss << text;
    }
    return ss.str();
}

int32_t task0() {
    const char* const KEY_FILE_PATH = "./assets-test/data/keys.ent";
    const char* const DB_FILE_PATH = "./assets-test/data/scores.dat";
    const char* const KEY_ID0 = "2898db5e-c31b-4837-946d-d4fc3f02ef09";

    const char* const INPUT_PATH = "./assets-test/input/scores.xlsx";
    const char* const OUTPUT_PATH = "./assets-test/output/scores.xlsx";

    hinge_framework::Aes256Cipher aes256;
    hinge_framework::RsaCipher rsa;
    hinge_framework::Key aes256_key0;

    try {
        if (!aes256.isKeyExists(KEY_ID0, KEY_FILE_PATH)) {
            aes256_key0 = aes256.generateKey(KEY_ID0);
            if (aes256.writeKeyToFile(aes256_key0, KEY_FILE_PATH)) {
                std::cout << "文件创建并写入成功！" << std::endl;
            }
            else {
                std::cout << "文件创建并写入失败！" << std::endl;
            }
        }
    }
    catch (const std::exception e) {
        std::cerr << "捕获到异常: " << e.what() << std::endl;
    }

    aes256_key0 = aes256.readKeyFromFile(KEY_ID0, KEY_FILE_PATH);

    std::cout << "Key ID: " << aes256_key0.id_ << "\n"
        << "Key: " << aes256_key0.key_ << "\n\n" << std::endl;

    std::cout << "Key ID: " << hinge_framework::hashPlaintext(aes256_key0.id_.c_str()) << "\n"
        << "Key: " << hinge_framework::hashPlaintext(aes256_key0.key_.c_str()) << "\n\n" << std::endl;

    Database db(DB_FILE_PATH, aes256_key0.key_);
    db.createTable();

    int16_t choice;
    do {
        showMainMenu();
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        system("cls");
        switch (choice) {
        case 1: {
            int id;
            std::string name;
            float score;
            std::cout << "Enter student ID: ";
            std::cin >> id;
            std::cout << "Enter student name: ";
            std::cin.ignore();
            std::getline(std::cin, name);
            std::cout << "Enter student score: ";
            std::cin >> score;

            // 记录开始时间点
            auto start = std::chrono::high_resolution_clock::now();

            db.insertStudent(Student(id, name, score));

            // 记录结束时间点
            auto end = std::chrono::high_resolution_clock::now();

            // 计算执行时间
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

            std::cout << "\nThe student info has been added successfully!\n" << std::endl;

            // 输出执行时间
            std::cout << "\nExecution time: " << duration.count() << " ms\n" << std::endl;

            system("pause");
            system("cls");
            break;
        }
        case 2: {
            int id;
            std::cout << "Enter student ID to delete: ";
            std::cin >> id;

            // 记录开始时间点
            auto start = std::chrono::high_resolution_clock::now();

            db.deleteStudent(id);

            // 记录结束时间点
            auto end = std::chrono::high_resolution_clock::now();

            // 计算执行时间
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

            std::cout << "\nThe student info has been deleted successfully!\n" << std::endl;

            // 输出执行时间
            std::cout << "\nExecution time: " << duration.count() << " ms\n" << std::endl;

            system("pause");
            system("cls");
            break;
        }
        case 3: {
            int id;
            std::string name;
            float score;
            std::cout << "Enter student ID to update: ";
            std::cin >> id;
            std::cout << "Enter new student name: ";
            std::cin.ignore();
            std::getline(std::cin, name);
            std::cout << "Enter new student score: ";
            std::cin >> score;

            // 记录开始时间点
            auto start = std::chrono::high_resolution_clock::now();

            db.updateStudent(Student(id, name, score));

            // 记录结束时间点
            auto end = std::chrono::high_resolution_clock::now();

            // 计算执行时间
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

            std::cout << "\nThe student info has been updated successfully!\n" << std::endl;

            // 输出执行时间
            std::cout << "\nExecution time: " << duration.count() << " ms\n" << std::endl;

            system("pause");
            system("cls");
            break;
        }
        case 4: {
            int id;
            std::cout << "Enter student ID to search: ";
            std::cin >> id;

            // 记录开始时间点
            auto start = std::chrono::high_resolution_clock::now();

            try {
                Student student = db.getStudent(id);
                std::cout << "Student found: ID = " << student.getId() << ", Name = " << student.getName() << ", Score = " << student.getScore() << std::endl;
            }
            catch (const std::runtime_error& e) {
                std::cerr << e.what() << std::endl;
            }

            // 记录结束时间点
            auto end = std::chrono::high_resolution_clock::now();

            // 计算执行时间
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

            // 输出执行时间
            std::cout << "\nExecution time: " << duration.count() << " ms\n" << std::endl;

            system("pause");
            system("cls");
            break;
        }
        case 5: {
            // 记录开始时间点
            auto start = std::chrono::high_resolution_clock::now();

            const uint16_t LINE_LENGTH = 48;

            std::vector<Student> allStudents = db.getAllStudents();

            std::ostringstream output;

            output << std::setfill('-') << std::setw(LINE_LENGTH) << "" << std::setfill(' ') << std::endl;
            output << "| " << centerAlign("ID", 8) << " | " << centerAlign("Name", 24) << " | " << centerAlign("Score", 6) << " |" << std::endl;
            output << std::setfill('-') << std::setw(LINE_LENGTH) << "" << std::setfill(' ') << std::endl;

            for (const auto& student : allStudents) {
                output << "| " << std::right << std::setw(8) << student.getId() << " | "
                    << std::left << std::setw(24) << student.getName() << " | "
                    << std::right << std::setw(6) << student.getScore() << " |" << std::endl;
            }

            output << std::setfill('-') << std::setw(LINE_LENGTH) << "" << std::setfill(' ') << std::endl;

            // 记录结束时间点
            auto end = std::chrono::high_resolution_clock::now();

            // 计算执行时间
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

            std::cout << output.str() << std::endl;

            // 输出执行时间
            std::cout << "Execution time: " << duration.count() << " ms" << std::endl;

            system("pause");
            system("cls");
            break;
        }
        case 6: {
            // 记录开始时间点
            auto start = std::chrono::high_resolution_clock::now();

            // Import database form Excel file
            const char* input_path = INPUT_PATH;
            try {
                if (db.importFromExcel(input_path)) {
                    // 记录结束时间点
                    auto end = std::chrono::high_resolution_clock::now();

                    // 计算执行时间
                    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

                    std::cout << "Database imported successfully from: " << input_path << std::endl;

                    // 输出执行时间
                    std::cout << "\nExecution time: " << duration.count() << " ms\n" << std::endl;
                }
                else {
                    // 记录结束时间点
                    auto end = std::chrono::high_resolution_clock::now();

                    // 计算执行时间
                    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

                    std::cerr << "Error exporting database." << std::endl;

                    // 输出执行时间
                    std::cout << "\nExecution time: " << duration.count() << " ms\n" << std::endl;
                }
            }
            catch (const std::exception& e) {
                std::cerr << "An exception occurred while importing database: " << e.what() << std::endl;
            }

            system("pause");
            system("cls");
            break;
        }
        case 7: {

            // 记录开始时间点
            auto start = std::chrono::high_resolution_clock::now();

            // Export database to Excel file
            const char* output_path = OUTPUT_PATH;
            if (db.exportToExcel(output_path)) {
                // 记录结束时间点
                auto end = std::chrono::high_resolution_clock::now();

                // 计算执行时间
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

                std::cout << "Database exported successfully to: " << output_path << std::endl;

                // 输出执行时间
                std::cout << "\nExecution time: " << duration.count() << " ms\n" << std::endl;
            }
            else {
                // 记录结束时间点
                auto end = std::chrono::high_resolution_clock::now();

                // 计算执行时间
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

                std::cerr << "Error exporting database." << std::endl;

                // 输出执行时间
                std::cout << "\nExecution time: " << duration.count() << " ms\n" << std::endl;
            }

            system("pause");
            system("cls");
            break;
        }
        case 8:
            std::cout << "Exiting program..." << std::endl;
            break;
        default:
            std::cout << "Invalid choice. Please enter a number between 1 and 5." << std::endl;
            system("pause");
            system("cls");
        }
    } while (choice != 8);

    return 0;
}

int32_t task1() {
    const char* const KEY_FILE_PATH = "./assets-test/data/keys.ent";
    const char* const DB_FILE_PATH = "./assets-test/data/scores.dat";
    const char* const KEY_ID0 = "2898db5e-c31b-4837-946d-d4fc3f02ef09";

    const char* const ORIGIN_FILE_PATH = "./assets-test/origin/";
    const char* const ENCRYPTED_FILE_PATH = "./assets-test/encrypted/";
    const char* const DECRYPTED_FILE_PATH = "./assets-test/decrypted/";

    hinge_framework::Aes256Cipher aes256;
    hinge_framework::RsaCipher rsa;
    hinge_framework::Key aes256_key0;

    try {
        if (!aes256.isKeyExists(KEY_ID0, KEY_FILE_PATH)) {
            aes256_key0 = aes256.generateKey(KEY_ID0);
            if (aes256.writeKeyToFile(aes256_key0, KEY_FILE_PATH)) {
                std::cout << "文件创建并写入成功！" << std::endl;
            }
            else {
                std::cout << "文件创建并写入失败！" << std::endl;
            }
        }
    }
    catch (const std::exception e) {
        std::cerr << "捕获到异常: " << e.what() << std::endl;
    }

    aes256_key0 = aes256.readKeyFromFile(KEY_ID0, KEY_FILE_PATH);

    std::cout << "Key ID: " << aes256_key0.id_ << "\n"
        << "Key: " << aes256_key0.key_ << "\n\n" << std::endl;

    aes256.encryptDirectory(aes256_key0.key_, ORIGIN_FILE_PATH, ENCRYPTED_FILE_PATH);
    aes256.decryptDirectory(aes256_key0.key_, ENCRYPTED_FILE_PATH, DECRYPTED_FILE_PATH);

    return 0;
}

int32_t task2() {
    std::string input;

    while (true) {
        std::cout << "Enter your input (type 'e' to quit): \n";
        std::getline(std::cin, input);

        if (input == "e") {
            std::cout << "\nExiting the program...\n" << std::endl;
            break;
        }

        const char* hashed_value = hinge_framework::hashPlaintext(input.c_str());
        std::cout << "\nHashed value of '" << input << "': \n" 
            << hashed_value << "\n" 
            << std::endl;

        system("pause");
        system("cls");
    }

    return 0;
}

int32_t main(int32_t argc, char* argv[]) {
    const int32_t main_return = task2();

    system("pause");
    return main_return;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
