#include <windows.h>
#include <iostream>
#include <string>
#include <sstream>
#include <cctype>
#include <fstream>
#include <vector>
#include <algorithm>

// Helper: Compute the greatest common divisor.
int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

// Structure to hold a resolution.
struct Resolution {
    int width;
    int height;
};

int main() {
    // Print ASCII art once.
    std::cout << "\n    ___ _           _               _____                  _      \n"
        << "   /   (_)___ _ __ | | __ _ _   _  /__   \\___   __ _  __ _| | ___ \n"
        << "  / /\\ / / __| '_ \\| |/ _` | | | |   / /\\/ _ \\ / _` |/ _` | |/ _ \\\n"
        << " / /_//| \\__ \\ |_) | | (_| | |_| |  / / | (_) | (_| | (_| | |  __/\n"
        << "/___,' |_|___/ .__/|_|\\__,_|\\__, |  \\/   \\___/ \\__, |\\__, |_|\\___|\n"
        << "             |_|            |___/              |___/ |___/        \n"
        << " _             __                   _                             \n"
        << "| |__  _   _  / _\\ ___ ___________ | | __                         \n"
        << "| '_ \\| | | | \\ \\ / _ \\_  /_  / _ \\| |/ /                         \n"
        << "| |_) | |_| | _\\ \\  __// / / / (_) |   <                          \n"
        << "|_.__/ \\__, | \\__/\\___/___/___\\___/|_|\\_\\                         \n"
        << "       |___/                                                       \n";

    // Name of the configuration file.
    const std::string configFilename = "config.txt";

    // Main loop.
    while (true) {
        // Load stored toggle resolutions (or use defaults if file not present).
        int toggleWidth1 = 2560, toggleHeight1 = 1440;
        int toggleWidth2 = 1920, toggleHeight2 = 1440;
        std::ifstream configFile(configFilename);
        if (configFile) {
            configFile >> toggleWidth1 >> toggleHeight1 >> toggleWidth2 >> toggleHeight2;
            configFile.close();
        }

        // Lambda to compute aspect ratio as string.
        auto computeAspect = [&](int w, int h) -> std::string {
            int g = gcd(w, h);
            return std::to_string(w / g) + ":" + std::to_string(h / g);
            };

        // Show current toggle settings (with aspect ratios).
        std::cout << "\nCurrent setting: "
            << toggleWidth1 << "x" << toggleHeight1 << " (" << computeAspect(toggleWidth1, toggleHeight1) << ")"
            << " <-> "
            << toggleWidth2 << "x" << toggleHeight2 << " (" << computeAspect(toggleWidth2, toggleHeight2) << ")\n\n";

        // Show main menu.
        std::cout << "-> Enter command [e],[x] or [exit] to exit\n"
            << "-> Press Enter to toggle resolution\n"
            << "-> [c] to configure\n---> ";
        std::string command;
        std::getline(std::cin, command);
        for (char& ch : command) {
            ch = static_cast<char>(std::tolower(ch));
        }

        if (command == "exit" || command == "e" || command == "x") {
            break; // Exit the program.
        }
        else if (command == "c") {
            // Configuration mode.
            std::cout << "\nConfiguration mode:\n";
            std::cout << "Available resolutions:\n";

            // Enumerate unique available resolutions.
            std::vector<Resolution> availableRes;
            DEVMODE dm = { 0 };
            dm.dmSize = sizeof(dm);
            for (int i = 0; EnumDisplaySettings(NULL, i, &dm); i++) {
                Resolution res{ dm.dmPelsWidth, dm.dmPelsHeight };
                bool found = false;
                for (const auto& r : availableRes) {
                    if (r.width == res.width && r.height == res.height) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    availableRes.push_back(res);
                }
            }
            std::sort(availableRes.begin(), availableRes.end(), [](const Resolution& a, const Resolution& b) {
                return (a.width == b.width) ? (a.height < b.height) : (a.width < b.width);
                });
            for (const auto& res : availableRes) {
                std::cout << res.width << "x" << res.height
                    << " (" << computeAspect(res.width, res.height) << ")\n";
            }
            std::cout << "\n";

            // Prompt for first resolution in "WIDTHxHEIGHT" format.
            std::cout << "Enter first toggle resolution (format: WIDTHxHEIGHT, e.g., 1920x1440): ";
            std::string resInput1;
            std::getline(std::cin, resInput1);
            resInput1.erase(std::remove_if(resInput1.begin(), resInput1.end(), ::isspace), resInput1.end());
            size_t pos1 = resInput1.find('x');
            if (pos1 == std::string::npos) {
                std::cerr << "Invalid format. Returning to main menu.\n";
                continue;
            }
            int newWidth1, newHeight1;
            try {
                newWidth1 = std::stoi(resInput1.substr(0, pos1));
                newHeight1 = std::stoi(resInput1.substr(pos1 + 1));
            }
            catch (...) {
                std::cerr << "Invalid numbers. Returning to main menu.\n";
                continue;
            }

            // Prompt for second resolution.
            std::cout << "Enter second toggle resolution (format: WIDTHxHEIGHT, e.g., 2560x1440): ";
            std::string resInput2;
            std::getline(std::cin, resInput2);
            resInput2.erase(std::remove_if(resInput2.begin(), resInput2.end(), ::isspace), resInput2.end());
            size_t pos2 = resInput2.find('x');
            if (pos2 == std::string::npos) {
                std::cerr << "Invalid format. Returning to main menu.\n";
                continue;
            }
            int newWidth2, newHeight2;
            try {
                newWidth2 = std::stoi(resInput2.substr(0, pos2));
                newHeight2 = std::stoi(resInput2.substr(pos2 + 1));
            }
            catch (...) {
                std::cerr << "Invalid numbers. Returning to main menu.\n";
                continue;
            }

            // Save the new resolutions to the config file.
            std::ofstream outFile(configFilename);
            if (!outFile) {
                std::cerr << "Error: Could not open config file for writing. Returning to main menu.\n";
                continue;
            }
            outFile << newWidth1 << " " << newHeight1 << " "
                << newWidth2 << " " << newHeight2;
            outFile.close();
            std::cout << "Toggle resolutions updated to "
                << newWidth1 << "x" << newHeight1 << " (" << computeAspect(newWidth1, newHeight1) << ")"
                << " and "
                << newWidth2 << "x" << newHeight2 << " (" << computeAspect(newWidth2, newHeight2) << ").\n";

            // Immediately set the display resolution to the first configured resolution.
            DEVMODE newMode = { 0 };
            newMode.dmSize = sizeof(newMode);
            newMode.dmPelsWidth = newWidth1;
            newMode.dmPelsHeight = newHeight1;
            newMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
            LONG changeResult = ChangeDisplaySettingsEx(NULL, &newMode, NULL, CDS_UPDATEREGISTRY, NULL);
            if (changeResult != DISP_CHANGE_SUCCESSFUL) {
                MessageBox(NULL, L"Failed to change display settings.", L"Error", MB_OK | MB_ICONERROR);
            }
            else {
                std::cout << "Display resolution set to "
                    << newWidth1 << "x" << newHeight1 << " (" << computeAspect(newWidth1, newHeight1) << ").\n";
            }
            // After configuration and setting the resolution, exit immediately.
            break;
        }
        else if (!command.empty()) {
            std::cerr << "Invalid command. Returning to main menu.\n";
            continue;
        }

        // If the command is empty, then toggle the resolution.
        DEVMODE devMode = { 0 };
        devMode.dmSize = sizeof(DEVMODE);
        if (!EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devMode)) {
            MessageBox(NULL, L"Failed to get current display settings.", L"Error", MB_OK | MB_ICONERROR);
            continue;
        }

        int currentWidth = devMode.dmPelsWidth;
        int currentHeight = devMode.dmPelsHeight;
        int targetWidth = 0, targetHeight = 0;

        if (currentWidth == toggleWidth1 && currentHeight == toggleHeight1) {
            targetWidth = toggleWidth2;
            targetHeight = toggleHeight2;
        }
        else if (currentWidth == toggleWidth2 && currentHeight == toggleHeight2) {
            targetWidth = toggleWidth1;
            targetHeight = toggleHeight1;
        }
        else {
            MessageBox(NULL, L"Current resolution is not supported by this toggle.", L"Error", MB_OK | MB_ICONERROR);
            continue;
        }

        devMode.dmPelsWidth = targetWidth;
        devMode.dmPelsHeight = targetHeight;
        devMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

        LONG result = ChangeDisplaySettingsEx(NULL, &devMode, NULL, CDS_UPDATEREGISTRY, NULL);
        if (result != DISP_CHANGE_SUCCESSFUL) {
            MessageBox(NULL, L"Failed to change display settings.", L"Error", MB_OK | MB_ICONERROR);
        }
        else {
            std::cout << "Display resolution changed to "
                << targetWidth << "x" << targetHeight << " ("
                << computeAspect(targetWidth, targetHeight) << ").\n";
        }
        // After toggling, exit immediately.
        break;
    }

    return 0;
}
