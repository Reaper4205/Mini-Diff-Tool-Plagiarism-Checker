#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <iomanip>
#include "file_handler.h"
#include "diff_engine.h"
#include "plagiarism_checker.h"
#include "report_generator.h"

// Helper function to pause and wait for user to press Enter
void pauseConsole() {
    std::cout << "\nPress Enter to return to main menu...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// Helper to prompt for a string line input
std::string promptString(const std::string& promptText) {
    std::string input;
    std::cout << promptText;
    std::getline(std::cin, input);
    // Trim leading/trailing whitespace
    size_t first = input.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = input.find_last_not_of(" \t\r\n");
    return input.substr(first, (last - first + 1));
}

// Function to handle File Comparison (Option 1)
bool performComparison(DiffResult& outResult, 
                       PlagiarismReport& outPlagiarism, 
                       bool& hasComparison) 
{
    ReportGenerator::printHeader("FILE COMPARISON SETUP");
    std::cout << "  Tip: You can use sample paths: samples/file1.txt and samples/file2.txt\n\n";

    std::string pathA = promptString("  Enter path for File A (Original): ");
    if (pathA.empty()) {
        std::cout << "  [!] File path cannot be empty. Operation canceled.\n";
        return false;
    }

    std::string pathB = promptString("  Enter path for File B (Target/Copy): ");
    if (pathB.empty()) {
        std::cout << "  [!] File path cannot be empty. Operation canceled.\n";
        return false;
    }

    std::vector<std::string> linesA;
    std::vector<std::string> linesB;
    std::string errorMsg;

    std::cout << "\n  --> Reading File A: " << pathA << " ...\n";
    if (!FileHandler::readFile(pathA, linesA, errorMsg)) {
        std::cout << "  [X] Failed: " << errorMsg << "\n";
        return false;
    }
    std::cout << "      [OK] Loaded " << linesA.size() << " line(s) from " 
              << FileHandler::extractFileName(pathA) << "\n";

    std::cout << "  --> Reading File B: " << pathB << " ...\n";
    if (!FileHandler::readFile(pathB, linesB, errorMsg)) {
        std::cout << "  [X] Failed: " << errorMsg << "\n";
        return false;
    }
    std::cout << "      [OK] Loaded " << linesB.size() << " line(s) from " 
              << FileHandler::extractFileName(pathB) << "\n";

    // Handle empty file notifications
    if (linesA.empty() && linesB.empty()) {
        std::cout << "  [!] Notice: Both files are completely empty.\n";
    } else if (linesA.empty()) {
        std::cout << "  [!] Notice: File A is empty.\n";
    } else if (linesB.empty()) {
        std::cout << "  [!] Notice: File B is empty.\n";
    }

    std::cout << "\n  --> Computing Longest Common Subsequence (LCS) & Aligning diffs...\n";
    outResult = DiffEngine::compare(pathA, linesA, pathB, linesB);
    outPlagiarism = PlagiarismChecker::analyze(outResult);
    hasComparison = true;

    std::cout << "      [OK] Comparison successfully completed!\n";
    ReportGenerator::displayComparisonSummary(outResult);

    return true;
}

// Display the Main Menu
void printMainMenu(bool hasComparison, const DiffResult& result) {
    std::cout << "\n======================================================================\n";
    std::cout << "         MINI DIFF TOOL & PLAGIARISM CHECKER (C++17)                  \n";
    std::cout << "======================================================================\n";
    if (hasComparison) {
        std::cout << "  [ACTIVE FILES]: A: \"" << result.fileAName << "\" (" << result.totalLinesA << " lines) | "
                  << "B: \"" << result.fileBName << "\" (" << result.totalLinesB << " lines)\n";
        std::cout << "  [SIMILARITY]  : " << std::fixed << std::setprecision(2) << result.similarityScore << " %\n";
    } else {
        std::cout << "  [STATUS]: No files currently loaded or compared.\n";
    }
    std::cout << "----------------------------------------------------------------------\n";
    std::cout << "  1. Compare Two Text Files\n";
    std::cout << "  2. Check Similarity Percentage\n";
    std::cout << "  3. View Matching Lines\n";
    std::cout << "  4. View Changed Lines (Added / Deleted / Modified)\n";
    std::cout << "  5. Detect Potential Plagiarism\n";
    std::cout << "  6. Generate Comparison Report (View & Save)\n";
    std::cout << "  7. Exit\n";
    std::cout << "======================================================================\n";
    std::cout << "  Enter your choice (1-7): ";
}

int main() {
    DiffResult currentResult;
    PlagiarismReport currentPlagiarism;
    bool hasComparison = false;

    std::cout << "\n======================================================================\n";
    std::cout << "   WELCOME TO THE MINI DIFF TOOL & PLAGIARISM CHECKER CONSOLE APP     \n";
    std::cout << "   Developed for Data Structures & Algorithms (C++17) Project        \n";
    std::cout << "======================================================================\n";

    while (true) {
        printMainMenu(hasComparison, currentResult);

        std::string choiceStr;
        if (!std::getline(std::cin, choiceStr)) {
            break;
        }

        // Validate menu input
        if (choiceStr.empty()) continue;
        int choice = -1;
        try {
            choice = std::stoi(choiceStr);
        } catch (...) {
            choice = -1;
        }

        switch (choice) {
            case 1: {
                performComparison(currentResult, currentPlagiarism, hasComparison);
                pauseConsole();
                break;
            }

            case 2: {
                if (!hasComparison) {
                    std::cout << "\n  [!] Please run Option 1 (Compare Two Text Files) first.\n";
                    std::string ans = promptString("  Would you like to compare files now? (y/n): ");
                    if (ans == "y" || ans == "Y") {
                        performComparison(currentResult, currentPlagiarism, hasComparison);
                    }
                } else {
                    ReportGenerator::displaySimilarityScore(currentResult);
                }
                pauseConsole();
                break;
            }

            case 3: {
                if (!hasComparison) {
                    std::cout << "\n  [!] Please run Option 1 (Compare Two Text Files) first.\n";
                    std::string ans = promptString("  Would you like to compare files now? (y/n): ");
                    if (ans == "y" || ans == "Y") {
                        performComparison(currentResult, currentPlagiarism, hasComparison);
                    }
                } else {
                    ReportGenerator::displayMatchingLines(currentResult);
                }
                pauseConsole();
                break;
            }

            case 4: {
                if (!hasComparison) {
                    std::cout << "\n  [!] Please run Option 1 (Compare Two Text Files) first.\n";
                    std::string ans = promptString("  Would you like to compare files now? (y/n): ");
                    if (ans == "y" || ans == "Y") {
                        performComparison(currentResult, currentPlagiarism, hasComparison);
                    }
                } else {
                    ReportGenerator::displayChangedLines(currentResult);
                }
                pauseConsole();
                break;
            }

            case 5: {
                if (!hasComparison) {
                    std::cout << "\n  [!] Please run Option 1 (Compare Two Text Files) first.\n";
                    std::string ans = promptString("  Would you like to compare files now? (y/n): ");
                    if (ans == "y" || ans == "Y") {
                        performComparison(currentResult, currentPlagiarism, hasComparison);
                    }
                } else {
                    ReportGenerator::displayPlagiarismReport(currentResult, currentPlagiarism);
                }
                pauseConsole();
                break;
            }

            case 6: {
                if (!hasComparison) {
                    std::cout << "\n  [!] Please run Option 1 (Compare Two Text Files) first.\n";
                    std::string ans = promptString("  Would you like to compare files now? (y/n): ");
                    if (ans == "y" || ans == "Y") {
                        performComparison(currentResult, currentPlagiarism, hasComparison);
                    }
                }
                
                if (hasComparison) {
                    ReportGenerator::displayComparisonSummary(currentResult);
                    ReportGenerator::displayPlagiarismReport(currentResult, currentPlagiarism);

                    std::string saveAns = promptString("  Save full report to 'comparison_report.txt'? (y/n) [Default: y]: ");
                    if (saveAns.empty() || saveAns == "y" || saveAns == "Y") {
                        std::string fullReport = ReportGenerator::buildFullTextReport(currentResult, currentPlagiarism);
                        std::string savePath = "comparison_report.txt";
                        std::string error;
                        if (ReportGenerator::saveReportToFile(savePath, fullReport, error)) {
                            std::cout << "\n  [SUCCESS] Comparison report saved to: " << savePath << "\n";
                        } else {
                            std::cout << "\n  [ERROR] Failed to save report: " << error << "\n";
                        }
                    } else {
                        std::cout << "  Report not saved.\n";
                    }
                }
                pauseConsole();
                break;
            }

            case 7: {
                std::cout << "\n======================================================================\n";
                std::cout << "  Thank you for using Mini Diff Tool & Plagiarism Checker!\n";
                std::cout << "  Exiting application...\n";
                std::cout << "======================================================================\n\n";
                return 0;
            }

            default: {
                std::cout << "\n  [!] Invalid choice: '" << choiceStr << "'. Please enter a number between 1 and 7.\n";
                pauseConsole();
                break;
            }
        }
    }

    return 0;
}
