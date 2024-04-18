#define  _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS true
#include <filesystem>
#include <chrono>
#include <codecvt>
#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <windows.h>
#include <codecvt>
#include <ShlObj.h>
#include <thread>

using namespace std;

//string folderPath;

vector<string> toIgnoreStarts = {
    "* [[",
    "{{",
    "[[File:",
    ":",
    "+&",
    "&",
    "# ",
    "{|",
    "!",
    "<;sup>",
    "|",
    "\alpha",
    "[[Image:",
    "}}",
    "",
    "",
    "",
    ""
};

vector<string> replaceStr = {
    "&lt", "<",
    "&gt", ">",
    "<;sup>", "",
    "<;/sup>;", "",
    "<;sub>;", "",
    "<;/sub>;", ""

};

vector<string> deleteBetween = {
    "<;ref","/>;",
    "<;ref", ";/ref>",
    "{{","}}"
};

vector<string> deleteFromFilename = {
    "#","%","&","{","}","\\","< ",">","*","?","/","$","!","`","'", 
    "\"",":","@","+","|","="
};





bool isValidFormat(const std::string& input, const std::string& argumentName) {
    // Remove leading and trailing whitespaces
    std::string trimmedInput = input;
    trimmedInput.erase(0, trimmedInput.find_first_not_of(" \t\n\r"));
    trimmedInput.erase(trimmedInput.find_last_not_of(" \t\n\r") + 1);

    // Construct the regex pattern dynamically based on the provided argument name
    std::string patternString = "<" + argumentName + ">([^<]+)</" + argumentName + ">";
    std::regex pattern(patternString);

    // Check if the input matches the regex pattern
    return std::regex_match(trimmedInput, pattern);
}

string replaceAll(std::string& str, const std::string& oldStr, const std::string& newStr) {
    size_t pos = 0;
    while ((pos = str.find(oldStr, pos)) != std::string::npos) {
        str.replace(pos, oldStr.length(), newStr);
        pos += newStr.length();
    }
    return str;
}

string removeWhiteSpaces(string& str) {

    while (!str.empty())
    {
        if (str[0] == ' ')
        {
            str.erase(0, 1);
        }
        else
        {
            break;
        }
    }

    while (!str.empty())
    {
        if (str[str.size() - 1] == ' ')
        {
            str.erase(str.size() - 1, 1);
        }
        else
        {
            break;
        }
    }

    return str;
}

bool containsSubstring(const std::string& mainStr, const std::string& subStr) {
    return mainStr.find(subStr) != std::string::npos;
}

void saveVectorToFile(const std::vector<std::string>& strings, std::string filename, const std::string& folderPath) {
    // Hardcoded path where files will be saved
    std::string path = folderPath;

    // Appending the path to the filename
    
    for (string illegal : deleteFromFilename) {
        replaceAll(filename, illegal, "");
    }

    filename = path + filename + ".txt";
    
    // Open the file
    std::ofstream outputFile(filename);

    if (outputFile.is_open()) {
        for (const std::string& str : strings) {
            outputFile << str << std::endl;
        }
        outputFile.close();
        //std::cout << "Successfully saved to " << filename << std::endl;
    }
    else {
        std::cerr << "Unable to save: " << filename << std::endl;
    }
}

bool startCheck(string mainStr, string subStr) {

    if (mainStr.size() < subStr.size() || mainStr.empty())
    {
        return false;
    }

    for (size_t i = 0; i < subStr.size(); i++)
    {      
        if (mainStr[i] != subStr[i])
        {
            return false;
        }
    }
    
    return true;
}

bool substringAtPos(const std::string& str, const std::string& substr, std::size_t pos) {
    if (pos + substr.length() > str.length()) {
        // If the substring is longer than the remaining characters in the string, return false
        return false;
    }
    // Check if the substring matches at the specified position
    return str.substr(pos, substr.length()) == substr;
}

void updateProgressBar(double progress) {
    int barWidth = 70;

    std::cout << "[";
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << " %\r";
    std::cout.flush();
}

//Should the line be added as text
bool lineProcces(string& line) {

    bool addLine = true;
    bool readLine = false;
    string tempStr, whole;

    removeWhiteSpaces(line);

    if (line.empty())
    {
        return addLine;
    }

    for (const string toCheck: toIgnoreStarts)
    {
        if (toCheck.empty())
        {
            continue;
        }
        if (startCheck(line,toCheck))
        {
            return false;
        }
    }

    for (size_t i = 0; i < replaceStr.size(); i+=2)
    {
        replaceAll(line, replaceStr[i], replaceStr[i+1]);
    }

    string lineCopy = line;

    for (size_t i = 0; i < deleteBetween.size(); i += 2)
    {
        size_t bigger = (deleteBetween[i].size() > deleteBetween[i + 1].size()) ? deleteBetween[i].size() : deleteBetween[i + 1].size();

        if (bigger > line.size())
        {
            continue;
        }

        for (size_t j = 0; j < (int)(line.size()) - deleteBetween[i + 1].size() + 1; j++)
        {
            if (substringAtPos(line, deleteBetween[i], j))
            {
                readLine = true;
                j += deleteBetween[i].size() - 1;
            }
            else if (substringAtPos(line, deleteBetween[i+1], j))
            {
                //printf("%s \n", whole.c_str());
                whole = deleteBetween[i] + whole + deleteBetween[i + 1];
                replaceAll(lineCopy, whole, "");
                readLine = false;
                whole.clear();
            }
            else if (readLine)
            {
                whole += line[j];
            }
        }
        readLine = false;
        whole.clear();
    }
    line = lineCopy;

    //return addLine;
    lineCopy = line;
    //string last;
    tempStr.clear();
    whole.clear();

    readLine = false;
    for (int i = 0; i < (int)(line.size()) - 1; i++)
    {
        
        if (substringAtPos(line,"[[",i))
        {
            readLine = true;
            i += 1;
        }
        else if (substringAtPos(line, "]]", i))
        {
            whole = "[[" + whole + "]]";
            //cout << whole << endl;
            replaceAll(lineCopy, whole, tempStr);
            readLine = false;
            whole.clear();
            tempStr.clear();
        }
        else if (line[i] == '|' && readLine)
        {
            whole += line[i];
            //last = tempStr;
            tempStr.clear();
        }
        else if(readLine)
        {
            tempStr += line[i];
            whole += line[i];
        } 
    }
    line = lineCopy;

    for (const string toCheck : toIgnoreStarts)
    {
        if (toCheck.empty())
        {
            continue;
        }
        if (startCheck(line, toCheck))
        {
            return false;
        }
    }

    return addLine;
}

void test() {

    string str = " In geometry, an '''{{nowrap|{{mvar|n}}-gonal}} antiprism''' or {{nowrap|'''{{mvar|n}}-antiprism'''}} is a polyhedron composed of two parallel direct copies (not mirror images) of an {{nowrap|{{mvar|n}}-sided}} polygon, connected by an alternating band of  triangles. They are represented by the Conway notation .";
    //13
    cout << str << endl << endl;

    lineProcces(str);

    cout << str << endl << endl;

    lineProcces(str);

    cout << str << endl << endl;

    lineProcces(str);

    cout << str << endl << endl;

}

std::size_t utf8_len(const std::string& str) {
    std::wstring_convert<codecvt_utf8<char32_t>, char32_t> conv;
    std::u32string u32str = conv.from_bytes(str);
    return u32str.size() * sizeof(char32_t);
}

string getPath() {
    OPENFILENAME ofn;       // common dialog box structure
    wchar_t szFile[260] = { 0 };       // if using TCHAR and Unicode strings

    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    //ofn.lpstrFilter = L"C++ Files\0*.cpp\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    // Display the Open File dialog and retrieve the filename from the Open File dialog.
    std::wstring filePath;
    if (GetOpenFileNameW(&ofn) == TRUE)
    {
        filePath = ofn.lpstrFile;
        std::wcout << L"Selected file: " << filePath << std::endl;
    }
    else
    {
        std::wcout << L"No file selected." << std::endl;
        throw exception("123");
    }

    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::string narrowString = converter.to_bytes(filePath);

    return narrowString;
}

std::string getFolderPath() {
    BROWSEINFOW bi = { 0 };
    bi.lpszTitle = L"Select a folder"; // Dialog title
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE; // Restrict selection to folders

    wchar_t szDir[MAX_PATH] = { 0 }; // Buffer to store selected folder path

    PIDLIST_ABSOLUTE pidl = SHBrowseForFolderW(&bi); // Show folder selection dialog

    if (pidl != NULL) {
        if (SHGetPathFromIDListW(pidl, szDir) == TRUE) { // Get selected folder path
            std::wstring folderPath(szDir);
            std::wcout << L"Selected folder: " << folderPath << std::endl;

            // Convert wide string to narrow string
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            std::string narrowString = converter.to_bytes(folderPath);

            return narrowString; // Return narrow string
        }
        else {
            std::wcout << L"Failed to retrieve folder path." << std::endl;
            return ""; // Return empty string on failure
        }

        CoTaskMemFree(pidl); // Free allocated memory for PIDL
    }
    else {
        std::wcout << L"No folder selected." << std::endl;
        throw std::exception("No folder selected."); // Throw exception if no folder selected
    }
}

string pickAPath() {
    std::string filePath;
    while (true)
    {
        try
        {
            filePath = getPath();

            std::ifstream file(filePath);

            if (!file.is_open()) {
                std::cerr << "Error: Unable to open file " << filePath << std::endl;
                throw;
            }
            file.close();
            break;
        }
        catch (const std::exception& e)
        {
            cout << "Bad file path" << endl;
            printf("Do you wish to exit program? \n [y] - yes \n [n] - no \n");
            string tstr;
            cin >> tstr;
            if (tstr == "y")
            {
                return 0;
            }
        }
    }

    return filePath;
}

string pickAFolder() {
    string folderPath;
    while (true)
    {
        try
        {
            folderPath = getFolderPath() + "\\";
            //cout << folderPath << endl;
            //return 1;
            break;
        }
        catch (const std::exception& e)
        {
            cout << "Bad file path" << endl;
            printf("Do you wish to exit program? \n [y] - yes \n [n] - no \n");
            string tstr;
            cin >> tstr;
            if (tstr == "y")
            {
                return 0;
            }
        }
    }
    return folderPath;
}



int getDataFromXml(const std::string filePath, const string folderPath, const int ID, const int numberOfThreads, size_t* pageCountPtr, vector<bool>* running) {
    

    std::ifstream file(filePath);
    file.seekg(0, std::ios::end);
    size_t startPos = ((size_t)file.tellg() / numberOfThreads)*ID;
    size_t endPos = ((size_t)file.tellg() / numberOfThreads) * (ID+1);
    file.seekg(startPos, std::ios::beg);
    string line;

    string pageTitle;
    vector<string> Page;
    bool saving = false;
    bool redirect = false;

    bool lastArticle = false;
    //std::string line;
    //goto whileSkip;

    while (true)
    {
        getline(file, line);
        if (isValidFormat(line, "title")) 
        {
            goto found;
        }
    }

    while (std::getline(file, line))
    {
        
        if (isValidFormat(line, "title"))
        {
        found:;
            removeWhiteSpaces(line);
            replaceAll(line, "<title>", "");
            replaceAll(line, "</title>", "");
            //cout << line << endl;
            pageTitle = line;
            continue;
        }

        if (containsSubstring(line, "<text") && containsSubstring(line, "#REDIRECT [["))
        {
            //cout << "Skipping: " << pageTitle << endl;
            redirect = true;
            continue;
            
        }
        if (containsSubstring(line, "<text"))
        {
            redirect = false;
            saving = true;
            continue;
        }
        if (containsSubstring(line, "== See also ==") || containsSubstring(line, "==See also==") || containsSubstring(line, "== Notes =="))
        {
            saving = false;
        }
        if (containsSubstring(line, "</text>"))
        {

            saving = false;
            if (!redirect)
            {
                if (!Page.empty())
                {
                    saveVectorToFile(Page, pageTitle, folderPath);
                    (*pageCountPtr)++;
                }
            }
            Page.clear();

            if (lastArticle)
            {
                break;
            }
            if (file.tellg() > endPos)
            {
                lastArticle = true;
            }

            continue;
        }

        if (saving)
        {
            //system("cls");
            //cout << line << endl << endl;
            //system("pause");
            if (true || lineProcces(line))
            {
                //cout << line << endl << endl;
                //lineCount++;
                //lineProcces(line);
                Page.push_back(line);
            }
            else {
                //cout << "None..." << endl << endl;
            }
        }
 
    }

    file.close();
    running->at(ID).flip();

    return 0; // Return success

whileSkip:;
    const size_t totalBytes = 96ULL * 1024ULL * 1024ULL * 1024ULL;
    size_t bytesRead = 0;
    double progress = 0.0;
    size_t Last = 10, sizeCounter = 0;

    // Read lines from the file
    while (std::getline(file, line)) {
        // Update progress bar

        if (sizeCounter > Last)
        {
            //cout << sizeCounter << endl;
            Last *= 1.5;

            
            progress = static_cast<double>(bytesRead) / totalBytes;
            updateProgressBar(progress);
        }
        sizeCounter++;
        bytesRead += utf8_len(line); // +1 for newline character
        
    }
    std::cout << std::endl << "Total lines read: " << bytesRead << std::endl;

    file.close();
}

std::vector<std::string> getFilesInFolder(const std::string& folderPath) {
    std::vector<std::string> files;

    try {
        for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
            if (std::filesystem::is_regular_file(entry.path())) {
                files.push_back(entry.path().string());
            }
        }
    }
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }

    return files;
}

void threadDataExtracionManager(const std::string& filePath, const string& folderPath) {

    const int threadCount = 10;
    vector<thread> threadsList(threadCount);
    size_t articleCount = 0;
    size_t lastCounter = 0;
    double percent = 0.0;
    auto start = std::chrono::steady_clock::now();
    double time_remaining = 0.0;

    vector<bool> running(threadCount);
    bool allFinished = true;

    for (size_t i = 0; i < threadCount; i++)
    {
        threadsList[i] = thread(getDataFromXml, filePath, folderPath, i, threadCount, &articleCount, &running);
        running[i] = true;
    }

    while (true)
    {
        if (articleCount > lastCounter + 30)
        {
            //system("cls");
            percent = ((double)articleCount / 6812260) * 100;
            cout << endl << "Article count: " << articleCount << endl;
            cout << "Percent estimation: " << percent << "%" << endl;

            const auto end = std::chrono::steady_clock::now();
            const auto duration_msecs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

            time_remaining = (100 - percent) / (percent / duration_msecs.count());
            
            printf("Time elapsed: %lld seconds. \n", duration_msecs.count() / 1000);

            printf("Estimated time of completion: %f days, %f hours, %f minutes, %f seconds \n\n", time_remaining / (1000 * 60 * 60 * 24), time_remaining / (1000 * 60 * 60), time_remaining / (1000 * 60), time_remaining / (1000));
            lastCounter = articleCount;

            

        }
        allFinished = true;
        for (size_t i = 0; i < threadCount; i++)
        {
            if (running[i])
            {
                allFinished = false;
            }
        }
        if (allFinished)
        {
            break;
        }
    }

    for (size_t i = 0; i < threadCount; i++)
    {
        threadsList[i].join();
    }
    cout << "Finished" << endl;
}

int txtProcessThreadManager(string& folderPath) {
    folderPath += "\\";
    std::vector < std::string> paths = getFilesInFolder(folderPath);
    for (string i : paths) {
        cout << i << endl;
    }

    return 1;
}

int main() {

    string tstr;
    printf("Welcome to wikiExtractor \n Choose what you want to do:\n[0] - Extract data from .xml to .txt\n[1] - Process .txt (delete hyperlink etc...)\n");
    cin >> tstr;
    string filePath, folderpath;
    switch (tstr[0])
    {
    default:
        break;
    case '0':
        
        system("cls");
        printf("Choose your wikipedia dump.xml location!\n");
        filePath = pickAPath();
        //system("cls");
        printf("Choose your destination folder!\n");
        folderpath = pickAFolder();
        system("cls");

        threadDataExtracionManager(filePath, folderpath);
        //getDataFromXml(filePath, folderpath, 1, 2);
        break;
    case '1':
        system("cls");
        printf("Choose your destination folder!\n");
        folderpath = pickAFolder();
        txtProcessThreadManager(folderpath);
        break;
    }


    return 0;
}

