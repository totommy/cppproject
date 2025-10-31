#ifndef FILE_HPP
#define FILE_HPP

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

class File {
private:
    fstream file_stream;
    string name;
    // int create_time = static_cast<int>(time(nullptr));
    std::chrono::system_clock::time_point create_time;
    int access_times = 0;
    friend ostream& operator<<(ostream& os, const File& file) {
        os << "name = " << file.getName() << ",";
        os << "create_time = " << file.getCreateTimeString() << ",";
        os << "access_time = " << file.getAccessTimes() << endl;
        return os;
    }
public:
    // constructor: 改為 by-value name，然後 move
    File(fstream&& file_stream, string name) :
        file_stream(std::move(file_stream)), name(std::move(name)), create_time(std::chrono::system_clock::now()) {}
    // deconstructor RASII
    ~File() {
        if (file_stream.is_open()) {
            file_stream.close();
        }
    }
    // because fstream exists in File 
    // we need File class supports move constructor and move assignment
    // No copy constructor and no copy assignment
    File(const File&) = delete; // no copy constructor
    File& operator=(const File&) = delete; // no copy assignment
    File(File&&) = default; // move constructor
    File& operator=(File&&) = default; // move assignment
    fstream& getFstream() { return this->file_stream; }


    string getName() const { return this->name; }
    // int getCreateTime() const { return create_time; }
    std::string getCreateTimeString() const {
        auto t = std::chrono::system_clock::to_time_t(create_time);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      create_time.time_since_epoch()) % 1000;
        std::stringstream ss;
        ss << std::put_time(std::localtime(&t), "%F %T")
           << "." << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }
    int getAccessTimes() const { return access_times; }
};


#endif