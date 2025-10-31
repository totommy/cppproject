# C++ HTTP Server Project

A modern **C++ HTTP server** built with [`cpp-httplib`](https://github.com/yhirose/cpp-httplib), featuring an **LRU cache**, **ThreadPool**, and **RESTful API** architecture.

This project is designed to demonstrate multi-threaded request handling, efficient caching, and modular C++ server design — suitable for system programming or backend-oriented resumes.

---

## 🚀 Features
- Lightweight HTTP server (based on cpp-httplib)
- ThreadPool for concurrent request processing
- LRU cache for optimized memory usage
- RESTful API for CRUD operations
- Modular design for scalability and maintainability

---

## 🛠️ Build Instructions
```bash
# Clone repository
git clone https://github.com/totommy/cppproject.git
cd cppproject

# Build with CMake + Ninja
mkdir build && cd build
cmake .. -G Ninja
ninja

# Run server
./server
