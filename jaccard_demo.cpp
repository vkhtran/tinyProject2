#include <algorithm>
#include <cctype>
#include <iostream>
#include <set>
#include <sstream>
#include <string>

using namespace std;

string normalizeWord(string word) {
    string cleaned;

    for (char ch : word) {
        if (isalpha(static_cast<unsigned char>(ch))) {
            cleaned += static_cast<char>(tolower(static_cast<unsigned char>(ch)));
        }
    }

    return cleaned;
}

set<string> sentenceToSet(const string& sentence) {
    set<string> words;
    stringstream ss(sentence);
    string word;

    while (ss >> word) {
        string cleaned = normalizeWord(word);
        if (!cleaned.empty()) {
            words.insert(cleaned);
        }
    }

    return words;
}

double jaccardSimilarity(const set<string>& a, const set<string>& b) {
    int intersectionCount = 0;

    for (const string& word : a) {
        if (b.count(word)) {
            intersectionCount++;
        }
    }

    int unionCount = static_cast<int>(a.size() + b.size() - intersectionCount);

    if (unionCount == 0) {
        return 1.0;
    }

    return static_cast<double>(intersectionCount) / unionCount;
}

void printSet(const set<string>& words) {
    cout << "{ ";
    for (const string& word : words) {
        cout << word << " ";
    }
    cout << "}";
}

int main() {
    string sentence1 = "apple banana orange banana";
    string sentence2 = "banana orange grape";

    set<string> set1 = sentenceToSet(sentence1);
    set<string> set2 = sentenceToSet(sentence2);

    cout << "Vi du Jaccard similarity\n\n";
    cout << "Cau 1: " << sentence1 << "\n";
    cout << "Cau 2: " << sentence2 << "\n\n";

    cout << "Tap hop tu cua cau 1 = ";
    printSet(set1);
    cout << "\n";

    cout << "Tap hop tu cua cau 2 = ";
    printSet(set2);
    cout << "\n\n";

    cout << "Y tuong:\n";
    cout << "- Intersection = cac phan tu chung\n";
    cout << "- Union = tat ca phan tu khong trung lap\n\n";

    int intersectionCount = 0;
    for (const string& word : set1) {
        if (set2.count(word)) {
            intersectionCount++;
        }
    }

    int unionCount = static_cast<int>(set1.size() + set2.size() - intersectionCount);
    double score = jaccardSimilarity(set1, set2);

    cout << "Intersection count = " << intersectionCount << "\n";
    cout << "Union count        = " << unionCount << "\n";
    cout << "Jaccard = Intersection / Union = "
         << intersectionCount << " / " << unionCount
         << " = " << score << "\n\n";

    cout << "Cach hieu nhanh:\n";
    cout << "- Gan 1: hai tap rat giong nhau\n";
    cout << "- Gan 0: hai tap rat khac nhau\n";

    return 0;
}
