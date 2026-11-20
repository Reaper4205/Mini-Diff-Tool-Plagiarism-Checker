#ifndef DIFF_ENGINE_H
#define DIFF_ENGINE_H

#include <string>
#include <vector>

/**
 * @brief Classification of each line during file comparison.
 */
enum class DiffType {
    IDENTICAL,  // Line exists in both files unchanged
    ADDED,      // Line exists only in File B (new insertion)
    DELETED,    // Line exists only in File A (removal)
    MODIFIED    // Line in File A was altered to a different line in File B
};

/**
 * @brief Represents a single line entry in the diff output.
 */
struct DiffLine {
    DiffType type;
    int lineNumA;          // 1-based index in File A (-1 if not present in A)
    int lineNumB;          // 1-based index in File B (-1 if not present in B)
    std::string textA;     // Original content from File A
    std::string textB;     // New content from File B
};

/**
 * @brief Comprehensive results of the file comparison.
 */
struct DiffResult {
    std::string fileAPath;
    std::string fileBPath;
    std::string fileAName;
    std::string fileBName;

    int totalLinesA = 0;
    int totalLinesB = 0;
    int lcsLength = 0;

    int identicalCount = 0;
    int addedCount = 0;
    int deletedCount = 0;
    int modifiedCount = 0;

    double similarityScore = 0.0;

    std::vector<DiffLine> diffLines;
    std::vector<std::vector<int>> dpMatrix; // DP table for algorithmic inspection
};

/**
 * @brief Engine responsible for calculating the Longest Common Subsequence (LCS),
 *        aligning files, classifying lines, and calculating similarity percentage.
 */
class DiffEngine {
public:
    /**
     * @brief Compares two files line-by-line using LCS Dynamic Programming.
     * @param pathA Path to File A.
     * @param linesA Lines of File A.
     * @param pathB Path to File B.
     * @param linesB Lines of File B.
     * @return DiffResult containing metrics, counts, and line-by-line diff.
     */
    static DiffResult compare(const std::string& pathA,
                              const std::vector<std::string>& linesA,
                              const std::string& pathB,
                              const std::vector<std::string>& linesB);

    /**
     * @brief Computes the Dynamic Programming LCS table for two line vectors.
     * @param a First vector of strings.
     * @param b Second vector of strings.
     * @return 2D DP matrix of dimensions (M+1) x (N+1).
     */
    static std::vector<std::vector<int>> computeLCSMatrix(
        const std::vector<std::string>& a, 
        const std::vector<std::string>& b);

    /**
     * @brief Calculates similarity percentage using the formula:
     *        (2 * LCS / (Total_A + Total_B)) * 100
     * @param lcsLength Length of LCS.
     * @param totalA Total lines in File A.
     * @param totalB Total lines in File B.
     * @return Similarity score between 0.00 and 100.00.
     */
    static double calculateSimilarity(int lcsLength, int totalA, int totalB);

    /**
     * @brief Converts a DiffType enum to a string symbol/label.
     */
    static std::string diffTypeToString(DiffType type);
    static std::string diffTypeSymbol(DiffType type);
};

#endif // DIFF_ENGINE_H

