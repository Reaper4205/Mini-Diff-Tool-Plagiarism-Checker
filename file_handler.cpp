#include "file_handler.h"
#include <fstream>
#include <sstream>
#include <sys/stat.h>

std::string FileHandler::cleanLine(const std::string& str) {
    std::string result = str;
    // Remove carriage return if file has Windows CRLF line endings
    while (!result.empty() && (result.back() == '\r' || result.back() == '\n')) {
        result.pop_back();
    }
    return result;
}

bool FileHandler::fileExists(const std::string& filePath) {
    std::ifstream file(filePath);
    return file.good();
}

std::string FileHandler::extractFileName(const std::string& filePath) {
    if (filePath.empty()) return "";
    size_t lastSlash = filePath.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        return filePath.substr(lastSlash + 1);
    }
    return filePath;
}

bool FileHandler::readFile(const std::string& filePath, 
                           std::vector<std::string>& outLines, 
                           std::string& outError) {
    outLines.clear();
    outError.clear();

    if (filePath.empty()) {
        outError = "File path cannot be empty.";
        return false;
    }

    std::ifstream inFile(filePath);
    if (!inFile.is_open()) {
        outError = "Error: Unable to open file '" + filePath + "'. Please check the path and permissions.";
        return false;
    }

    std::string line;
    while (std::getline(inFile, line)) {
        outLines.push_back(cleanLine(line));
    }

    inFile.close();
    return true;
}

bool FileHandler::writeFile(const std::string& filePath, 
                            const std::string& content, 
                            std::string& outError) {
    outError.clear();
    if (filePath.empty()) {
        outError = "Output file path cannot be empty.";
        return false;
    }

    std::ofstream outFile(filePath);
    if (!outFile.is_open()) {
        outError = "Error: Unable to write to file '" + filePath + "'. Check directory permissions.";
        return false;
    }

    outFile << content;
    outFile.close();
    return true;
}

