#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>

bool write_to_pot(std::filesystem::path path, const std::vector<std::pair<std::string, std::string>>& data)
{
	std::ofstream file(path, std::ios_base::app);
	for (const auto& element : data)
	{
		//Example of .pot element 
		//#: src/slic3r/GUI/GUI_App.cpp:1647 src/slic3r/GUI/wxExtensions.cpp:687
		//msgctxt "Mode"
		//msgid "Advanced"
		//msgstr ""
		file << "\n#: resources/data/hints.ini: ["<< element.first << "]\nmsgid \"" << element.second << "\"\nmsgstr \"\"\n";
	}
	file.close();
	return true;
}
bool read_hints_ini(const std::filesystem::path& path, std::vector<std::pair<std::string, std::string>>& pot_elements)
{
	boost::property_tree::ptree tree;
	std::ifstream ifs(path.string());
	try {
		boost::property_tree::read_ini(ifs, tree);
	}
	catch (const boost::property_tree::ini_parser::ini_parser_error& err) {
		std::cout << err.what() << std::endl;
		return false;
	}
	for (const auto& section : tree) {
		if (section.first.rfind("hint:", 0) == 0) {
			for (const auto& data : section.second) {
				if (data.first == "text")
				{
					pot_elements.emplace_back(section.first, data.second.data());
					break;
				}
			}
		}
	}
	return true;
}

int main(int argc, char* argv[])
{
	std::vector<std::pair<std::string, std::string>> data;
	std::filesystem::path path_to_ini;
	std::filesystem::path path_to_pot;
	if (argc != 3)
	{
		std::cout << "HINTS_TO_POT FAILED: WRONG NUM OF ARGS" << std::endl;
		return -1;
	}
	try {
		path_to_ini = std::filesystem::canonical(argv[1]).parent_path() / "resources" / "data" / "hints.ini";
		path_to_pot = std::filesystem::canonical(argv[2]).parent_path() / "i18n" /"OrcaSlicer.pot";
	} catch (std::exception&) {
		std::cout << "HINTS_TO_POT FAILED: STD CANNONICAL" << std::endl;
		return -1;
	}
	
	if (!std::filesystem::exists(path_to_ini)){
		std::cout << "HINTS_TO_POT FAILED: PATH TO INI DOES NOT EXISTS" << std::endl;
		std::cout << path_to_ini.string() << std::endl;
		return -1;
	}
	if (!read_hints_ini(std::move(path_to_ini), data)) {
		std::cout << "HINTS_TO_POT FAILED TO READ HINTS INI" << std::endl;
		return -1;
	}
	if (!write_to_pot(std::move(path_to_pot), data)) {
		std::cout << "HINTS_TO_POT FAILED TO WRITE POT FILE" << std::endl;
		return -1;
	}
	std::cout << "HINTS_TO_POT SUCCESS" << std::endl;
    return 0;
}
