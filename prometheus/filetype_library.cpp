#include "filetype_library.h"
#include <imgui.h>
#include "game.h"
#include "window_manager/window_manager.h"
#include <fast-cpp-csv-parser/csv.h>
#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>

namespace filetype_library {
	std::map<int, char*> library;
	bool will_save = false;

	/*                class trim_policy = trim_chars<' ', '\t'>,
                class quote_policy = no_quote_escape<','>,
                class overflow_policy = throw_on_overflow,
*/
	typedef typename io::CSVReader<4, io::trim_chars<' ', '\t'>, io::no_quote_escape<','>, io::throw_on_overflow, io::single_and_empty_line_comment<';'>> csv;
	std::unique_ptr<csv> names_file;
	//file_type, file_id
	std::map<std::pair<int, int>, std::string> file_names{};

	const char* filename = "filetypes.json";
	void init() {
		try {
			nlohmann::json json;
			{
				std::ifstream input_file(filename);
				if (!input_file.is_open()) {
					printf("Failed to open input filetype file.");
					return;
				}
				json = nlohmann::json::parse(input_file);
			}
			if (!json.is_object()) {
				return;
			}

			for (auto it = json.begin(); it != json.end(); ++it) {
				int type = strtoll(it.key().c_str(), nullptr, 16);
				if (type == 0) {
					continue;
				}
				auto value = it.value().get<std::string>();
				char* pvalue = new char[256];
				strcpy(pvalue, value.c_str());

				library.emplace(type, pvalue);
			}

			will_save = true;
		}
		catch (nlohmann::json::exception ex) {
			printf("Failed to open filetypes.json. Saving is disabled. JSON Error: %s\n", ex.what());
		}
		catch (std::exception ex) {
			printf("Failed to open filetypes.json. Saving is disabled. Error: %s\n", ex.what());
		}
		catch (...) {
			printf("Failed to open filetypes.json. Saving is disabled.\n");
		}
	}

	void save_filetypes() {
		if (!will_save) {
			imgui_helpers::messageBox("Saving filetypes is disabled.", "Hashlib");
			return;
		}
		try {
			nlohmann::json result{};
			for (auto& hash : library) {
				char buf[16];
				sprintf_s(buf, "%x", hash.first);
				result.emplace(buf, hash.second);
			}
			std::ofstream fstream(filename, std::ios::out | std::ios::trunc);
			if (!fstream.is_open()) {
				imgui_helpers::messageBox("Filestream open error", "Hashlib");
				return;
			}
			fstream << result.dump(4);
			fstream.flush();
		}
		catch (nlohmann::json::exception ex) {
			printf("Failed to save filetypes.json. Saving is disabled. JSON Error: %s\n", ex.what());
		}
		catch (std::exception ex) {
			printf("Failed to save filetypes.json. Saving is disabled. Error: %s\n", ex.what());
		}
		catch (...) {
			printf("Failed to save filetypes.json. Saving is disabled.\n");
		}
	}

	void display_filetype(__int64 filetype) {
		ImGui::Text("%p", filetype);
		
		ImGui::SameLine();
		char buf[64];
		sprintf_s(buf, "Copy##%p", filetype);
		if (ImGui::Button(buf)) {
			imgui_helpers::openCopyWindow(filetype);
		}
		memset(buf, 0, sizeof(buf));
		ImGui::SameLine();
		sprintf(buf, "Copy Search##%p", filetype);
		if (ImGui::Button(buf)) {
			imgui_helpers::openCopyWindow(filetype);
		}

		int type = bitswap(16 * (filetype & 0xFFFF000000000000uLL)) + 1;
		int fileid = filetype & 0xFFFF;
		try {
			if (!names_file) {
				names_file = std::make_unique<csv>("names.csv");
				names_file->read_header(io::ignore_extra_column, "GUID Index", "GUID Type", "Name", "Canonical");
				std::string file_id_s, filetype_s, name;
				int canonical;
				while (names_file->read_row(file_id_s, filetype_s, name, canonical)) {
					int file_id = (int)strtol(file_id_s.c_str(), NULL, 16);
					int filetype = (int)strtol(filetype_s.c_str(), NULL, 16);
					file_names.emplace(std::pair<int, int>{ filetype, file_id }, name + " (Canonical: " + (canonical ? "Yes" : "No") + ")");
				}
			}
		}
		catch (std::exception& ex) {
			imgui_helpers::messageBox(ex.what(), "CSV Parser Error");
		}

		auto filename = file_names.find(std::pair<int, int>{type, fileid});
		if (filename != file_names.end()) {
			//ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(255, 0, 255, 255)));
			ImGui::Text("FILENAME -> %s", filename->second.c_str());
			ImGui::PopStyleColor();
		}

		auto found = library.find(type);
		if (found != library.end()) {
			//ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(0, 255, 0, 255)));
			ImGui::Text("-> %s", found->second);
			ImGui::PopStyleColor();
		}
	}
}