#define _WIN32_WINNT 0x0A00  // Windows 10 以上版本
#include <iostream>
#include <sstream>
#include <fstream>
// #include <ctime>
#include <chrono> // 高精度時間
#include <filesystem>
#include <unordered_map>
#include <list>
#include <mutex> 
#include "../include/UserInterface.hpp"

using namespace std;


int main(void) {
    User& user = User::getInstance();

    httplib::Server svr;

    // CORS: 允許前端 fetch
    svr.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type"}
    });

   // --- CREATE FILE ---
    svr.Post("/file/create", [&](const httplib::Request& req, httplib::Response& res) {
        auto body_json = json::parse(req.body);
        string filename = body_json["name"];
        
        // 用來測試多線程
        // for (int i = 1; i <= 5; i++) {
        //     cout << "add task " << i << endl;
        //     string name = filename + to_string(i);
        //     user.submitTask("CREATE", name); // 傳值
        //     this_thread::sleep_for(std::chrono::milliseconds(1000));
        // }

        user.submitTask("CREATE", filename);
        json resp;
        resp["status"] = "ok";
        resp["message"] = "tasks submitted";
        res.set_content(resp.dump(), "application/json");
    });

    svr.Delete("/file/delete", [&](const httplib::Request& req, httplib::Response& res) {
        auto body_json = json::parse(req.body);
        string filename = body_json["name"];
        
        // 用來測試多線程
        // for (int i = 1; i <= 5; i++) {
        //     cout << "add task " << i << endl;
        //     string name = filename + to_string(i);
        //     user.submitTask("DELETE", name); // 傳值
        //     this_thread::sleep_for(std::chrono::milliseconds(1000));
        // }

        user.submitTask("DELETE", filename);
        json resp;
        resp["status"] = "ok";
        resp["message"] = "tasks submitted";
        res.set_content(resp.dump(), "application/json");
    });


    svr.Post("/file/printLRU", [&](const httplib::Request& req, httplib::Response& res) {
        // 用來測試多線程
        json myjson = user.printTopN();  // ✅ 同步
        res.set_content(myjson.dump(4), "application/json");

    });

    svr.Post("/file/printAll", [&](const httplib::Request& req, httplib::Response& res) {
        // 用來測試多線程
        json myjson = user.printAll();  // ✅ 同步
        res.set_content(myjson.dump(4), "application/json");

    });

    svr.Get("/file/list", [&](const httplib::Request& req, httplib::Response& res) {
        json resp = user.getLRUStatus();
        res.set_content(resp.dump(4), "application/json");
    });


    // C++17 兼容的 ends_with
    auto ends_with = [](const std::string& str, const std::string& suffix) {
        return str.size() >= suffix.size() &&
            str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
    };


    // 靜態檔案
    svr.Get(R"(/(.*))", [ends_with](const httplib::Request& req, httplib::Response& res) {
        std::string filename = req.matches[1].str();
        std::string path;

        if (filename.empty()) {
            path = "../src/test.html"; // 預設首頁
        } else {
            path = "../src/" + filename;  // JS/CSS 也在 src/
        }

        std::ifstream ifs(path, std::ios::binary);
        if (!ifs) {
            res.status = 404;
            res.set_content("Not Found", "text/plain");
            return;
        }

        std::stringstream buffer;
        buffer << ifs.rdbuf();
        std::string content = buffer.str();

        // 判斷 Content-Type
        if (ends_with(path, ".js"))
            res.set_content(content, "application/javascript");
        else if (ends_with(path, ".css"))
            res.set_content(content, "text/css");
        else
            res.set_content(content, "text/html");
    });

    std::cout << "Server listening on port 8080...\n";
    svr.listen("localhost", 8080);

    return 0;
}
