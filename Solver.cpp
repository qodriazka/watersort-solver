#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <map>
#include <algorithm>
using namespace std;

using Board = vector<vector<char>>;

string boardToString(const Board& board) {
    string s = "";
    for (const auto& bottle : board) {
        for (char c : bottle) {
            s += c;
        }
    }
    return s;
}

struct State {
    Board currentBoard;
    vector<pair<int, int>> path;
    int f_cost;
    bool operator>(const State& other) const {
        return f_cost > other.f_cost;
    }
};

pair<char, int> getTop(const vector<char>& bottle) {
    for (int i = 3; i >= 0; --i) {
        if (bottle[i] != '.') {
            return {bottle[i], i};
        }
    }
    return {'\0', -1};
}

bool isBottleFull(const vector<char>& bottle) {
    return bottle[3] != '.';
}

bool isBottleEmpty(const vector<char>& bottle) {
    return bottle[0] == '.';
}

int countEmptySlots(const vector<char>& bottle) {
    int count = 0;
    for (int i = 3; i >= 0; --i) {
        if (bottle[i] == '.') {
            count++;
        } else {
            break;
        }
    }
    return count;
}

int countTopSameColors(const vector<char>& bottle) {
    pair<char, int> top = getTop(bottle);
    if (isBottleEmpty(bottle)) return 0;
    int count = 0;
    char color = top.first;
    for (int i = top.second; i >= 0; --i) {
        if (bottle[i] == color) {
            count++;
        } else {
            break;
        }
    }
    return count;
}

bool isSolved(const Board& board) {
    for (const auto& bottle : board) {
        if (isBottleEmpty(bottle)) {
            continue;
        } else {
            if (!isBottleFull(bottle)) {
                return false;
            }
            char firstColor = bottle[0];
            for (int i = 1; i < 4; ++i) {
                if (bottle[i] != firstColor) {
                    return false;
                }
            }
        }
    }
    return true;
}

vector<pair<int, int>> getPossibleMoves(const Board& currentBoard, int N) {
    vector<pair<int, int>> moves;
    for (int srcIdx = 0; srcIdx < N; ++srcIdx) {
        for (int destIdx = 0; destIdx < N; ++destIdx) {
            if (srcIdx == destIdx) continue;
            const auto& srcBottle = currentBoard[srcIdx];
            const auto& destBottle = currentBoard[destIdx];
            if (isBottleEmpty(srcBottle)) continue;
            if (isBottleFull(destBottle)) continue;
            char topSrcColor = getTop(srcBottle).first;
            char topDestColor = getTop(destBottle).first;
            if (topDestColor == '\0' || topSrcColor == topDestColor) {
                if (countEmptySlots(destBottle) > 0) {
                    moves.push_back({srcIdx, destIdx});
                }
            }
        }
    }
    return moves;
}

Board applyMove(const Board& currentBoard, int srcIdx, int destIdx) {
    Board newBoard = currentBoard;
    auto& srcBottle = newBoard[srcIdx];
    auto& destBottle = newBoard[destIdx];
    pair<char, int> topSrc = getTop(srcBottle);
    char topSrcColor = topSrc.first;
    int topSrcIndex = topSrc.second;
    int sameColorsAtSrcTop = countTopSameColors(srcBottle);
    int emptySlotsInDest = countEmptySlots(destBottle);
    int pourAmount = min(sameColorsAtSrcTop, emptySlotsInDest);
    for (int i = 0; i < pourAmount; ++i) {
        int destPourIndex = getTop(destBottle).second + 1;
        destBottle[destPourIndex] = topSrcColor;
        srcBottle[topSrcIndex - i] = '.';
    }
    return newBoard;
}

int heuristic(const Board& board) {
    int h = 0;
    for (const auto& bottle : board) {
        char prevColor = '\0';
        for (int i = 0; i < 4; ++i) {
            char currentColor = bottle[i];
            if (currentColor != '.' && prevColor != '\0' && currentColor != prevColor) {
                h++;
            }
            if (currentColor != '.') {
                prevColor = currentColor;
            }
        }
    }
    return h;
}

vector<pair<int, int>> solve(Board initialBoard, int N) {
    priority_queue<State, vector<State>, greater<State>> openList;
    map<string, int> closedList;
    State startState;
    startState.currentBoard = initialBoard;
    startState.f_cost = heuristic(initialBoard);
    openList.push(startState);
    closedList[boardToString(startState.currentBoard)] = 0;
    while (!openList.empty()) {
        State currentState = openList.top();
        openList.pop();
        int currentG = currentState.path.size();
        if (currentG > closedList[boardToString(currentState.currentBoard)]) {
            continue;
        }
        if (isSolved(currentState.currentBoard)) {
            return currentState.path;
        }
        vector<pair<int, int>> possibleMoves = getPossibleMoves(currentState.currentBoard, N);
        for (const auto& move : possibleMoves) {
            Board nextBoard = applyMove(currentState.currentBoard, move.first, move.second);
            string nextBoardString = boardToString(nextBoard);
            int nextG = currentG + 1;
            if (closedList.find(nextBoardString) == closedList.end() || nextG < closedList[nextBoardString]) {
                State nextState;
                nextState.currentBoard = nextBoard;
                nextState.path = currentState.path;
                nextState.path.push_back(move);
                nextState.f_cost = nextG + heuristic(nextBoard);
                openList.push(nextState);
                closedList[nextBoardString] = nextG;
            }
        }
    }
    return {};
}

int main() {
    int N;
    cout << "Enter the number of bottles (N): ";
    cin >> N;
    Board initialBoard(N, vector<char>(4, '.')); 
    cout << "Enter bottle configurations (example: 'AABB', 'CC..', '....'):" << endl;
    for (int i = 0; i < N; ++i) {
        string s;
        cout << "Bottle " << (i + 1) << ": ";
        cin >> s;
        for (int j = 0; j < 4; ++j) {
            initialBoard[i][j] = s[j];
        }
    }
    vector<pair<int, int>> solutions = solve(initialBoard, N);
    if (!solutions.empty()) {
        cout << "Number of steps: " << solutions.size() << endl;
        cout << "Steps:" << endl;
        for (const auto& move : solutions) {
            cout << "Move from bottle " << (move.first + 1) << " to bottle " << (move.second + 1) << endl;
        }
    } else {
        cout << "No solution found" << endl;
    }
    return 0;
}
