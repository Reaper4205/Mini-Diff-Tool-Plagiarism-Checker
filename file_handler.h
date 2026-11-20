#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <string>
#include <vector>

/**
 * @brief Utility class responsible for safe file operations:
 *        reading line-by-line, writing reports, and checking file existence.
 */
class FileHandler {
public:
    /**
     * @brief Reads all lines from a text file into a vector.
     * @param filePath Path to the file.
     * @param outLines Vector to store lines.
     * @param outError String to store error message if operation fails.
     * @return true if successfully opened and read (even if empty); false on error.
     */
    static bool readFile(const std::string& filePath, 
                         std::vector<std::string>& outLines, 
                         std::string& outError);

    /**
     * @brief Writes a string content into a file.
     * @param filePath Path to destination file.
     * @param content Text to write.
     * @param outError String to store error message if write fails.
     * @return true if successfully written; false on error.
     */
    static bool writeFile(const std::string& filePath, 
                          const std::string& content, 
                          std::string& outError);

    /**
     * @brief Checks if a file exists on disk and is accessible.
     * @param filePath Path to file.
     * @return true if file exists; false otherwise.
     */
    static bool fileExists(const std::string& filePath);

    /**
     * @brief Extracts file name and extension from a path.
     * @param filePath Full or relative file path.
     * @return Base file name.
     */
    static std::string extractFileName(const std::string& filePath);

    /**
     * @brief Strips Windows carriage return ('\r') and trailing whitespace.
     * @param str Input line string.
     * @return Cleaned string.
     */
    static std::string cleanLine(const std::string& str);
};

#endif // FILE_HANDLER_H

