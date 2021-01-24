#include <json.hpp>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <io.h>

using json = nlohmann::json;
using namespace std;

string timestamp_fmt(string utc_time) {
    vector<int> month31 = {1,3,5,7,8,10};
    bool leapYear = false;
    // 2020-04-26T16:12:38.104922+00:00
    int year = stoi(utc_time.substr(0,4));
    int month = stoi(utc_time.substr(5,2));
    int day = stoi(utc_time.substr(8,2));
    int hour = stoi(utc_time.substr(11,2))+5;
    int minute = stoi(utc_time.substr(14,2))+30;
    int second = stoi(utc_time.substr(17,2));
    if (minute > 60) {
        minute -= 60;
        hour += 1;
    }
    if (hour > 23) {
        hour -= 23;
        day += 1;
    }
    leapYear = (year % 1000 != 0 && year % 4 == 0) || (year % 1000 == 0 && year % 400 == 0);
    if (month == 2) {
        if (leapYear) {
            if (day > 29) {
                day -= 28;
                month += 1;
            }
        } else {
            if (day > 28) {
                day -= 27;
                month += 1;
            }
        }
    } else if (month == 12) {
        if (day > 31) {
            day -= 30;
            month -= 11;
            year += 1;
        }
    } else if (find(month31.begin(), month31.end(), month) != month31.end()) {
        if (day > 31) {
            day -= 30;
            month += 1;
        }
    }
    string ist_time = "";
    ist_time += to_string(year) + "-";
    if (month < 10) {
        ist_time += "0";
    }
    ist_time += to_string(month) + "-";
    if (day < 10) {
        ist_time += "0";
    }
    ist_time += to_string(day) + " ";
    if (hour < 10) {
        ist_time += "0";
    }
    ist_time += to_string(hour) + ":";
    if (minute < 10) {
        ist_time += "0";
    }
    ist_time += to_string(minute) + ":";
    if (second < 10) {
        ist_time += "0";
    }
    ist_time += to_string(second);
    return ist_time;
}

int main() {
    string rawJson = "", x;
    ifstream rawFile("messages.json");
    while(getline(rawFile, x)) {
        rawJson += x;
    }
    auto j = json::parse(rawJson);
    mkdir("./conversations");
    
    for (auto& convItr : json::iterator_wrapper(j)) {
        int convIndex = stoi(convItr.key());
        string fileName = "conversations/conversation_"+to_string(convIndex+1)+".txt";
        fstream convFile;
        convFile.open(fileName, ios::out);
        auto convJson = j[convIndex]["conversation"];

        for (auto convText  : json::iterator_wrapper(convJson)) {
            int convReverseIndex = convJson.size() - stoi(convText.key()) - 1;
            auto textJson = convJson[convReverseIndex];
            string textTimestamp = timestamp_fmt(string(textJson["created_at"]));
            
            convFile << textTimestamp;
            convFile << "   " << string(textJson["sender"]) << ":";
            if (textJson["story_share"] != nullptr) {
                convFile << " " << string(textJson["story_share"]);
            }
            if (textJson["text"] != nullptr) {
                convFile << " " << string(textJson["text"]);
            }
            if (textJson["media"] != nullptr) {
                convFile << " " << string(textJson["media"]);
            }
            if (textJson["media_share_url"] != nullptr) {
                convFile << " " << string(textJson["media_share_url"]);
            }
            if (textJson["likes"] != nullptr) {
                convFile << " [Liked by ";
                int ls = textJson["likes"].size();
                for (int i = 0; i < ls; i++) {
                    if (i == ls - 1 && ls > 1) {
                        convFile << " and ";
                    } else if (i > 0) {
                        convFile << ", ";
                    }
                   convFile << textJson["likes"][i]["username"];

                }
                convFile << "]";
            }
            convFile << "\n";
        }

    }
}