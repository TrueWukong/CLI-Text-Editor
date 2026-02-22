#include <iostream>   // Allows input and output (cin, cout)
#include <fstream>    // Allows file handling (ifstream, ofstream)
#include <vector>     // Allows dynamic array (vector)
#include <stack>      // Allows stack data structure
#include <string>     // Allows string usage

using namespace std;  // So I don't have to write std:: every time

// Structure that represents a single editing action
struct Action {
    string type;        // Types of action I want to implement: "insert", "delete", "replace"
    int lineNumber;     // Line where action occurred (0-based index)
    string oldText;     // Stores old text (used for undo)
    string newText;     // Stores new text (used for redo)
};

// Stores all lines of the file
vector<string> lines;

// Stack for undo operations
stack<Action> undoStack;

// Stack for redo operations
stack<Action> redoStack;

// Displays all text with line numbers
void printText() {
    for (size_t i = 0; i < lines.size(); i++) {
        cout << i + 1 << ": " << lines[i] << endl;
    }
}

// Opens a file and loads it into memory
void openFile(const string& filename) {
    ifstream file(filename);   // Open file for reading
    lines.clear();             // Clear current text

    if (!file) {               // If file fails to open
        cout << "Error: Cannot open file\n";
        return;
    }

    string line;
    while (getline(file, line)) {  // Read file line by line
        lines.push_back(line);
    }

    file.close();              // Close file
    cout << "File loaded successfully.\n";
}

// Saves current text to file
void saveFile(const string& filename) {
    ofstream file(filename);   // Open file for writing

    for (const string& line : lines) {
        file << line << endl;  // Write each line
    }

    file.close();              // Close file
    cout << "File saved.\n";
}

// Inserts a line at a specific position
void insertLine(int pos, const string& text) {
    if (pos < 1 || pos > lines.size() + 1) {
        cout << "Invalid line number\n";
        return;
    }

    Action action = {"insert", pos - 1, "", text};
    undoStack.push(action);        // Save action for undo

    while (!redoStack.empty())     // Clear redo history
        redoStack.pop();

    lines.insert(lines.begin() + pos - 1, text);
}

// Deletes a line
void deleteLine(int pos) {
    if (pos < 1 || pos > lines.size()) {
        cout << "Invalid line number\n";
        return;
    }

    Action action = {"delete", pos - 1, lines[pos - 1], ""};
    undoStack.push(action);

    while (!redoStack.empty())
        redoStack.pop();

    lines.erase(lines.begin() + pos - 1);
}

// Replaces first occurrence of a word
void replaceText(const string& oldWord, const string& newWord) {
    for (size_t i = 0; i < lines.size(); i++) {

        size_t found = lines[i].find(oldWord);

        if (found != string::npos) {

            Action action = {"replace", (int)i, lines[i], ""};
            undoStack.push(action);

            while (!redoStack.empty())
                redoStack.pop();

            lines[i].replace(found, oldWord.length(), newWord);

            action.newText = lines[i];  // Store updated version
            undoStack.top() = action;   // Update action in stack

            cout << "Replaced in line " << i + 1 << endl;
            return;
        }
    }

    cout << "Word not found.\n";
}

// Finds and prints lines containing a word
void findWord(const string& word) {
    bool found = false;

    for (size_t i = 0; i < lines.size(); i++) {
        if (lines[i].find(word) != string::npos) {
            cout << "Found at line " << i + 1 << ": " << lines[i] << endl;
            found = true;
        }
    }

    if (!found)
        cout << "Word not found.\n";
}

// Undo last action
void undo() {

    if (undoStack.empty()) {
        cout << "Nothing to undo\n";
        return;
    }

    Action action = undoStack.top();
    undoStack.pop();

    redoStack.push(action);

    if (action.type == "insert") {
        lines.erase(lines.begin() + action.lineNumber);
    }
    else if (action.type == "delete") {
        lines.insert(lines.begin() + action.lineNumber, action.oldText);
    }
    else if (action.type == "replace") {
        lines[action.lineNumber] = action.oldText;
    }

    cout << "Undo completed.\n";
}

// Redo last undone action
void redo() {

    if (redoStack.empty()) {
        cout << "Nothing to redo\n";
        return;
    }

    Action action = redoStack.top();
    redoStack.pop();

    undoStack.push(action);

    if (action.type == "insert") {
        lines.insert(lines.begin() + action.lineNumber, action.newText);
    }
    else if (action.type == "delete") {
        lines.erase(lines.begin() + action.lineNumber);
    }
    else if (action.type == "replace") {
        lines[action.lineNumber] = action.newText;
    }

    cout << "Redo completed.\n";
}

// Main program loop
int main() {

    string command;

    cout << "Mini CLI Text Editor\n";
    cout << "Commands:\n";
    cout << "open filename\n";
    cout << "save filename\n";
    cout << "insert lineNumber text\n";
    cout << "delete lineNumber\n";
    cout << "find word\n";
    cout << "replace oldWord newWord\n";
    cout << "undo\nredo\nprint\nexit\n";

    while (true) {

        cout << ">> ";
        getline(cin, command);

        if (command == "exit")
            break;

        else if (command.rfind("open ", 0) == 0)
            openFile(command.substr(5));

        else if (command.rfind("save ", 0) == 0)
            saveFile(command.substr(5));

        else if (command.rfind("insert ", 0) == 0) {
            size_t space = command.find(' ', 7);
            int lineNum = stoi(command.substr(7, space - 7));
            string text = command.substr(space + 1);
            insertLine(lineNum, text);
        }

        else if (command.rfind("delete ", 0) == 0)
            deleteLine(stoi(command.substr(7)));

        else if (command.rfind("find ", 0) == 0)
            findWord(command.substr(5));

        else if (command.rfind("replace ", 0) == 0) {
            size_t space = command.find(' ', 8);
            string oldWord = command.substr(8, space - 8);
            string newWord = command.substr(space + 1);
            replaceText(oldWord, newWord);
        }

        else if (command == "undo")
            undo();

        else if (command == "redo")
            redo();

        else if (command == "print")
            printText();

        else
            cout << "Unknown command\n";
    }

    return 0;
}
