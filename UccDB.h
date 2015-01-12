/*
 * UccDB.h
 *
 *  Created on: Jan 11, 2015
 *      Author: root
 */

#ifndef UCCDB_H_
#define UCCDB_H_

#include "mongo/client/dbclient.h" // for the driver
using namespace std;
using namespace mongo;

class UccDB {
	DBClientConnection connection;
	string dbUrl = "localHost";
	string dbName = "mydb";
public:
	UccDB(string dbUrl, string dbName);
	void runCommand(const BSONObj& cmd,
            BSONObj &info,
            int options=0);
};

#endif /* UCCDB_H_ */
