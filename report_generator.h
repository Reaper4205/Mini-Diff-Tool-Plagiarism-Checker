#ifndef REPORT_GENERATOR_H
#define REPORT_GENERATOR_H

#include "diff_engine.h"
#include "plagiarism_checker.h"
#include <string>

/**
 * @brief Handles terminal formatting and persistent file export
 *        for comparison summaries, diff views, and plagiarism reports.
 */
class ReportGenerator {
public:
    /**
     * @brief Prints a standardized decorative banner header.
     */
    static void printHeader(const std::string& title);

    /**
     * @brief Prints a summary comparison table to the console.
     */
    static void displayComparisonSummary(const DiffResult& result);

    /**
     * @brief Prints detailed similarity score calculation and visual meter.
     */
    static void displaySimilarityScore(const DiffResult& result);

    /**
     * @brief Prints all identical / matching lines between the two files.
     */
    static void displayMatchingLines(const DiffResult& result);

    /**
     * @brief Prints all modified, added, and deleted lines with line numbers.
     */
    static void displayChangedLines(const DiffResult& result);

    /**
     * @brief Prints the comprehensive plagiarism detection findings.
     */
    static void displayPlagiarismReport(const DiffResult& result, 
                                        const PlagiarismReport& report);

    /**
     * @brief Formats the complete comparison and plagiarism findings into a single string.
     */
    static std::string buildFullTextReport(const DiffResult& result, 
                                           const PlagiarismReport& report);

    /**
     * @brief Exports the full text report to a file (default: comparison_report.txt).
     */
    static bool saveReportToFile(const std::string& filePath,
                                 const std::string& reportContent,
                                 std::string& outError);
};

#endif // REPORT_GENERATOR_H

