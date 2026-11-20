#include "plagiarism_checker.h"
#include <sstream>
#include <iomanip>

std::string PlagiarismChecker::riskToString(PlagiarismRisk risk) {
    switch (risk) {
        case PlagiarismRisk::LOW:      return "LOW RISK (Original Content)";
        case PlagiarismRisk::MODERATE: return "MODERATE RISK (Partial Overlap)";
        case PlagiarismRisk::HIGH:     return "HIGH RISK (Potential Plagiarism)";
        case PlagiarismRisk::CRITICAL: return "CRITICAL RISK (Severe Plagiarism)";
    }
    return "UNKNOWN";
}

std::string PlagiarismChecker::getDisclaimer() {
    return "DISCLAIMER: This tool conducts an offline, lexical line-by-line comparison\n"
           "between the two user-supplied files using the Longest Common Subsequence (LCS)\n"
           "algorithm. It DOES NOT query external databases, web search engines, or private\n"
           "institutional archives. Similarity indicates textual overlap between these two files only.";
}

PlagiarismReport PlagiarismChecker::analyze(const DiffResult& diff) {
    PlagiarismReport report;
    report.similarityScore = diff.similarityScore;
    report.matchingLineCount = diff.identicalCount;
    report.totalLinesA = diff.totalLinesA;
    report.totalLinesB = diff.totalLinesB;
    report.disclaimer = getDisclaimer();

    // Calculate coverage ratios
    if (diff.totalLinesA > 0) {
        report.matchingRatioA = (static_cast<double>(diff.identicalCount) / diff.totalLinesA) * 100.0;
    } else {
        report.matchingRatioA = 0.0;
    }

    if (diff.totalLinesB > 0) {
        report.matchingRatioB = (static_cast<double>(diff.identicalCount) / diff.totalLinesB) * 100.0;
    } else {
        report.matchingRatioB = 0.0;
    }

    // Determine Risk Category and Warnings
    if (report.similarityScore >= 80.0) {
        report.riskLevel = PlagiarismRisk::CRITICAL;
        report.riskDescription = "CRITICAL: Extreme textual overlap detected. Files are virtually identical or direct copies.";
        report.warningMessage = "****************************************************************\n"
                                "*  !!! CRITICAL PLAGIARISM ALERT !!!                           *\n"
                                "*  Over 80% similarity detected! Content appears unoriginal.  *\n"
                                "****************************************************************";
    } else if (report.similarityScore >= 60.0) {
        report.riskLevel = PlagiarismRisk::HIGH;
        report.riskDescription = "HIGH RISK: Substantial portions of content match verbatim. High probability of plagiarism.";
        report.warningMessage = "****************************************************************\n"
                                "*  [!] HIGH SIMILARITY WARNING                                 *\n"
                                "*  Similarity exceeds 60%. Manual review strongly advised.     *\n"
                                "****************************************************************";
    } else if (report.similarityScore >= 30.0) {
        report.riskLevel = PlagiarismRisk::MODERATE;
        report.riskDescription = "MODERATE: Moderate overlap detected. Contains shared sections, boilerplate, or partial edits.";
        report.warningMessage = "[INFO] Moderate similarity detected. Review shared blocks to determine legitimacy.";
    } else {
        report.riskLevel = PlagiarismRisk::LOW;
        report.riskDescription = "LOW RISK: Content exhibits high independence and originality.";
        report.warningMessage = "[OK] Low similarity detected. Files appear mostly independent.";
    }

    // Extract contiguous identical blocks (clusters of copied lines)
    CopiedBlock currentBlock;
    bool inBlock = false;

    for (const auto& line : diff.diffLines) {
        if (line.type == DiffType::IDENTICAL) {
            if (!inBlock) {
                inBlock = true;
                currentBlock.startLineA = line.lineNumA;
                currentBlock.startLineB = line.lineNumB;
                currentBlock.lines.clear();
            }
            currentBlock.endLineA = line.lineNumA;
            currentBlock.endLineB = line.lineNumB;
            currentBlock.lines.push_back(line.textA);
        } else {
            if (inBlock) {
                currentBlock.lineCount = static_cast<int>(currentBlock.lines.size());
                report.copiedBlocks.push_back(currentBlock);
                inBlock = false;
            }
        }
    }

    if (inBlock) {
        currentBlock.lineCount = static_cast<int>(currentBlock.lines.size());
        report.copiedBlocks.push_back(currentBlock);
    }

    return report;
}

