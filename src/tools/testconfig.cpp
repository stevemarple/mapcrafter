/*
 * Copyright 2012-2014 Moritz Hilscher
 *
 * This file is part of Mapcrafter.
 *
 * Mapcrafter is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mapcrafter is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Mapcrafter.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "../mapcraftercore/config/mapcrafterconfig.h"

#include <iostream>
#include <string>

namespace config = mapcrafter::config;

int main(int argc, char **argv) {
	if (argc < 2) {
		std::cerr << "Usage: ./testconfig [configfile]" << std::endl;
		return 1;
	}

	std::string configfile = argv[1];

	config::MapcrafterConfig parser;
	config::ValidationMap validation;
	bool ok = parser.parse(configfile, validation);

	if (!validation.empty()) {
		if (ok)
			LOG(WARNING) << "Some notes on your configuration file:";
		else
			LOG(FATAL) << "Your configuration file is invalid!";
		validation.log();
		LOG(WARNING) << "Please read the documentation about the new configuration file format.";
	} else {
		LOG(INFO) << "Everything ok.";
	}

	std::cout << std::endl << "The parsed configuration file:" << std::endl;
	parser.dump(std::cout);

	return 0;
}
