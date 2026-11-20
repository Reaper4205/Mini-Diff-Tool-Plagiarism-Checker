#include "report_generator.h"
#include "file_handler.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>

void ReportGenerator::printHeader(const std::string& title) {
    std::cout << "\n======================================================================\n";
    std::cout << "  " << title << "\n";
    std::cout << "======================================================================\n";
}

void ReportGenerator::displayComparisonSummary(const DiffResult& result) {
    printHeader("COMPARISON SUMMARY REPORT");
    std::cout << std::left << std::setw(28) << "  File A (Original):" << result.fileAName << " (" << result.fileAPath << ")\n";
    std::cout << std::left << std::setw(28) << "  File B (Target):"   << result.fileBName << " (" << result.fileBPath << ")\n";
    std::cout << "----------------------------------------------------------------------\n";
    std::cout << std::left << std::setw(32) << "  Total Lines in File A:"       << std::right << std::setw(6) << result.totalLinesA << "\n";
    std::cout << std::left << std::setw(32) << "  Total Lines in File B:"       << std::right << std::setw(6) << result.totalLinesB << "\n";
    std::cout << std::left << std::setw(32) << "  LCS Length (Matched lines):"  << std::right << std::setw(6) << result.lcsLength << "\n";
    std::cout << "----------------------------------------------------------------------\n";
    std::cout << std::left << std::setw(32) << "  [=] Identical Lines:"         << std::right << std::setw(6) << result.identicalCount << "\n";
    std::cout << std::left << std::setw(32) << "  [~] Modified Lines:"          << std::right << std::setw(6) << result.modifiedCount << "\n";
    std::cout << std::left << std::setw(32) << "  [+] Added Lines:"             << std::right << std::setw(6) << result.addedCount << "\n";
    std::cout << std::left << std::setw(32) << "  [-] Deleted Lines:"           << std::right << std::setw(6) << result.deletedCount << "\n";
    std::cout << "----------------------------------------------------------------------\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << std::left << std::setw(32) << "  Overall Similarity Percentage:" << std::right << std::setw(6) << result.similarityScore << " %\n";
    std::cout << "======================================================================\n";
}

void ReportGenerator::displaySimilarityScore(const DiffResult& result) {
    printHeader("SIMILARITY SCORE ANALYSIS");
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "  Formula:\n";
    std::cout << "    Similarity = ( 2 * LCS Length / (Total Lines A + Total Lines B) ) * 100\n\n";
    std::cout << "  Calculation Details:\n";
    std::cout << "    - File A (" << result.fileAName << ") Total Lines : " << result.totalLinesA << "\n";
    std::cout << "    - File B (" << result.fileBName << ") Total Lines : " << result.totalLinesB << "\n";
    std::cout << "    - Longest Common Subsequence (LCS): " << result.lcsLength << "\n";

    if (result.totalLinesA + result.totalLinesB == 0) {
        std::cout << "    - Both files are empty -> Defined Similarity = 100.00%\n";
    } else {
        std::cout << "    - Substitution: (2 * " << result.lcsLength << " / (" 
                  << result.totalLinesA << " + " << result.totalLinesB << ")) * 100\n";
    }

    std::cout << "\n  Final Similarity Percentage : " << result.similarityScore << " %\n\n";

    // Visual Meter (30 characters wide)
    int meterFilled = static_cast<int>((result.similarityScore / 100.0) * 30.0);
    if (meterFilled > 30) meterFilled = 30;
    if (meterFilled < 0) meterFilled = 0;

    std::cout << "  Similarity Gauge:\n  [";
    for (int i = 0; i < meterFilled; ++i) std::cout << "=";
    for (int i = meterFilled; i < 30; ++i) std::cout << " ";
    std::cout << "] " << result.similarityScore << " %\n";
    std::cout << "======================================================================\n";
}

void ReportGenerator::displayMatchingLines(const DiffResult& result) {
    printHeader("MATCHING / IDENTICAL LINES VIEW");
    std::cout << "  Showing lines that appear identically and in common subsequence:\n\n";
    std::cout << std::left << std::setw(10) << "File A" 
              << std::setw(10) << "File B" 
              << "Matching Content\n";
    std::cout << "----------------------------------------------------------------------\n";

    int matchCount = 0;
    for (const auto& dl : result.diffLines) {
        if (dl.type == DiffType::IDENTICAL) {
            matchCount++;
            std::cout << "Line " << std::left << std::setw(5) << dl.lineNumA
                      << "Line " << std::setw(5) << dl.lineNumB
                      << "| " << dl.textA << "\n";
        }
    }

    if (matchCount == 0) {
        std::cout << "  (No identical lines found between the two files)\n";
    }

    std::cout << "----------------------------------------------------------------------\n";
    std::cout << "  Total Matching Lines: " << matchCount << "\n";
    std::cout << "======================================================================\n";
}

void ReportGenerator::displayChangedLines(const DiffResult& result) {
    printHeader("CHANGED LINES VIEW (Added / Deleted / Modified)");
    std::cout << "  Symbols Legend:\n";
    std::cout << "    [~] MODIFIED : Line in File A was altered in File B\n";
    std::cout << "    [-] DELETED  : Line exists in File A but was removed from File B\n";
    std::cout << "    [+] ADDED    : Line was newly inserted into File B\n\n";
    std::cout << std::left << std::setw(8) << "Status" 
              << std::setw(11) << "Line(A->B)" 
              << "Line Content Details\n";
    std::cout << "----------------------------------------------------------------------\n";

    int changeCount = 0;
    for (const auto& dl : result.diffLines) {
        if (dl.type == DiffType::IDENTICAL) continue;

        changeCount++;
        if (dl.type == DiffType::MODIFIED) {
            std::string lineNums = std::to_string(dl.lineNumA) + "->" + std::to_string(dl.lineNumB);
            std::cout << "[~] MOD " << std::left << std::setw(11) << lineNums
                      << "| (A): " << dl.textA << "\n";
            std::cout << "           " << std::left << std::setw(11) << " "
                      << "| (B): " << dl.textB << "\n";
        } else if (dl.type == DiffType::DELETED) {
            std::string lineNums = std::to_string(dl.lineNumA) + "-> -";
            std::cout << "[-] DEL " << std::left << std::setw(11) << lineNums
                      << "| (A): " << dl.textA << "\n";
        } else if (dl.type == DiffType::ADDED) {
            std::string lineNums = "- ->" + std::to_string(dl.lineNumB);
            std::cout << "[+] ADD " << std::left << std::setw(11) << lineNums
                      << "| (B): " << dl.textB << "\n";
        }
    }

    if (changeCount == 0) {
        std::cout << "  (Both files are completely identical; no changes detected)\n";
    }

    std::cout << "----------------------------------------------------------------------\n";
    std::cout << "  Total Changed Lines: " << changeCount 
              << " (Modified: " << result.modifiedCount 
              << ", Added: " << result.addedCount 
              << ", Deleted: " << result.deletedCount << ")\n";
    std::cout << "======================================================================\n";
}

void ReportGenerator::displayPlagiarismReport(const DiffResult& result, 
                                            const PlagiarismReport& report) {
    printHeader("PLAGIARISM DETECTION & INTEGRITY ANALYSIS");
    std::cout << std::left << std::setw(28) << "  File A (Original):" << result.fileAName << "\n";
    std::cout << std::left << std::setw(28) << "  File B (Target):"   << result.fileBName << "\n";
    std::cout << "----------------------------------------------------------------------\n";
    std::cout << std::fixed << std::setprecision(2);

    std::cout << "\n" << report.warningMessage << "\n\n";

    std::cout << "  Assessment Summary:\n";
    std::cout << "    - Risk Classification  : " << PlagiarismChecker::riskToString(report.riskLevel) << "\n";
    std::cout << "    - Similarity Percentage: " << report.similarityScore << " %\n";
    std::cout << "    - Matching Lines       : " << report.matchingLineCount << "\n";
    std::cout << "    - Overlap Ratio in A   : " << report.matchingRatioA << " % (" << report.matchingLineCount << "/" << report.totalLinesA << " lines)\n";
    std::cout << "    - Overlap Ratio in B   : " << report.matchingRatioB << " % (" << report.matchingLineCount << "/" << report.totalLinesB << " lines)\n";
    std::cout << "    - Evaluation Note      : " << report.riskDescription << "\n\n";

    std::cout << "  Identified Copied / Verbatim Blocks (" << report.copiedBlocks.size() << " detected):\n";
    std::cout << "----------------------------------------------------------------------\n";

    if (report.copiedBlocks.empty()) {
        std::cout << "  No multi-line identical blocks detected.\n";
    } else {
        int blockIndex = 1;
        for (const auto& blk : report.copiedBlocks) {
            std::cout << "  Block #" << blockIndex++ << ": " 
                      << blk.lineCount << " line(s) matching\n";
            std::cout << "    File A: Lines " << blk.startLineA << " to " << blk.endLineA 
                      << " <===> File B: Lines " << blk.startLineB << " to " << blk.endLineB << "\n";
            std::cout << "    Content Sample:\n";
            for (const auto& l : blk.lines) {
                std::cout << "      | " << l << "\n";
            }
            std::cout << "----------------------------------------------------------------------\n";
        }
    }

    std::cout << "\n" << report.disclaimer << "\n";
    std::cout << "======================================================================\n";
}

std::string ReportGenerator::buildFullTextReport(const DiffResult& result, 
                                                const PlagiarismReport& report) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2);

    // Header & metadata
    ss << "======================================================================\n";
    ss << "              MINI DIFF TOOL & PLAGIARISM CHECKER REPORT              \n";
    ss << "======================================================================\n";

    // Current date/time
    std::time_t now = std::time(nullptr);
    char dtBuf[64];
    std::strftime(dtBuf, sizeof(dtBuf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    ss << "Report Generated At: " << dtBuf << "\n\n";

    // File info
    ss << "--- FILE METADATA ---\n";
    ss << "File A (Original): " << result.fileAName << " (" << result.fileAPath << ")\n";
    ss << "File B (Target)  : " << result.fileBName << " (" << result.fileBPath << ")\n";
    ss << "Total Lines in File A: " << result.totalLinesA << "\n";
    ss << "Total Lines in File B: " << result.totalLinesB << "\n\n";

    // Metrics
    ss << "--- COMPARISON METRICS ---\n";
    ss << "LCS Length (Matched): " << result.lcsLength << "\n";
    ss << "Identical Lines     : " << result.identicalCount << "\n";
    ss << "Modified Lines      : " << result.modifiedCount << "\n";
    ss << "Added Lines         : " << result.addedCount << "\n";
    ss << "Deleted Lines       : " << result.deletedCount << "\n";
    ss << "Similarity Score    : " << result.similarityScore << " %\n\n";

    // Plagiarism findings
    ss << "--- PLAGIARISM ANALYSIS ---\n";
    ss << "Risk Level       : " << PlagiarismChecker::riskToString(report.riskLevel) << "\n";
    ss << "Overlap in File A: " << report.matchingRatioA << " %\n";
    ss << "Overlap in File B: " << report.matchingRatioB << " %\n";
    ss << "Verdict          : " << report.riskDescription << "\n";
    ss << "\nCopied Blocks Count: " << report.copiedBlocks.size() << "\n";

    int bIdx = 1;
    for (const auto& blk : report.copiedBlocks) {
        ss << "  [Block #" << bIdx++ << "] Lines A(" << blk.startLineA << "-" << blk.endLineA 
           << ") <=> Lines B(" << blk.startLineB << "-" << blk.endLineB << "), Count: " 
           << blk.lineCount << "\n";
        for (const auto& l : blk.lines) {
            ss << "      " << l << "\n";
        }
    }
    ss << "\n";

    // Complete line-by-line diff
    ss << "--- LINE-BY-LINE ALIGNED DIFF ---\n";
    ss << std::left << std::setw(12) << "Diff Type" 
       << std::setw(10) << "Line A" 
       << std::setw(10) << "Line B" 
       << "Content Details\n";
    ss << "----------------------------------------------------------------------\n";

    for (const auto& dl : result.diffLines) {
        std::string sA = (dl.lineNumA > 0) ? std::to_string(dl.lineNumA) : "-";
        std::string sB = (dl.lineNumB > 0) ? std::to_string(dl.lineNumB) : "-";

        if (dl.type == DiffType::IDENTICAL) {
            ss << std::left << std::setw(12) << "IDENTICAL" 
               << std::setw(10) << sA 
               << std::setw(10) << sB 
               << dl.textA << "\n";
        } else if (dl.type == DiffType::MODIFIED) {
            ss << std::left << std::setw(12) << "MODIFIED" 
               << std::setw(10) << sA 
               << std::setw(10) << sB 
               << "OLD: " << dl.textA << "  ==>  NEW: " << dl.textB << "\n";
        } else if (dl.type == DiffType::DELETED) {
            ss << std::left << std::setw(12) << "DELETED" 
               << std::setw(10) << sA 
               << std::setw(10) << sB 
               << "DEL: " << dl.textA << "\n";
        } else if (dl.type == DiffType::ADDED) {
            ss << std::left << std::setw(12) << "ADDED" 
               << std::setw(10) << sA 
               << std::setw(10) << sB 
               << "ADD: " << dl.textB << "\n";
        }
    }

    ss << "\n--- DISCLAIMER ---\n";
    ss << report.disclaimer << "\n";
    ss << "======================================================================\n";

    return ss.str();
}

bool ReportGenerator::saveReportToFile(const std::string& filePath,
                                     const std::string& reportContent,
                                     std::string& outError) {
    return FileHandler::writeFile(filePath, reportContent, outError);
}
