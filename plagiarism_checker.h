#ifndef PLAGIARISM_CHECKER_H
#define PLAGIARISM_CHECKER_H

#include "diff_engine.h"
#include <string>
#include <vector>

/**
 * @brief Risk category based on the calculated similarity percentage.
 */
enum class PlagiarismRisk {
    LOW,        // < 30%
    MODERATE,   // 30% - 59.99%
    HIGH,       // 60% - 79.99%
    CRITICAL    // >= 80%
};

/**
 * @brief Represents a block of identical lines detected between both files.
 */
struct CopiedBlock {
    int startLineA;
    int endLineA;
    int startLineB;
    int endLineB;
    int lineCount;
    std::vector<std::string> lines;
};

/**
 * @brief Detailed plagiarism evaluation result.
 */
struct PlagiarismReport {
    double similarityScore;
    int matchingLineCount;
    int totalLinesA;
    int totalLinesB;
    double matchingRatioA; // Matching lines / Total lines in A (%)
    double matchingRatioB; // Matching lines / Total lines in B (%)
    PlagiarismRisk riskLevel;
    std::string riskDescription;
    std::string warningMessage;
    std::vector<CopiedBlock> copiedBlocks;
    std::string disclaimer;
};

/**
 * @brief Class that conducts plagiarism analysis on DiffResult.
 */
class PlagiarismChecker {
public:
    /**
     * @brief Evaluates plagiarism between the two files.
     * @param diff The result from DiffEngine.
     * @return PlagiarismReport with risk level, copied segments, and alerts.
     */
    static PlagiarismReport analyze(const DiffResult& diff);

    /**
     * @brief Returns a string representation of the risk level.
     */
    static std::string riskToString(PlagiarismRisk risk);

    /**
     * @brief Gets the official academic disclaimer text.
     */
    static std::string getDisclaimer();
};

#endif // PLAGIARISM_CHECKER_H

