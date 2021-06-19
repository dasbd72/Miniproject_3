/**
 * @file LOG.h
 * @author Me (https://hackmd.io/zTTsOA9pRZGhzlqwiIAi8A)
 * @version 0.1
 * @date 2021-06-20
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef DEBUGLOG
#define DEBUGLOG
#include <bits/stdc++.h>

struct LOCALTIME {
    static std::chrono::_V2::system_clock::time_point start;
    static double get_duration() {
        return std::chrono::duration<double>(std::chrono::system_clock::now() - start).count();
    }
    static void initialize() {
        start = std::chrono::system_clock::now();
    }
};
std::chrono::_V2::system_clock::time_point LOCALTIME::start;

class LOG {
   public:
    enum TYPE {
        INFO,
        ERROR,
        NONE
    };
    static std::ofstream fout;
    TYPE type;
    LOG(TYPE type = NONE) : type(type) {}
    ~LOG() {}
    template <class T>
    LOG& operator<<(const T& msg) {
        switch (this->type) {
            case INFO:
                fout << "[INFO]: ";
                break;
            case ERROR:
                fout << "[ERROR]: ";
                break;
            default:
                break;
        }
        this->fout << msg;
        this->fout.flush();
        return *this;
    }
    static void initialize() {
        fout.open("debuglog.txt", std::ofstream::ate | std::ofstream::app);
    }
    static void terminate() {
        fout.close();
    }
};
std::ofstream LOG::fout;

#endif