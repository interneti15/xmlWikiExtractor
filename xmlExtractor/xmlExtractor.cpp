#include <chrono>
#include <codecvt>
#include <iostream>
#include <fstream>
#include <regex>
#include <string>

using namespace std;

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

void saveVectorToFile(const std::vector<std::string>& strings, std::string filename) {
    // Hardcoded path where files will be saved
    std::string path = "I:\\wikitxt\\";

    // Appending the path to the filename
    filename = path + filename + ".txt";
    replaceAll(filename,"/","_");

    // Open the file
    std::ofstream outputFile(filename);

    if (outputFile.is_open()) {
        for (const std::string& str : strings) {
            outputFile << str << std::endl;
        }
        outputFile.close();
        std::cout << "Successfully saved to " << filename << std::endl;
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
                printf("%s \n", whole.c_str());
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

int main() {

    test();
    return 1;
    size_t lineCount = 0, pageCount = 0;
    // Path to the XML file
    std::string filePath = "I:\\enwiki-20231220-pages-articles-multistream.xml";

    // Open the XML file
    std::ifstream file(filePath);

    // Check if the file is opened successfully
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << filePath << std::endl;
        return 1; // Return error code
    }

    // Read and output the contents of the XML file
    auto start = std::chrono::steady_clock::now();
    string pageTitle;
    vector<string> Page;
    bool saving = false;
    double count = 0;
    bool redirect = false;
    int lastCounter = 0;
    double percent = 0.0;
    double time_remaining = 0.0;
    std::string line;
    //goto whileSkip;
    while (std::getline(file, line))
    {
        
        if (isValidFormat(line, "title")){
        
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
                saveVectorToFile(Page, pageTitle);
                count++;
                if (count > lastCounter + 17)
                {
                    percent = (count / 6812260)*100;
                    cout << endl << "Article count: " << count << " Relevant lines count: " << lineCount << endl;
                    cout << "Percent estimation: " << percent << "%" << endl;

                    const auto end = std::chrono::steady_clock::now();
                    const auto duration_msecs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

                    time_remaining = (100 - percent) / (percent / duration_msecs.count());
                    printf("Time elapsed: %lld seconds. \n", duration_msecs.count()/1000);

                    printf("Estimated time of completion: %f days, %f hours, %f minutes, %f seconds \n\n", time_remaining/(1000*60*60*24), time_remaining / (1000 * 60 * 60), time_remaining / (1000 * 60), time_remaining / (1000));
                    lastCounter = count;
                }
            }
            Page.clear();
            continue;
        }

        if (saving)
        {
            //system("cls");
            //cout << line << endl << endl;
            //system("pause");
            if (lineProcces(line))
            {
                //cout << line << endl << endl;
                lineCount++;
                //lineProcces(line);
                Page.push_back(line);
            }
            else {
                //cout << "None..." << endl << endl;
            }
        }
 
    }

    file.close();

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

