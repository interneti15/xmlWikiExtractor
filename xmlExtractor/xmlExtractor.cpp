#include <iostream>
#include <fstream>
#include <regex>
#include <string>

using namespace std;

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

    while (true)
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

    while (true)
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

int main() {
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
    
    string pageTitle;
    vector<string> Page;
    bool saving = false;
    size_t count = 0;

    

    std::string line;
    goto whileSkip;
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

        if (containsSubstring(line, "<text"))
        {
            saving = true;
            continue;
        }
        if (containsSubstring(line, "== See also =="))
        {
            saving = false;
        }
        if (containsSubstring(line, "</text>"))
        {
            replaceAll(line, "</text>", "");
            Page.push_back(line);

            saving = false;
            saveVectorToFile(Page, pageTitle);
            Page.clear();
            //break;
            count++;
            continue;
        }

        if (saving)
        {
            Page.push_back(line);
        }
 
    }
    
whileSkip:;


    while (std::getline(file, line))
    {
        cout << line << endl;
        system("pause");
        std::cout << "\033[F\033[K";
    }


    // Close the file
    file.close();

    return 0; // Return success
}

