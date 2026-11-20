#include "diff_engine.h"
#include "file_handler.h"
#include <algorithm>
#include <iomanip>
#include <cmath>

std::vector<std::vector<int>> DiffEngine::computeLCSMatrix(
    const std::vector<std::string>& a, 
    const std::vector<std::string>& b) 
{
    int m = static_cast<int>(a.size());
    int n = static_cast<int>(b.size());

    // Create DP table of dimensions (m + 1) x (n + 1) initialized to 0
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));

    // Fill table using optimal substructure:
    // dp[i][j] = dp[i-1][j-1] + 1 if lines match
    // dp[i][j] = max(dp[i-1][j], dp[i][j-1]) otherwise
    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (a[i - 1] == b[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1] + 1;
            } else {
                dp[i][j] = std::max(dp[i - 1][j], dp[i][j - 1]);
            }
        }
    }

    return dp;
}

double DiffEngine::calculateSimilarity(int lcsLength, int totalA, int totalB) {
    if (totalA == 0 && totalB == 0) {
        // Both files are empty, hence identical
        return 100.00;
    }
    if (totalA == 0 || totalB == 0) {
        // One file is empty, the other is not
        return 0.00;
    }

    // Standard similarity formula: (2 * LCS) / (Total_A + Total_B) * 100
    double score = (2.0 * static_cast<double>(lcsLength)) / 
                   (static_cast<double>(totalA + totalB)) * 100.0;

    if (score > 100.0) score = 100.0;
    if (score < 0.0) score = 0.0;

    return score;
}

std::string DiffEngine::diffTypeToString(DiffType type) {
    switch (type) {
        case DiffType::IDENTICAL: return "IDENTICAL";
        case DiffType::ADDED:     return "ADDED";
        case DiffType::DELETED:   return "DELETED";
        case DiffType::MODIFIED:  return "MODIFIED";
    }
    return "UNKNOWN";
}

std::string DiffEngine::diffTypeSymbol(DiffType type) {
    switch (type) {
        case DiffType::IDENTICAL: return " [=] ";
        case DiffType::ADDED:     return " [+] ";
        case DiffType::DELETED:   return " [-] ";
        case DiffType::MODIFIED:  return " [~] ";
    }
    return " [?] ";
}

DiffResult DiffEngine::compare(const std::string& pathA,
                              const std::vector<std::string>& linesA,
                              const std::string& pathB,
                              const std::vector<std::string>& linesB) 
{
    DiffResult result;
    result.fileAPath = pathA;
    result.fileBPath = pathB;
    result.fileAName = FileHandler::extractFileName(pathA);
    result.fileBName = FileHandler::extractFileName(pathB);

    int m = static_cast<int>(linesA.size());
    int n = static_cast<int>(linesB.size());
    result.totalLinesA = m;
    result.totalLinesB = n;

    // Step 1: Compute LCS Matrix via Dynamic Programming
    result.dpMatrix = computeLCSMatrix(linesA, linesB);
    result.lcsLength = result.dpMatrix[m][n];

    // Step 2: Compute Similarity Score
    result.similarityScore = calculateSimilarity(result.lcsLength, m, n);

    // Step 3: Backtrack from (m, n) to (0, 0) to obtain raw diff actions
    struct RawItem {
        char op; // '=', '+', '-'
        int lineA;
        int lineB;
        std::string textA;
        std::string textB;
    };

    std::vector<RawItem> rawItems;
    int i = m;
    int j = n;

    while (i > 0 || j > 0) {
        if (i > 0 && j > 0 && linesA[i - 1] == linesB[j - 1]) {
            rawItems.push_back({'=', i, j, linesA[i - 1], linesB[j - 1]});
            --i;
            --j;
        } else if (j > 0 && (i == 0 || result.dpMatrix[i][j - 1] >= result.dpMatrix[i - 1][j])) {
            rawItems.push_back({'+', -1, j, "", linesB[j - 1]});
            --j;
        } else if (i > 0 && (j == 0 || result.dpMatrix[i][j - 1] < result.dpMatrix[i - 1][j])) {
            rawItems.push_back({'-', i, -1, linesA[i - 1], ""});
            --i;
        }
    }

    // Reverse to get chronological order from line 1 onwards
    std::reverse(rawItems.begin(), rawItems.end());

    // Step 4: Group contiguous deletions and additions into MODIFIED lines
    size_t idx = 0;
    while (idx < rawItems.size()) {
        if (rawItems[idx].op == '=') {
            DiffLine dl;
            dl.type = DiffType::IDENTICAL;
            dl.lineNumA = rawItems[idx].lineA;
            dl.lineNumB = rawItems[idx].lineB;
            dl.textA = rawItems[idx].textA;
            dl.textB = rawItems[idx].textB;
            result.diffLines.push_back(dl);
            result.identicalCount++;
            idx++;
        } else {
            // Collect contiguous block of deletions ('-') and additions ('+')
            std::vector<RawItem> dels;
            std::vector<RawItem> adds;

            while (idx < rawItems.size() && rawItems[idx].op != '=') {
                if (rawItems[idx].op == '-') {
                    dels.push_back(rawItems[idx]);
                } else if (rawItems[idx].op == '+') {
                    adds.push_back(rawItems[idx]);
                }
                idx++;
            }

            // Pair up deletions and additions as MODIFIED
            size_t minPairs = std::min(dels.size(), adds.size());
            for (size_t p = 0; p < minPairs; ++p) {
                DiffLine dl;
                dl.type = DiffType::MODIFIED;
                dl.lineNumA = dels[p].lineA;
                dl.lineNumB = adds[p].lineB;
                dl.textA = dels[p].textA;
                dl.textB = adds[p].textB;
                result.diffLines.push_back(dl);
                result.modifiedCount++;
            }

            // Any remaining deletions are pure DELETED lines
            for (size_t d = minPairs; d < dels.size(); ++d) {
                DiffLine dl;
                dl.type = DiffType::DELETED;
                dl.lineNumA = dels[d].lineA;
                dl.lineNumB = -1;
                dl.textA = dels[d].textA;
                dl.textB = "";
                result.diffLines.push_back(dl);
                result.deletedCount++;
            }

            // Any remaining additions are pure ADDED lines
            for (size_t a = minPairs; a < adds.size(); ++a) {
                DiffLine dl;
                dl.type = DiffType::ADDED;
                dl.lineNumA = -1;
                dl.lineNumB = adds[a].lineB;
                dl.textA = "";
                dl.textB = adds[a].textB;
                result.diffLines.push_back(dl);
                result.addedCount++;
            }
        }
    }

    return result;
}

