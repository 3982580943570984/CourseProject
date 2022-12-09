#include "include/BTree.hpp"
#include "include/Application.hpp"

#include "imgui.h"
#include "imgui_stdlib.h"
#include "implot.h"

#define STB_IMAGE_IMPLEMENTATION
#include "_stb_include/stb_image.h"

/*
#include "nlohmann/json.hpp"
#include "graphviz/cgraph.h"
*/

#include <bits/chrono.h>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <ios>
#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>

bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height)
{
    // Load from file
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
    if (image_data == NULL)
        return false;

    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

    // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    *out_texture = image_texture;
    *out_width = image_width;
    *out_height = image_height;

    return true;
}

std::pair<bool, std::int32_t> optimalBinarySearch (const std::vector<std::int32_t> array, const std::int32_t key) {
	if (array.empty())
		return { false, 0 };

	std::int32_t left { 0 }, right = array.size();
	std::int32_t mid { (left + right) >> 1 };
	
	while (left < right) {
		mid = (left + right) >> 1;
		key <= array[mid]
			? right = mid
			: left = mid + 1;
	}

	return key == array[right]
		? std::make_pair(true, right)
		: std::make_pair(false, -1);
};

void Application::StartUp () {
	ImGui::SetNextWindowPos(ImVec2(0.f,0.f));
	ImGui::SetNextWindowSize(ImVec2(1024.f, 768.f));
	ImGui::SetNextWindowContentSize(ImVec2(10.f, 10.f));

	static ImGuiStyle* style = &ImGui::GetStyle();
	style->Alpha = 0.8f;
};

void Application::Update () {
	std::int32_t unique_id { 0 };

	static bool active = true;
	static bool visualize = false;

	static std::vector<std::pair<std::string, std::int32_t>> inputRows = {
		{ "Insert value", 0 },
		{ "Remove value", 0 },
		{ "Modify value", 0 },
		{ "Search value", 0 }
	};

	static std::vector<std::int32_t> values (2);
	static std::vector<std::pair<std::string, std::int32_t*>> outputRows = {
		{ "Optimal Binary Search Time", &values[0] },
		{ "B-tree Search Time", &values[1] }
	};

	static std::vector<std::string> result = {
		"", "", "", "",
	};

	static BTree tree { 4 };
	static std::vector<std::int32_t> array;
	static std::int32_t new_value { 0 };

	static int my_image_width = 0;
	static int my_image_height = 0;
	static GLuint my_image_texture = 0;
	
	ImFont* font = ImGui::GetFont();
	font->Scale = 1.2f;
	ImGui::PushFont(font);
    if (ImGui::Begin("Course Project", &active, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar)) {
		if (ImGui::BeginTable("BTree", 1, 0, ImVec2(1006.f, 180.f))) {

			ImGui::TableNextRow(0, 15.f);

			/* Buttons */
			ImGui::Dummy(ImVec2 { 0.f, 10.f });
			ImGui::Separator();
			for (auto &[name, value] : inputRows) {
				ImGui::PushID(unique_id++);
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				name == "Modify value"
					? ImGui::PushItemWidth(215.f)
					: ImGui::PushItemWidth(480.f);
				ImGui::InputInt("", &value, 0, 0);
				ImGui::PopID();
				ImGui::SameLine();

				if (name == "Insert value") {
					if (ImGui::Button(name.c_str(), ImVec2{ 240.f, 20.f })) {
						tree.insert(value);
						result[0] = "Successful insert";

						if (!array.empty()) {
							std::size_t index { 0 };
							while (value > array[index] && index < array.size()) ++index;
							array.insert(array.cbegin() + index, value);
						} else {
							array.push_back(value);
						}

						tree.traverse();
					}
					ImGui::Text("%s", result[0].c_str());
				}

				if (name == "Remove value") {
					if (ImGui::Button(name.c_str(), ImVec2{ 240.f, 20.f })) {
						if (tree.search(value)) {
							tree.remove(value);
							result[1] = "Successful remove";
							array.erase(std::remove(array.begin(), array.end(), value), array.end());
							tree.traverse();
						} else {
							result[1] = "Value isn't present in tree";
						}
					}
					ImGui::Text("%s", result[1].c_str());
				}

				if (name == "Modify value") {
					ImGui::Text(" to ");
					ImGui::SameLine();
					ImGui::PushID(unique_id++);
					ImGui::InputInt("", &new_value, 0, 0);
					ImGui::PopID();
					ImGui::SameLine();
					if (ImGui::Button(name.c_str(), ImVec2{ 240.f, 20.f })) {
						if (tree.search(value)) {
							tree.modify(value, new_value);
							result[2] = "Successful modify";
							*std::find(array.begin(), array.end(), value) = new_value;
						} else {
							result[2] = "Value to be modified isnt' present in tree";
						}
					}
					ImGui::Text("%s", result[2].c_str());
				}
				
				if (name == "Search value") {
					if (ImGui::Button(name.c_str(), ImVec2{ 240.f, 20.f })) {
						auto start = std::chrono::steady_clock::now();
						tree.search(value)
							? result[3] = "Successful search"
							: result[3] = "Value isn't present in tree";
						auto end = std::chrono::steady_clock::now();
						values[0] = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

						start = std::chrono::steady_clock::now();
						optimalBinarySearch(array, value);
						end = std::chrono::steady_clock::now();
						values[1] = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
					}
					ImGui::Text("%s", result[3].c_str());
				}

				ImGui::PopItemWidth();
				ImGui::Dummy(ImVec2{ 0.f, 10.f} );
			}
			ImGui::Separator();

			ImGui::Dummy(ImVec2{ 0.f, 10.f} );

			/* Search time */
			for (auto &[text, value] : outputRows) {
				ImGui::PushID(unique_id++);
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				if (!(*value)) {
					centeredText(std::string{ text + " ?" });
				} else {
					centeredText(std::string{ text + ' ' + std::to_string(*value) });
				}
				ImGui::PopID();
			}
			ImGui::Dummy(ImVec2{ 0.f, 10.f });
			ImGui::Separator();

			ImGui::EndTable();
		}

		ImGui::Dummy(ImVec2{ 0.f, 10.f });
		ImGui::SetCursorPosX(272.f);
		if (ImGui::Button("Visualize B-tree", ImVec2{ 480.f, 20.f })) {
			visualize = !visualize;
			if (visualize) {
				std::string visualization = tree.visualize();

				std::ofstream graph;
				graph.open("../graph.dot", std::ofstream::out | std::ofstream::trunc);
				graph << "digraph g {\n" << "node [shape = record,height=.1];\n";
				graph << visualization;
				graph << "}";
				graph.close();
				
				std::system("dot -Tpng -O ../graph.dot");

				LoadTextureFromFile("../graph.dot.png", &my_image_texture, &my_image_width, &my_image_height);
			}
		}

		ImGui::Dummy(ImVec2{ 0.f, 10.f });
		ImGui::Separator();
		ImGui::Dummy(ImVec2{ 0.f, 10.f });
		ImGui::SetCursorPosX(272.f);
		if (ImGui::Button("Exit", ImVec2{ 480.f, 20.f })) {
			std::exit(0);
		}
		ImGui::Dummy(ImVec2{ 0.f, 10.f });
		ImGui::Separator();

		if (visualize) {
			if (ImGui::BeginChildFrame(++unique_id, ImVec2{ 1006.f, 300.f }, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoBackground)) {
				ImGui::Image((void*) (intptr_t) my_image_texture, ImVec2(my_image_width, my_image_height));
				ImGui::EndChildFrame();
			}
		}

		ImGui::PopFont();
		ImGui::End();
	}
};

void Application::centeredText (const std::string& text) {
    auto windowWidth = ImGui::GetWindowWidth();
    auto textWidth = ImGui::CalcTextSize(text.c_str()).x;
    ImGui::SetCursorPosX((windowWidth - textWidth) * .5f);
    ImGui::Text("%s", text.c_str());
};

/*
std::int32_t main () {
	Application application;
	application.Run();
	return 0;
};
*/
