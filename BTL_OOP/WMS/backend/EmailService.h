#ifndef EMAILSERVICE_H
#define EMAILSERVICE_H

#include <string>
#include <iostream>

class EmailService {
public:
    static bool guiEmail(const std::string& denEmail, const std::string& tieuDe, const std::string& noiDung) {
        std::cout << "========================================" << std::endl;
        std::cout << "[EMAIL SERVICE] GUI EMAIL DEN: " << denEmail << std::endl;
        std::cout << "TIEU DE: " << tieuDe << std::endl;
        std::cout << "NOI DUNG: " << noiDung << std::endl;
        std::cout << "========================================" << std::endl;
        return true;
    }
};

#endif // EMAILSERVICE_H

