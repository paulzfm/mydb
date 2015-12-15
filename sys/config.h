#ifndef CONFIG_H_
#define CONFIG_H_

#include "../util/common.h"

#define CONFFILE "mydb.conf"

class Configuration {
	private:
		static Configuration *config;
		Configuration();
		Configuration& operator = (const Configuration&);

	public:
		std::string basepath;	// with trailing slash

		static Configuration* conf();
};

#endif
