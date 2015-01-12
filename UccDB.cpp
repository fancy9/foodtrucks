/*
 * UccDB.cpp
 *
 *  Created on: Jan 11, 2015
 *      Author: root
 */

#include "UccDB.h"

UccDB::UccDB(string dbUrl, string dbName) {
	this->dbUrl = dbUrl;
	this->dbName = dbName;
	client::initialize();
	try {
		this->connection.connect(this->dbUrl);
	} catch (const mongo::DBException &e) {
		std::cout << "caught " << e.what() << std::endl;
	}

}

void UccDB::runCommand(const BSONObj& cmd, BSONObj& info, int options) {
	this->connection.runCommand(this->dbName, cmd, info, options);
}
