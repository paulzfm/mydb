#include "config.h"
#include "../lib/rapidjson/document.h"
#include "../lib/rapidjson/writer.h"
#include "../lib/rapidjson/stringbuffer.h"
#include <fstream>
#include <iostream>
#include <cstdlib>

Configuration* Configuration::config = NULL;

Configuration::Configuration() {
	std::ifstream fin(CONFFILE);
	if (fin.is_open()) {
		std::string strbuf;
		fin >> strbuf;
		fin.close();

		rapidjson::Document doc;
		doc.Parse(strbuf.c_str());
		if (doc.HasMember("basepath")) {
			basepath = doc["basepath"].GetString();

		} else {
			std::cerr << "[FATAL] `basepath` is required in config file but not found, exiting.." << std::endl;
			exit(2);
		}
		
	} else {
		std::cerr << "[FATAL] config file " << CONFFILE << " not found, exiting..." << std::endl;
		exit(1);
	}
}

Configuration* Configuration::conf() {
	if (config == NULL) config = new Configuration();
	return config;
}
