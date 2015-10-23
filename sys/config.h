#ifndef CONFIG_H_
#define CONFIG_H_

#include <string>

class Configuration {
	private:
		static const char CONFFILE[] = "db.conf";

		static Configuration *config = NULL;
		Configuration();

	public:
		std::string basepath;	// with trailing slash

		static Configuration* conf();
};

#endif
