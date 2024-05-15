// HingeFramework_DLLTEST.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <conio.h>
#include <SQLiteCpp/SQLiteCpp.h>
#include <HingeFramework/Hash.h>
#include <HingeFramework/Base64.h>
#include <HingeFramework/RsaCipher.h>
#include <HingeFramework/Aes256Cipher.h>
#include <HingeFramework/DatabaseHandler.h>

constexpr auto KEY_FILE_PATH = "./assets-test/data/keys.ent";
constexpr auto HASH_FILE_PATH = "./assets-test/data/hash.dat";
constexpr auto DB_FILE_PATH = "./assets-test/data/scores.dat";

// 用户类
class User {
private:
    std::string username;
    std::string password;

public:
    User(const std::string& username, const std::string& password) : username(username), password(password) {}

    std::string getUsername() const { return this->username; }
    std::string getPassword() const { return this->password; }
};

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
    hinge_framework::Key aes256_key_;

public:
    Database(const std::string& dbName, const std::string& encryptionKey) {
        db = new SQLite::Database(dbName.c_str(), SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
        db->key(encryptionKey.c_str());
        handler = new hinge_framework::DatabaseHandler(dbName, encryptionKey);

        const char* const KEY_ID = "8187b205-29a1-48ff-b73c-d9ff65ea7d9b";

        hinge_framework::Aes256Cipher aes256;
        hinge_framework::RsaCipher rsa;
        hinge_framework::Key aes256_key;

        try {
            if (!aes256.isKeyExists(KEY_ID, KEY_FILE_PATH)) {
                aes256_key = aes256.generateKey(KEY_ID);
                aes256.writeKeyToFile(aes256_key, KEY_FILE_PATH);
            }
        }
        catch (const std::exception e) {
            std::cerr << "Capture to exception: " << e.what() << std::endl;
        }

        aes256_key = aes256.readKeyFromFile(KEY_ID, KEY_FILE_PATH);

        this->aes256_key_ = aes256_key;
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
            db->exec("CREATE TABLE IF NOT EXISTS users (username TEXT PRIMARY KEY, password TEXT)");
            db->exec("CREATE TABLE IF NOT EXISTS students (id INTEGER PRIMARY KEY, name TEXT, score REAL)");
            db->exec("CREATE TABLE IF NOT EXISTS TESTING_TABLE (TESTING_HEADER0 INTEGER PRIMARY KEY, TESTING_HEADER1 TEXT, TESTING_HEADER2 REAL)");
        }
        catch (const std::exception e) {
            std::cerr << "捕获到异常: " << e.what() << std::endl;
        }
    }

    // 存储用户信息
    bool storeUser(const User& user) {
        try {
            // Check if the user already exists
            SQLite::Statement existQuery(*db, "SELECT COUNT(*) FROM users WHERE username = ?");
            existQuery.bind(1, user.getUsername());
            if (existQuery.executeStep()) {
                int count = existQuery.getColumn(0).getInt();
                if (count > 0) {
                    std::cout << "User already exists." << std::endl;
                    return false; // User already exists, return false
                }
            }

            // User does not exist, insert the user
            const hinge_framework::Hash hashedPassword = hinge_framework::sha3_256(user.getPassword().c_str());
            SQLite::Statement query(*db, "INSERT INTO users (username, password) VALUES (?, ?)");
            query.bind(1, user.getUsername());
            query.bind(2, hashedPassword.hash);
            query.exec();

            // Store the hashed password
            hinge_framework::storeHash(hashedPassword, HASH_FILE_PATH, this->aes256_key_.key_.c_str());

            return true; // Insertion successful
        }
        catch (const std::exception& e) {
            std::cerr << "捕获到异常: " << e.what() << std::endl;
            return false; // Insertion failed
        }
    }

    // 根据用户名获取用户信息
    User getUser(const std::string& username) {
        //const hinge_framework::Hash* hashedUsername = hinge_framework::retrieveHash(username.c_str(), HASH_FILE_PATH, this->aes256_key_.key_.c_str());
        SQLite::Statement query(*db, "SELECT * FROM users WHERE username = ?");
        query.bind(1, username);
        if (query.executeStep()) {
            std::string username = query.getColumn(0).getText();
            std::string password = query.getColumn(1).getText();
            return User(username, password);
        }
        throw std::runtime_error("User not found");
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

// 密码输入
void passwordInput(std::string& password) {
    char ch;
    while ((ch = getch()) != '\r') {
        if (ch == '\b') {
            if (!password.empty()) {
                std::cout << "\b \b";
                password.pop_back();
            }
        }
        else {
            password.push_back(ch);
            std::cout << '*';
        }
    }
    std::cout << std::endl;

    return;
}

// 登录功能
bool login(Database& db) {
    std::string username, password;

    std::cout << "Enter username: ";
    std::cin >> username;
    std::cout << "Enter password: ";
    passwordInput(password);

    const char* const KEY_ID = "8187b205-29a1-48ff-b73c-d9ff65ea7d9b";

    hinge_framework::Aes256Cipher aes256;
    hinge_framework::RsaCipher rsa;
    hinge_framework::Key aes256_key;

    try {
        if (!aes256.isKeyExists(KEY_ID, KEY_FILE_PATH)) {
            aes256_key = aes256.generateKey(KEY_ID);
            aes256.writeKeyToFile(aes256_key, KEY_FILE_PATH);
        }
    }
    catch (const std::exception e) {
        std::cerr << "Capture to exception: " << e.what() << std::endl;
    }

    aes256_key = aes256.readKeyFromFile(KEY_ID, KEY_FILE_PATH);

    try {
        // 根据用户名获取用户信息
        User user = db.getUser(username);
        // 对比密码哈希值是否匹配
        hinge_framework::Hash* hash = hinge_framework::retrieveHash(user.getPassword().c_str(), HASH_FILE_PATH, aes256_key.key_.c_str());
        if (hash && hinge_framework::compareHashes(password.c_str(), *hinge_framework::retrieveHash(user.getPassword().c_str(), HASH_FILE_PATH, aes256_key.key_.c_str()))) {
            return true;
        }
        else {
            return false;
        }
    }
    catch (const std::exception& e) {
        //std::cerr << "User not found." << std::endl;
        return false;
    }
}

// 初始化超管用户名和密码
void initializeSuperAdmin(Database& db) {
    //// 初始化超管用户名和密码的哈希值
    //hinge_framework::Hash usernameHash = { "6ac98c7ea5fc1d47159b410f49e7cc63898d8a8465a0ed66873fff0986694442", "a9mskT8lzOJFgBZxrfABfBA=" };
    //hinge_framework::Hash passwordHash = { "9f91fa50d7f267629ce8a1396d5e97b1e8d993a2e4347a1ccdb5c77d3198352f", "a2sp0meqgupXmg9RvkqSaNjM62D3fw==" };

    //// 存储超管用户名和密码的哈希值
    //hinge_framework::storeHash(&usernameHash, "./assets-test/data/hash.db", "username");
    //hinge_framework::storeHash(&passwordHash, "./assets-test/data/hash.db", "password");

    // 创建超管用户对象并存储
    User superAdmin("SUPERADMIN", "bugaosuni");
    db.storeUser(superAdmin);
}

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

static int32_t task0() {
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
                std::cout << "File creation and writing successfully!" << std::endl;
            }
            else {
                std::cerr << "File creation and writing failed!" << std::endl;
            }
        }
    }
    catch (const std::exception e) {
        std::cerr << "Capture to exception: " << e.what() << std::endl;
    }

    aes256_key0 = aes256.readKeyFromFile(KEY_ID0, KEY_FILE_PATH);

    std::cout << "Key ID: " << aes256_key0.id_ << "\n"
        << "Key: " << aes256_key0.key_ << "\n\n" << std::endl;

    std::cout << "Key ID: " << hinge_framework::sha3_256(aes256_key0.id_.c_str()).hash << "\n"
        << "Key: " << hinge_framework::sha3_256(aes256_key0.key_.c_str()).hash << "\n\n" << std::endl;

    system("pause");
    system("cls");

    Database db(DB_FILE_PATH, aes256_key0.key_);
    db.createTable();

    // 初始化超管用户名和密码
    initializeSuperAdmin(db);

    bool loggedIn = false;
    while (!loggedIn) {
        loggedIn = login(db);
        if (!loggedIn) {
            system("cls");
            std::cout << "Login failed: Invalid username or password, please try again.\n" << std::endl;
            system("pause");
        }
        system("cls");
    }

    system("cls");
    std::cout << "Login successful. Welcome!\n" << std::endl;
    system("pause");
    system("cls");

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
            const char* input_path = INPUT_PATH;

            // 记录开始时间点
            auto start = std::chrono::high_resolution_clock::now();

            // Import database form Excel file
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
            const char* const KEY_ID = "8187b205-29a1-48ff-b73c-d9ff65ea7d9b";
            hinge_framework::Key hash_key = aes256.readKeyFromFile(KEY_ID, KEY_FILE_PATH);
            const char* output_path = OUTPUT_PATH;
            const char* hash_output_path = "./assets-test/output/hashes.xlsx";
            Database hash_db(HASH_FILE_PATH, hash_key.key_);

            // 记录开始时间点
            auto start = std::chrono::high_resolution_clock::now();

            // Export database to Excel file
            if (db.exportToExcel(output_path) && hash_db.exportToExcel(hash_output_path)) {
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

static int32_t task1() {
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
                std::cout << "File creation and writing successfully!" << std::endl;
            }
            else {
                std::cerr << "File creation and writing failed!" << std::endl;
            }
        }
    }
    catch (const std::exception e) {
        std::cerr << "Capture to exception: " << e.what() << std::endl;
    }

    aes256_key0 = aes256.readKeyFromFile(KEY_ID0, KEY_FILE_PATH);

    std::cout << "Key ID: " << aes256_key0.id_ << "\n"
        << "Key: " << aes256_key0.key_ << "\n\n" << std::endl;

    aes256.encryptDirectory(aes256_key0.key_, ORIGIN_FILE_PATH, ENCRYPTED_FILE_PATH);
    aes256.decryptDirectory(aes256_key0.key_, ENCRYPTED_FILE_PATH, DECRYPTED_FILE_PATH);

    return 0;
}

static int32_t task2() {
    std::string input;

    while (true) {
        std::cout << "Enter your input (type 'e' to quit): \n";
        std::getline(std::cin, input);

        if (input == "e") {
            std::cout << "\nExiting the program...\n" << std::endl;
            break;
        }

        const hinge_framework::Hash hashed_value = hinge_framework::sha3_256(input.c_str());
        std::cout << "\nHashed value of '" << input << "': \n"
            << hashed_value.hash << "\n"
            << "\nThe salt of this hash value is: \n"
            << hashed_value.salt << "\n"
            << std::endl;

        system("pause");
        system("cls");
    }

    return 0;
}

int32_t main(int32_t argc, char* argv[]) {
    const int32_t main_return = task0();

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
